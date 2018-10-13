
#include <iostream>
#include "Image.h"

#ifdef DISPLAY
#include "MiniFB.h"
#endif

static const int WIDTH = 256;
static const int HEIGHT = 256;

int main(int argc, char *argv[]) {
    Image image(WIDTH, HEIGHT);

    bool success = image.save("out.png");
    if (!success) {
        std::cerr << "Cannot write output image.\n";
    }

    return 0;
}
