/*
1. numbers에 수학 상수 추가
2. 구현 여부가 불확실한(미래에 구현 될 수 있는) feature들이 구현되어 있는지 확인할 수 있는 macro 제공
*/

#include <iostream>
#include <numbers>

void numbers() {

    std::cout << std::numbers::pi << std::endl;
    std::cout << std::numbers::e << std::endl;
    std::cout << std::numbers::sqrt2 << std::endl;
}

#include <span>
#include <chrono>
void future_test_macro() {
#ifdef __cpp_concepts
    std::cout << "support concepts " << __cpp_concepts << std::endl;
#endif
#ifdef __cpp_modules
    std::cout << "support modules " << __cpp_modules << std::endl;
#endif
#ifdef __cpp_lib_span
    std::cout << "support span lib " << __cpp_lib_span << std::endl;
#endif
#ifdef __cpp_lib_chrono
    std::cout << "support chrono lib " << __cpp_lib_chrono << std::endl;
#endif
}

void preview() {
    numbers();
    future_test_macro();
}
