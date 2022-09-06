#include <iostream>

#include <numbers>
void test_numbers() {

    std::cout << std::numbers::pi << std::endl;
    std::cout << std::numbers::e << std::endl;
    std::cout << std::numbers::sqrt2 << std::endl;
}

#include <span>
#include <chrono>
void test_future_test_macro() {
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

int main() {
    test_numbers();
    test_future_test_macro();
}
