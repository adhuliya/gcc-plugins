#include <iostream>

int sum(int, int);

int main(int argc, char **argv) {
    int x = 10;

    std::cout << x << std::endl;

    std::cout << sum(10, 20) << std::endl;

    return 0;
}

int sum(int a , int b) {
    return a + b;
}
