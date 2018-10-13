#ifndef COLOR_MAPS_H
#define COLOR_MAPS_H

#include <memory>
#include <string>
#include <map>
#include <fstream>
#include "ColorMap.h"

class ColorMaps {
    std::map<std::string, std::shared_ptr<const ColorMap>> mMaps;

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
            std::shared_ptr<ColorMap> map = std::make_shared<ColorMap>();

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
                return false;
            }
        }

        return true;
    }

    /**
     * Get a color map by name, or null if not found.
     */
    std::shared_ptr<const ColorMap> get(const std::string &name) const {
        return mMaps.at(name);
    }
};

#endif // COLOR_MAPS_H
