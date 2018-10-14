#ifndef CONFIG_H
#define CONFIG_H

#include <memory>
#include "AttractorSet.h"
#include "Variations.h"
#include "ColorMap.h"
#include "ColorMaps.h"

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

    static std::unique_ptr<Config> load(std::string const &pathname,
            ColorMaps const &colorMaps) {

        std::fstream f(pathname);
        if (!f) {
            std::cerr << "Config file not found: " << pathname << std::endl;
            return nullptr;
        }

        // Get color map.
        std::string colorMapName;
        f >> colorMapName;

        // Get the color map by name.
        auto map = colorMaps.get(colorMapName);
        if (!map) {
            std::cerr << "Can't find map " << colorMapName << std::endl;
            return nullptr;
        }

        // Read attractors.
        int attractorCount;
        f >> attractorCount;
        auto attractorSet = std::make_unique<AttractorSet>(attractorCount);

        bool equalProbability = false;
        for (int i = 0; i < attractorCount; i++) {
            double probability, colorMapValue;
            f >> probability >> colorMapValue;

            std::string attractorType;
            f >> attractorType;

            if (attractorType == "average") {
                attractorSet->set(i, std::make_unique<AverageAttractor>(f));
            } else if (attractorType == "complex") {
                attractorSet->set(i, std::make_unique<ComplexAttractor>(f));
            } else if (attractorType == "transform") {
                attractorSet->set(i, std::make_unique<TransformAttractor>(f));
            } else {
                std::cerr << "Unknown attractor type: "
                    << attractorType << std::endl;
                return nullptr;
            }

            if (probability == 0) {
                // If any probability is zero, make them all equal.
                equalProbability = true;
            } else {
                attractorSet->setProbability(i, probability);
            }
            attractorSet->setColorMapValue(i, colorMapValue);
        }

        if (equalProbability) {
            attractorSet->makeEqualProbability();
        }
        attractorSet->makeProbabilityMap();

        // Get variations.
        auto variations = std::make_unique<Variations>(f);

        return std::make_unique<Config>(
                std::move(attractorSet), std::move(variations), map);
    }
};

#endif // CONFIG_H
