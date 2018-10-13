#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <sstream>
#include <stdexcept>

/**
 * Represents the double-based 2D bounding box that can be expanded and manipulated.
 */
class BoundingBox {
    double mMinX;
    double mMinY;
    double mMaxX;
    double mMaxY;
    bool mIsSet;

public:
    BoundingBox()
        : mIsSet(false) {

        // Nothing.
    }

    BoundingBox(double minX, double minY, double maxX, double maxY)
        : mMinX(minX), mMinY(minY), mMaxX(maxX), mMaxY(maxY), mIsSet(true) {

        // Nothing.
    }

    /**
     * Grow the bounding box so as to include the point (x, y).
     */
    void grow(double x, double y) {
        if (!mIsSet) {
            mMinX = x;
            mMinY = y;
            mMaxX = x;
            mMaxY = y;
            mIsSet = true;
        } else {
            if (x < mMinX) mMinX = x;
            if (y < mMinY) mMinY = y;
            if (x > mMaxX) mMaxX = x;
            if (y > mMaxY) mMaxY = y;
        }
    }

    /**
     * The width of the bounding box.
     */
    double getWidth() const {
        assertInitialized();
        return mMaxX - mMinX;
    }

    /**
     * The height of the bounding box.
     */
    double getHeight() const {
        assertInitialized();
        return mMaxY - mMinY;
    }

    /**
     * The horizontal center of the bounding box.
     */
    double getXCenter() const {
        assertInitialized();
        return (mMinX + mMaxX)/2;
    }

    /**
     * The vertical center of the bounding box.
     */
    double getYCenter() const {
        assertInitialized();
        return (mMinY + mMaxY)/2;
    }

    /**
     * Given a point "x" inside the box, returns its fraction horizontally in the
     * box.
     */
    double normalizeX(double x) const {
        assertInitialized();
        return (x - mMinX)/getWidth();
    }

    /**
     * Given a point "y" inside the box, returns its fraction vertically in the
     * box.
     */
    double normalizeY(double y) const {
        assertInitialized();
        return (y - mMinY)/getHeight();
    }

    /**
     * Grow the box by the specified fraction around its center. Negative values shrink
     * the box.
     */
    void growBy(double fraction) {
        assertInitialized();

        double centerX = getXCenter();
        double centerY = getYCenter();
        double width = getWidth();
        double height = getHeight();

        mMinX = centerX - width*(1 + fraction)/2;
        mMaxX = centerX + width*(1 + fraction)/2;
        mMinY = centerY - height*(1 + fraction)/2;
        mMaxY = centerY + height*(1 + fraction)/2;
    }

    /**
     * Makes the bounding box square by growing it in one dimension or the other.
     */
    void makeSquare() {
        assertInitialized();

        double width = getWidth();
        double height = getHeight();

        if (width > height) {
            double centerY = getYCenter();
            mMinY = centerY - width/2;
            mMaxY = centerY + width/2;
        } else {
            double centerX = getXCenter();
            mMinX = centerX - height/2;
            mMaxX = centerX + height/2;
        }
    }

    /**
     * Stream a human-friendly version of the bounding box.
     */
    void stream_to(std::ostream &s) const {
        if (!mIsSet) {
            s << "not set";
        } else {
            s << "(" << mMinX << ", " << mMinY << ")-(" << mMaxX << ", " << mMaxY << ")";
        }
    }

private:
    /**
     * Make sure that the bounding box is initialized. Throws std::logic_error if not.
     */
    void assertInitialized() const {
        if (!mIsSet) {
            throw std::logic_error("The bounding box is not initialized");
        }
    }
};

/**
 * Stream a human-friendly version of the bounding box.
 */
std::ostream &operator<<(std::ostream &s, const BoundingBox &bbox) {
    bbox.stream_to(s);

    return s;
}

#endif // BOUNDING_BOX_H
