#ifndef VARIATIONS_H
#define VARIATIONS_H

#include <math.h>

/**
 * Maintains a set of coefficients for variations later applied to points.
 */
class Variations {
    /// public static final int COEFFICIENT_COUNT = 7;
    static constexpr double EPS = 1e-10;
    double a, b, c, d, e, f, g;

public:
    Variations(double a, double b, double c, double d, double e, double f, double g) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
        this->g = g;
    }

    /**
     * Modifies the point by a blend of a bunch of variations.
     */
    void transform(double &x, double &y) {
        double tx = x;
        double ty = y;
        x = 0;
        y = 0;

        if (a > 0) {
            // Linear
            x += a*tx;
            y += a*ty;
        }

        if (b > 0) {
            // Sinunoidal
            double nx = sin(tx);
            double ny = sin(ty);
            x += b*nx;
            y += b*ny;
        }

        if (c > 0) {
            // Complex
            double r2 = tx*tx + ty*ty + 1e-6;
            double nx = tx/r2;
            double ny = ty/r2;
            x += c*nx;
            y += c*ny;
        }

        if (d > 0) {
            // Swirl
            double r2 = tx*tx + ty*ty;
            double c1 = sin(r2);
            double c2 = cos(r2);
            double nx = c1*tx - c2*ty;
            double ny = c2*tx + c1*ty;
            x += d*nx;
            y += d*ny;
        }

        if (e > 0) {
            // Horseshoe
            double at;
            if (tx < -EPS || tx > EPS ||
                ty < -EPS || ty > EPS) {

                at = atan2(tx, ty);
            } else {
                at = 0;
            }

            double c1 = sin(at);
            double c2 = cos(at);
            double nx = c1*tx - c2*ty;
            double ny = c2*tx + c1*ty;
            x += e*nx;
            y += e*ny;
        }

        if (f > 0) {
            // Untitled
            double nx;
            if (tx < -EPS || tx > EPS ||
                ty < -EPS || ty > EPS) {

                nx = atan2(tx, ty) / M_PI;
            } else {
                nx = 0;
            }
            double ny = sqrt(tx*tx + ty*ty) - 1.0;
            x += f*nx;
            y += f*ny;
        }

        if (g > 0) {
            // Bent
            double nx = tx;
            double ny = ty;
            if (nx < 0) nx *= 2;
            if (ny < 0) ny /= 2;
            x += g*nx;
            y += g*ny;
        }
    }
};

#endif // VARIATIONS_H
