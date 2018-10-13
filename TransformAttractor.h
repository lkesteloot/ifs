#ifndef TRANSFORM_ATTRACTOR_H
#define TRANSFORM_ATTRACTOR_H

#include "Attractor.h"

/**
 * A transformer that uses a matrix:
 *
 * | a  b  e |
 * | c  d  f |
 *
 * The matrix is multiplied with the point | x y 1 |.
 */
class TransformAttractor : public Attractor {
    double a, b, c, d, e, f;

public:
    TransformAttractor(double a, double b, double c,
            double d, double e, double f) {

        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
    }

    TransformAttractor(double x[]) {
        this->a = x[0];
        this->b = x[1];
        this->c = x[2];
        this->d = x[3];
        this->e = x[4];
        this->f = x[5];
    }

    virtual void transform(double &x, double &y) const {
        double new_x = a*x + b*y + e;
        double new_y = c*x + d*y + f;

        // Move to the inverse magnitude. (Why?)
        double r2 = 1/(new_x*new_x + new_y*new_y + 1e-6);

        x = new_x*r2;
        y = new_y*r2;
    }
};

#endif // TRANSFORM_ATTRACTOR_H
