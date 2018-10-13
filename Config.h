#ifndef CONFIG_H
#define CONFIG_H

#include <memory>
#include "AttractorSet.h"
#include "Variations.h"
#include "ColorMap.h"

// Configuration for this image.
class Config {
    std::unique_ptr<AttractorSet> mAttractorSet;
    std::unique_ptr<Variations> mVariations;
    std::shared_ptr<const ColorMap> mColorMap;

public:
    Config(std::unique_ptr<AttractorSet> &&attractorSet,
            std::unique_ptr<Variations> &&variations,
            std::shared_ptr<const ColorMap> colorMap)
        : mAttractorSet(std::move(attractorSet)),
        mVariations(std::move(variations)),
        mColorMap(colorMap) {

        // Nothing.
    }

    AttractorSet const &attractorSet() const {
        return *mAttractorSet;
    }

    Variations const &variations() const {
        return *mVariations;
    }

    ColorMap const &colorMap() const {
        return *mColorMap;
    }
};

#endif // CONFIG_H
