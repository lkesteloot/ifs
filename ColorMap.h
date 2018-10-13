#ifndef COLOR_MAP_H
#define COLOR_MAP_H

#include <cstdint>
#include <string>
#include <istream>
#include <sstream>
#include <vector>

class ColorMap {
    static constexpr int ENTRY_COUNT = 256;
    std::string mTitle;
    std::vector<uint8_t> mRgb;

public:
    ColorMap()
        : mRgb(ENTRY_COUNT*3) {

        // Nothing.
    }

    // The reader should point to the first line of the
    // map, which is the title. The title should be
    // followed by 256 lines of RGB triplets.  This
    // function stops reading after the 256th line.
    //
    // Returns whether successful.
    bool read(std::istream &is) {
        std::getline(is, mTitle);
        if (is.eof()) {
            std::cerr << "Can't read title of colormap" << std::endl;
            return false;
        }

        for (int i = 0; i < ENTRY_COUNT; i++) {
            std::string line;
            std::getline(is, line);
            if (is.eof() && i != ENTRY_COUNT - 1) {
                std::cerr << "Unexpected end of file part-way through colormap "
                    << mTitle << std::endl;
                return false;
            }

            std::stringstream s(line);
            int red, green, blue;

            s >> red >> green >> blue;
            if (!s.eof()) {
                return false;
            }

            mRgb[i*3 + 0] = red;
            mRgb[i*3 + 1] = green;
            mRgb[i*3 + 2] = blue;
        }

        return true;
    }

    std::string getTitle() const {
        return mTitle;
    }

    void getColor(int index, uint8_t &red, uint8_t &green, uint8_t &blue) const {
        red = mRgb[index*3 + 0];
        green = mRgb[index*3 + 1];
        blue = mRgb[index*3 + 2];
    }
};

#endif // COLOR_MAP_H
