#include <type_traits>

template<typename T>
static void foo(T arg) {
}

template<typename T> requires std::is_integral_v<T>
static void goo(T arg) {
}

static void intro() {
    foo(1);
    foo(3.4);
    goo(1);
    // goo(3.4); // requires 절 조건에 부합하지 않으므로 error
}

template<typename T>
T gcd1(T a, T b) {
    return b == 0 ? a : gcd1(b, a%b);
}

template<typename T> requires std::is_integral_v<T>
T gcd2(T a, T b) {
    return b == 0 ? a : gcd2(b, a%b);
}

static void error_message() {
    gcd1(4, 2);
    // T를 가지고 % 연산을 할 수 없다는 에러 메시지. 다소 직관적이지 않은 에러 메시지
    // gcd1(4.2, 2.1);

    gcd2(4, 2);
    // T가 integer가 아니라는 에러 메시지. 직관적인 에러 메시지
    // gcd2(4.2, 2.1); 
}

static double gcd3(double a, double b) { return 0; }
template<typename T>
T gcd3(T a, T b) {
    return b == 0 ? a : gcd3(b, a%b);
}

static double gcd4(double a, double b) { return 0; }
template<typename T> requires std::is_integral_v<T>
T gcd4(T a, T b) {
    return b == 0 ? a : gcd4(b, a%b);
}

void not_create_type_code_from_template() {
    gcd3(4.2, 2.1); // return 0; 하는 double type 버전 사용
    gcd3(4, 2); // int type으로 template 찍어냄
    // gcd3(4.2f, 2.1f); // float type으로 template 찍어냈으나, % 연산이 안되서 에러 남

    gcd4(4.2, 2.1); // return 0; 하는 double type 버전 사용
    gcd4(4, 2); // int type으로 template 찍어냄
    // requires 절 때문에 float type으로 template 찍어내지 못함
    // 그러나 에러가 나진 않고, double type 버전을 암시적으로 사용하게 됨
    // 이를 SFINAE(Substitution Failure Is Not An Error) - "치환에 실패한 것은 에러가 아님" 이라고 함
    // c++20 이전에는 std::enable_if를 사용했었음
    gcd4(4.2f, 2.1f); 
}

void require_clauses() {
    intro();
    error_message();
    not_create_type_code_from_template();
}
