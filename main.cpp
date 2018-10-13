
#include <memory>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <thread>
#include <unistd.h>
#include "Image.h"
#include "AttractorSet.h"
#include "BoundingBox.h"
#include "Variations.h"
#include "ColorMaps.h"
#include "Config.h"
#include "Timer.h"

#ifdef DISPLAY
#include "MiniFB.h"
#endif

static const bool INTERACTIVE = true;
static const uint64_t FEW_SECONDS_ITERATIONS = INTERACTIVE ? -1 : 25000000LL;
static const uint64_t ITERATION_UPDATE = 10000000LL;
static const int FUSE_LENGTH = 10000;
static const int WIDTH = 256*2;
static const int HEIGHT = 256*2;

static bool g_done = false;

static BoundingBox computeBoundingBox(Config const &config) {
    std::cout << "Finding the bounding box..." << std::endl;

    BoundingBox bbox;

    // Starting point.
    double x = 0;
    double y = 0;

    // Find the bounding box
    // XXX do a better job of ignoring outliers. Fuse length isn't enough.
    const int SAMPLE_COUNT = 10000;
    std::vector<double> x_history(SAMPLE_COUNT);
    std::vector<double> y_history(SAMPLE_COUNT);
    for (int i = 0; i < FUSE_LENGTH + SAMPLE_COUNT; i++) {
        if (i >= FUSE_LENGTH) {
            x_history[i - FUSE_LENGTH] = x;
            y_history[i - FUSE_LENGTH] = y;
            bbox.grow(x, y);
        }

        Attractor const &attractor = config.attractorSet().choose();
        attractor.transform(x, y);
        config.variations().transform(x, y);
    }

    bbox.growBy(0.05);  // 5% larger
    bbox.makeSquare();

    std::cout << "Computed bounding box (strict): " << bbox << std::endl;

    std::sort(x_history.begin(), x_history.end());
    std::sort(y_history.begin(), y_history.end());

    const double PERCENTILE = 0.1;
    const int skip = (int) (PERCENTILE*SAMPLE_COUNT/100 + 0.5);
    bbox = BoundingBox(x_history[skip], y_history[skip],
            x_history[SAMPLE_COUNT - skip - 1], y_history[SAMPLE_COUNT - skip - 1]);
    bbox.growBy(0.05);  // 5% larger
    bbox.makeSquare();
    std::cout << "Computed bounding box (percentile): " << bbox << std::endl;

    return bbox;
}

static void render(Image &image, Config const &config,
        const BoundingBox &bbox, int seed) {

    // Initialize the seed for our thread.
    init_rand(seed);

    // Color (0-1) in the color map.
    double colorMapValue = 0;

    // Starting point.
    double x = 0;
    double y = 0;

    for (uint64_t i = 0; !g_done && i < FEW_SECONDS_ITERATIONS; i++) {
        Attractor const &attractor = config.attractorSet().choose();
        attractor.transform(x, y);
        config.variations().transform(x, y);

        // Map to pixel.
        int ix = (int) (bbox.normalizeX(x)*(WIDTH - 1) + 0.5);
        int iy = (int) ((1 - bbox.normalizeY(y))*(HEIGHT- 1) + 0.5);

        // Move half-way to new color value.
        double newColorMapValue = attractor.getColorMapValue();
        colorMapValue = (colorMapValue + newColorMapValue)/2;

        if (image.isInBounds(ix, iy) && i >= FUSE_LENGTH) {
            // Look up RGB color.
            int colorIndex = (int) (colorMapValue*255 + 0.5);

            linear_color red, green, blue;
            config.colorMap().getColor(colorIndex, red, green, blue);
            image.touchPixel(ix, iy, red, green, blue);
        }

        if (!INTERACTIVE && i % ITERATION_UPDATE == 0 && i != 0) {
            std::cout << (i*100/FEW_SECONDS_ITERATIONS) << "%" << std::endl;
        }
    }
}

int main(/* int argc, char *argv[] */) {
    // Number of threads to use.
    int thread_count = std::thread::hardware_concurrency();
    std::cout << "Using " << thread_count << " threads.\n";

    // Load all color maps.
    ColorMaps colorMaps;
    bool success = colorMaps.read("ColorMap.txt");
    if (!success) {
        std::cout << "Cannot read color map file." << std::endl;
        return -1;
    }

    // Get the color map by name.
    auto map = colorMaps.get("wooden-highlight");

    // Get the attractor set.
    /// auto attractorSet = AttractorSet::makeFernAttractors();
    /// auto attractorSet = AttractorSet::makeSierpinskiAttractors();
    auto attractorSet = AttractorSet::makeLeafAttractors3();

    // Get variations.
    auto variations = std::make_unique<Variations>(0, 0, 0, 0, 1, 0, 0);

    auto config = std::make_unique<Config>(
            std::move(attractorSet), std::move(variations), map);

    // Compute bounding box.
    BoundingBox bbox = computeBoundingBox(*config);

#ifdef DISPLAY
    if (INTERACTIVE) {
        if (!mfb_open("ifs", WIDTH, HEIGHT)) {
            std::cerr << "Failed to open the display.\n";
            return -1;
        }
    }
#endif

    // Generate the image on multiple threads.
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<Image>> images;
    for (int t = 0; t < thread_count; t++) {
        images.emplace_back(std::make_unique<Image>(WIDTH, HEIGHT));
        threads.emplace_back(render, std::ref(*images.back()),
                std::cref(*config), std::cref(bbox), random());
    }

    if (INTERACTIVE) {
        while (!g_done) {
            // Time this update work.
            Timer timer;

            // Blend images.
            Image image(WIDTH, HEIGHT);
            for (int t = 0; t < thread_count; t++) {
                image.add(*images[t]);
            }

            // Simulate film exposure.
            image.brightenDarks();

            // Convert to 8-bit.
            std::vector<gamma_color> bgra;
            image.toBgra(bgra);

            int state = mfb_update(&bgra[0]);
            if (state < 0) {
                // Tell threads to quit.
                g_done = true;
            } else {
                double elapsed = timer.elapsed();

                std::cerr << "Update took " <<
                    std::fixed << std::setprecision(elapsed < 0.2 ? 3 : 1) << elapsed << " seconds.\n";
                usleep(1000*1000);
            }
        }

        // Wait for threads to finish.
        for (int t = 0; t < thread_count; t++) {
            threads[t].join();
        }
    } else {
        // Wait for worker threads to quit, then blend images.
        Image image(WIDTH, HEIGHT);
        for (int t = 0; t < thread_count; t++) {
            threads[t].join();
            image.add(*images[t]);
        }

        std::cout << "Brightening darks..." << std::endl;
        image.brightenDarks();

        // Save the final image.
        success = image.save("out.png");
        if (!success) {
            std::cerr << "Cannot write output image.\n";
        }
    }

    return 0;
}
