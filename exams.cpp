#include <iostream>

void exam1() {
    auto swap = []<typename T>(T& x, T& y) {T t=x; x=y; y=t;};

    int x=1, y=2;
    swap(x, y);

    std::cout << x << std::endl;
    std::cout << y << std::endl;
}

void exams() {
    exam1();
}