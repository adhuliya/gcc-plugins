#include <stdio.h>

int main(int argc, char **argv) {
    int x;
    int *z;

    x = 20;

    *z = x;

    printf("%d\n", *z);

    return 0;
}

