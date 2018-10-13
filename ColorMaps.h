#ifndef COLOR_MAPS_H
#define COLOR_MAPS_H

#include <string>
#include <map>
#include <fstream>
#include "ColorMap.h"

class ColorMaps {
    std::map<std::string, ColorMap *> mMaps;

public:
    /**
     * Reads the color map file, returning whether successful.
     */
    bool read(const std::string &pathname) {
        std::ifstream f(pathname);

        if (!f) {
            std::cerr << "Colormap file " << pathname << " cannot be opened." << std::endl;
            return false;
        }

        while (!f.eof()) {
            ColorMap *map = new ColorMap();

            bool success = map->read(f);
            if (success) {
                mMaps[map->getTitle()] = map;

                // Skip blank line.
                std::string blank_line;
                std::getline(f, blank_line);
                if (!blank_line.empty()) {
                    std::cerr << "Line should have been blank in colormap file: "
                        << blank_line << std::endl;
                    return false;
                }
            } else {
                delete map;
                return false;
            }
        }

        return true;
    }

    /**
     * Get a color map by name, or null if not found.
     */
    ColorMap *get(const std::string &name) {
        return mMaps[name];
    }
};

#endif // COLOR_MAPS_H
