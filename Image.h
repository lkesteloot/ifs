#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>
#include "stb_image_write.h"

// Image with doubles for RGB and count.
class Image {
    int mWidth;
    int mHeight;
    int mPixelCount;
    std::vector<double> mRed;
    std::vector<double> mGreen;
    std::vector<double> mBlue;
    std::vector<int> mCount;

public:
    Image(int width, int height)
        : mWidth(width), mHeight(height), mPixelCount(width*height),
        mRed(mPixelCount), mGreen(mPixelCount), mBlue(mPixelCount), mCount(mPixelCount)
    {
        // Nothing.
    }

    // Saves the image to the pathname as a PNG file, returning
    // whether successful.
    bool save(const std::string &pathname) const {
        std::vector<unsigned char> rgb(mPixelCount*3);

        for (int i = 0; i < mPixelCount; i++) {
            rgb[i*3] = i % 256;
        }

        int success = stbi_write_png(pathname.c_str(),
                mWidth, mHeight, 3, &rgb[0], mWidth*3);

        return success;
    }
};

#endif // IMAGE_H
