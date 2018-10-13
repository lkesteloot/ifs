#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <math.h>
#include "stb_image_write.h"
#include "util.h"

// Image with 64-bit values for RGB.
class Image {
    int mWidth;
    int mHeight;
    int mPixelCount;
    std::vector<uint64_t> mRed;
    std::vector<uint64_t> mGreen;
    std::vector<uint64_t> mBlue;
    std::vector<uint64_t> mCount;

public:
    Image(int width, int height)
        : mWidth(width), mHeight(height), mPixelCount(width*height),
        mRed(mPixelCount), mGreen(mPixelCount), mBlue(mPixelCount), mCount(mPixelCount)
    {
        // Nothing.
    }

    /**
     * Returns whether the pixel (x, y) is within the image.
     */
    bool isInBounds(int x, int y) const {
        return x >= 0 && y >= 0 && x < mWidth && y < mHeight;
    }

    /**
     * Add some color to a pixel.
     */
    void touchPixel(int x, int y, linear_color red, linear_color green, linear_color blue) {
        int index = y*mWidth + x;

        mRed[index] += red;
        mGreen[index] += green;
        mBlue[index] += blue;
        mCount[index] += 1;
    }

    // Add other image data to ours.
    void add(const Image &other) {
        if (other.mWidth != mWidth || other.mHeight != mHeight) {
            throw std::logic_error("The image sizes must match");
        }

        for (int i = 0; i < mPixelCount; i++) {
            mRed[i] += other.mRed[i];
            mGreen[i] += other.mGreen[i];
            mBlue[i] += other.mBlue[i];
            mCount[i] += other.mCount[i];
        }
    }

    /**
     * Multiply image by log of its count so that darks get brighter.
     */
    void brightenDarks() {
        for (int i = 0; i < mPixelCount; i++) {
            uint64_t count = mCount[i];

            if (count > 0) {
                // Multiply by log to brighten the darks and simulate film
                // exposure. Add 1 to avoid negative values.
                double mult = log(1.0 + count)/count;

                mRed[i] = (int) (mRed[i]*mult);
                mGreen[i] = (int) (mGreen[i]*mult);
                mBlue[i] = (int) (mBlue[i]*mult);
            }
        }
    }

    void toRgb(std::vector<gamma_color> &rgb) const {
        // Find max so we can normalize whole image.
        uint64_t max = getMaxComponent();
        double invCount = max == 0 ? 0 : 1.0/max;

        rgb.resize(mPixelCount*3);

        for (int i = 0; i < mPixelCount; i++) {
            // Gamma correct.
            rgb[i*3 + 0] = (int) (255.99*sqrt(mRed[i]*invCount));
            rgb[i*3 + 1] = (int) (255.99*sqrt(mGreen[i]*invCount));
            rgb[i*3 + 2] = (int) (255.99*sqrt(mBlue[i]*invCount));
        }
    }

    void toBgra(std::vector<gamma_color> &bgra) const {
        // Find max so we can normalize whole image.
        uint64_t max = getMaxComponent();
        double invCount = max == 0 ? 0 : 1.0/max;

        bgra.resize(mPixelCount*4);

        for (int i = 0; i < mPixelCount; i++) {
            // Gamma correct.
            bgra[i*4 + 0] = (int) (255.99*sqrt(mBlue[i]*invCount));
            bgra[i*4 + 1] = (int) (255.99*sqrt(mGreen[i]*invCount));
            bgra[i*4 + 2] = (int) (255.99*sqrt(mRed[i]*invCount));
            bgra[i*4 + 3] = 255;
        }
    }

    // Saves the image to the pathname as a PNG file, returning
    // whether successful.
    bool save(const std::string &pathname) const {
        std::vector<gamma_color> rgb;
        toRgb(rgb);

        int success = stbi_write_png(pathname.c_str(),
                mWidth, mHeight, 3, &rgb[0], mWidth*3);

        return success;
    }

private:
    uint64_t getMaxComponent() const {
        uint64_t max = 0;

        for (int i = 0; i < mPixelCount; i++) {
            if (mRed[i] > max) max = mRed[i];
            if (mGreen[i] > max) max = mGreen[i];
            if (mBlue[i] > max) max = mBlue[i];
        }

        return max;
    }
};

#endif // IMAGE_H
