#include <type_traits>
#include <string>
using namespace std::literals;

template<typename T> class Wrapper {
    T data;
public:
    // Wrapper(T value) : data (value) {
    // }
    // explict 키워드를 붙이면 복사 초기화, 암시적 변환 방지
    // explicit Wrapper(T value) : data (value) {
    // }
    // c++20에서는 explict에 bool 조건을 넣을 수 있음
    explicit(std::is_integral_v<T>) Wrapper(T value) : data (value) {
    }
};

static void explicit_bool() {
    // 직접 초기화
    Wrapper<std::string> w0(std::string("abcd"));
    Wrapper w1("abcd"s); // c++17 부터 template 인자 추론 가능
    Wrapper w2{"abcd"s}; // uniform 초기화. 암시적 type 변환 방지
    // 복사 초기화
    Wrapper w3 = "abcd"s;
    Wrapper w4 = {"abcd"s};
    // 암시적 변환
    w1 = "xyz"s;

    Wrapper w5{10}; // ok
    // Wrapper w6 = 10; // explicit(std::is_integral_v<T>) 때문에 error 남
}

static int foo() { return 0; }

static void range_for_with_init() {
    int x[3] = {1,2,3};

    // c++11 range for 추가
    for (auto n : x) { 
    }

    // c++17 초기화 구문을 가진 제어문 추가
    if (int r = foo(); r == 0) { 
    }
    switch (int n = foo(); n) {
    }

    // c++20 초기화 구문을 가진 range for 추가
    for (int y[3]{1,2,3}; auto n : y) {
    }
    // 그런데 이건 안됨. 어차피 모양이 for와 비슷하니 그냥 for 쓰라고 함
    // while (int n=0; n < 10) {
    // }
}

#include <string_view>
enum COLOR {RED=1, GREEN=2, BLUE=3}; // c++98
enum class Color {Red=1, Green=2, Blue=3}; // c++11 scoped enum

static std::string_view to_string(Color c) {
    switch (c) {
        // case Color::Red: return "red";
        // case Color::Green: return "green";
        // case Color::Blue: return "blue";
        using enum Color; // c++20
        case Red: return "red";
        case Green: return "green";
        case Blue: return "blue";
    }
    return "invalid";
}

static void using_enum() {
    int n1 = RED;   // ok
    // int n2 = Red;   // error (scoped enum)
    // int n3 = Color::Red;    // error (scoped enum)
    Color c = Color::Red;   // ok
}

void etc() {
    explicit_bool();
    range_for_with_init();
    using_enum();
}