#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <vector>
#include <string>
#include "stb_image_write.h"

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
    void touchPixel(int x, int y, int red, int green, int blue) {
        int index = y*mWidth + x;

        mRed[index] += red;
        mGreen[index] += green;
        mBlue[index] += blue;
        mCount[index] += 1;
    }

    // Saves the image to the pathname as a PNG file, returning
    // whether successful.
    bool save(const std::string &pathname) const {
        std::vector<unsigned char> rgb(mPixelCount*3);

        for (int i = 0; i < mPixelCount; i++) {
            if (mCount[i] == 0) {
                rgb[i*3 + 0] = 0;
                rgb[i*3 + 1] = 0;
                rgb[i*3 + 2] = 0;
            } else {
                double invCount = 1.0/mCount[i];

                rgb[i*3 + 0] = (int) (mRed[i]*invCount);
                rgb[i*3 + 1] = (int) (mGreen[i]*invCount);
                rgb[i*3 + 2] = (int) (mBlue[i]*invCount);
            }
        }

        int success = stbi_write_png(pathname.c_str(),
                mWidth, mHeight, 3, &rgb[0], mWidth*3);

        return success;
    }
};

#endif // IMAGE_H
