#ifndef ATTRACTOR_H
#define ATTRACTOR_H

// Base class of attractors that can transform 2D points to other 2D points.
class Attractor {
    double mProbability;
    double mColorMapValue;

public:
    virtual ~Attractor() {
        // Nothing.
    }

    // Transform a point in-place.
    virtual void transform(double &x, double &y) const = 0;

    // Probability of choosing this attractor in the set it's contained in.
    void setProbability(double p) {
        mProbability = p;
    }

    double getProbability() {
        return mProbability;
    }

    // 0 to 1 value into the color map.
    void setColorMapValue(double colorMapValue) {
        mColorMapValue = colorMapValue;
    }

    double getColorMapValue() {
        return mColorMapValue;
    }
};

#endif // ATTRACTOR_H
