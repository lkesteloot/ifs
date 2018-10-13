
#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Image.h"
#include "AttractorSet.h"
#include "BoundingBox.h"
#include "Variations.h"

#ifdef DISPLAY
#include "MiniFB.h"
#endif

static const uint64_t FEW_SECONDS_ITERATIONS = 25000000LL;
static const int FUSE_LENGTH = 10000;
static const int WIDTH = 256*2;
static const int HEIGHT = 256*2;

static BoundingBox computeBoundingBox(AttractorSet *attractorSet, Variations *variations) {
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

        Attractor *attractor = attractorSet->choose();
        attractor->transform(x, y);
        variations->transform(x, y);
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

int main(int argc, char *argv[]) {
    Image image(WIDTH, HEIGHT);

    // Get the attractor set.
    /// AttractorSet *attractorSet = AttractorSet::makeFernAttractors();
    /// AttractorSet *attractorSet = AttractorSet::makeSierpinskiAttractors();
    AttractorSet *attractorSet = AttractorSet::makeLeafAttractors3();

    // Get variations.
    Variations *variations = new Variations(0, 0, 0, 0, 1, 0, 0);

    // Compute bounding box.
    BoundingBox bbox = computeBoundingBox(attractorSet, variations);

    // Color (0-1) in the color map.
    double colorMapValue = 0;

    // Starting point.
    double x = 0;
    double y = 0;

    for (uint64_t i = 0; i < FEW_SECONDS_ITERATIONS; i++) {
        Attractor *attractor = attractorSet->choose();
        attractor->transform(x, y);
        variations->transform(x, y);
        /// std::cout << attractor << " " << x << " " << y << std::endl;

        // Map to pixel.
        int ix = (int) (bbox.normalizeX(x)*(WIDTH - 1) + 0.5);
        int iy = (int) ((1 - bbox.normalizeY(y))*(HEIGHT- 1) + 0.5);

        // Move half-way to new color value.
        double newColorMapValue = attractor->getColorMapValue();
        colorMapValue = (colorMapValue + newColorMapValue)/2;

        if (image.isInBounds(ix, iy) && i >= FUSE_LENGTH) {
            // Look up RGB color.
            int colorIndex = (int) (colorMapValue*255 + 0.5);
            /// short[] color = map.getColor(colorIndex);

            image.touchPixel(ix, iy, colorIndex, colorIndex, colorIndex);
        }
    }

    bool success = image.save("out.png");
    if (!success) {
        std::cerr << "Cannot write output image.\n";
    }

    return 0;
}
