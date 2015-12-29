
#include <iostream>

int main(int argc, char **argv) {
    int a = 12;
    int *p = &a;

    *p *= 2;

    std::cout << "Value of a = " << *p;

    return 0;
}
