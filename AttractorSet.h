#ifndef ATTRACTOR_SET_H
#define ATTRACTOR_SET_H

#include <vector>
#include "util.h"
#include "Attractor.h"
#include "TransformAttractor.h"
#include "AverageAttractor.h"
#include "ComplexAttractor.h"

/**
 * A list of attractors and their relative probability.
 */
class AttractorSet {
    static const int PROBABILITY_MAP_SIZE = 1024;
    /**
     * List of attractors.
     */
    std::vector<std::unique_ptr<Attractor>> mAttractors;
    /**
     * List of indexes into the "mAttractors" list, in proportion to their
     * desired probability.
     */
    std::vector<int> mProbabilityMap;

public:
    AttractorSet(int size)
        : mAttractors(size), mProbabilityMap(PROBABILITY_MAP_SIZE) {

        // Nothing.
    }

    /**
     * Set all attractors to have equal probability.
     */
    void makeEqualProbability() {
        double probability = 1.0/mAttractors.size();

        for (auto const &a : mAttractors) {
            a->setProbability(probability);
        }
    }

    /**
     * Convert the individual probabilities into a map of indexes into the
     * attractor array.
     */
    void makeProbabilityMap() {
        double increment = 1.0/PROBABILITY_MAP_SIZE;
        double value = 0;
        unsigned index = 0;
        double target = mAttractors[index]->getProbability();

        for (int i = 0; i < PROBABILITY_MAP_SIZE; i++) {
            while (value >= target && index < mAttractors.size() - 1) {
                index++;
                target += mAttractors[index]->getProbability();
            }

            mProbabilityMap[i] = index;
            value += increment;
        }
    }

    /**
     * Return a random attractor.
     */
    Attractor const &choose() const {
        return *mAttractors[mProbabilityMap[my_randl() % PROBABILITY_MAP_SIZE]];
    }

    /**
     * Set the attractor at the specified index.
     */
    void set(int index, std::unique_ptr<Attractor> &&attractor) {
        mAttractors[index] = std::move(attractor);
    }

    /**
     * Return the specified attractor.
     */
    Attractor const &get(int index) {
        return *mAttractors[index];
    }

    /**
     * Set the probability of the specified attractor. All probabilities must sum to 1.0.
     */
    void setProbability(int index, double probability) {
        mAttractors[index]->setProbability(probability);
    }

    /**
     * Set the color map value (0 to 1) of the specified attractor.
     */
    void setColorMapValue(int index, double colorMapValue) {
        mAttractors[index]->setColorMapValue(colorMapValue);
    }

#if 0
    static AttractorSet *makeFlameTestAttractors(double param) {
        AttractorSet *a = new AttractorSet(4);

        double[][][] coefs = { {
            { -0.681206, -0.0779465, 0.20769, 0.755065, -0.0416126, -0.262334 },
            { 0.953766, 0.48396, 0.43268, -0.0542476, 0.642503, -0.995898 },
            { 0.840613, -0.816191, 0.318971, -0.430402, 0.905589, 0.909402 },
            { 0.960492, -0.466555, 0.215383, -0.727377, -0.126074, 0.253509 },
        }, {
            { 0.0779465, -0.681206, -0.755065, 0.20769, -0.0416126, -0.262334 },
            { -0.48396, 0.953766, 0.0542476, 0.43268, 0.642503, -0.995898 },
            { 0.816191, 0.840613, 0.430402, 0.318971, 0.905589, 0.909402 },
            { 0.466555, 0.960492, 0.727377, 0.215383, -0.126074, 0.253509 },
        }, {
            { 0.681206, 0.0779465, -0.20769, -0.755065, -0.0416126, -0.262334 },
            { -0.953766, -0.48396, -0.43268, 0.0542476, 0.642503, -0.995898 },
            { -0.840613, 0.816191, -0.318971, 0.430402, 0.905589, 0.909402 },
            { -0.960492, 0.466555, -0.215383, 0.727377, -0.126074, 0.253509 },
        }, {
            { -0.0779465, 0.681206, 0.755065, -0.20769, -0.0416126, -0.262334 },
            { 0.48396, -0.953766, -0.0542476, -0.43268, 0.642503, -0.995898 },
            { -0.816191, -0.840613, -0.430402, -0.318971, 0.905589, 0.909402 },
            { -0.466555, -0.960492, -0.727377, -0.215383, -0.126074, 0.253509 },
        }, {
            { -0.681206, -0.0779465, 0.20769, 0.755065, -0.0416126, -0.262334 },
            { 0.953766, 0.48396, 0.43268, -0.0542476, 0.642503, -0.995898 },
            { 0.840613, -0.816191, 0.318971, -0.430402, 0.905589, 0.909402 },
            { 0.960492, -0.466555, 0.215383, -0.727377, -0.126074, 0.253509 },
        } };

        param *= 4;
        int set;
        double interp;

        if (param >= 4) {
            set = 3;
            interp = 1;
        } else {
            set = (int)param;
            interp = param - set;
        }

        double[] params = new double[6];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 6; j++) {
                params[j] = (1 - interp)*coefs[set][i][j] +
                    interp*coefs[set + 1][i][j];
            }

            a->set(i, new TransformAttractor(params));
        }

        a->setColorMapValue(0, 1);
        a->setColorMapValue(1, 0);
        a->setColorMapValue(2, 0);
        a->setColorMapValue(3, 0);

        a->makeEqualProbability();
        a->makeProbabilityMap();

        return a;
    }
#endif
};

#endif // ATTRACTOR_SET_H
