#ifndef AVERAGE_ATTRACTOR_H
#define AVERAGE_ATTRACTOR_H

#include "Attractor.h"

/**
 * An attractor that moves the point half-way between its current location and
 * the target location.
 */
class AverageAttractor : public Attractor {
    double mTx;
    double mTy;

public:
    AverageAttractor(double tx, double ty)
        : mTx(tx), mTy(ty) {

        // Nothing.
    }

    virtual void transform(double &x, double &y) const {
        x = (x + mTx)/2;
        y = (y + mTy)/2;
    }
};

#endif // AVERAGE_ATTRACTOR_H
