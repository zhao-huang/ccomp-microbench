#include "strucs.h"

int main(int argc, char **argv)
{
    struct goober g;
    for (int i = 0; i < 500000000; i++) {
        testFunc(&g);
    }
    return 0;
}
