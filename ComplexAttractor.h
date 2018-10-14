#ifndef COMPLEX_ATTRACTOR_H
#define COMPLEX_ATTRACTOR_H

#include "Attractor.h"

/**
 * Attractor that treats the point like a complex number (with X the real value
 * and Y the imaginary value). The constructor takes two values, S and A.
 * The transform brings the point a fraction of the way to A. If S is (0,0),
 * the point is moved to A. If it's (1,0), it stays where it is.
 */
class ComplexAttractor : public Attractor {
    double mSr;
    double mSi;
    double mAr;
    double mAi;

public:
    ComplexAttractor(double sr, double si, double ar, double ai)
        : mSr(sr), mSi(si), mAr(ar), mAi(ai) {

        // Nothing.
    }

    ComplexAttractor(std::istream &is) {
        is >> mSr >> mSi >> mAr >> mAi;
    }

    virtual void transform(double &x, double &y) const {
        // P' = S*P + (1 - S)*A
        //    = S*P + A - S*A
        //    = S*(P - A) + A
        double new_x = multReal(mSr, mSi, x, y) + multReal(1 - mSr, mSi, mAr, mAi);
        double new_y = multImg(mSr, mSi, x, y) + multImg(1 - mSr, mSi, mAr, mAi);

        x = new_x;
        y = new_y;
    }

private:
    /**
     * Real part of complex product.
     */
    static double multReal(double ar, double ai, double br, double bi) {
        return ar*br - ai*bi;
    }

    /**
     * Imaginary part of complex product.
     */
    static double multImg(double ar, double ai, double br, double bi) {
        return ar*bi + ai*br;
    }
};

#endif // COMPLEX_ATTRACTOR_H
