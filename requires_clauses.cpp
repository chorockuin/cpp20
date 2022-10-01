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
    // goo(3.4); // int 형이어야 한다는 requires 절 조건에 부합하지 않으므로 error
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

#include <iostream>
class Test {
public:
    virtual void f() {}
};

template<typename T> requires std::is_polymorphic_v<T> // type T에 가상함수가 들어있는지 확인하는 트레잇
void hoo(const T& arg) {
    std::cout << "type has virtual function" << std::endl;
}

template<typename T> requires (!std::is_polymorphic_v<T>)
void hoo(const T& arg) {
    std::cout << "type has no virtual function" << std::endl;
}

void function_overloading_with_requires() {
    hoo(Test());
}

// T 포인터가 아니면 T 값을 그냥 출력
template<typename T> requires (!std::is_pointer_v<T>)
void printv(const T& arg) {
    std::cout << arg << std::endl;
}

// 포인터면 T 값과 T가 가리키는 값을 역참조해서 출력
template<typename T> requires std::is_pointer_v<T>
void printv(const T& arg) {
    std::cout << arg << " : " << *arg << std::endl;
}

void function_overloading_with_requires1() {
    int n = 0;
    printv(n);
    printv(&n);
}

#include <vector>
#include <list>
template<typename T> requires std::random_access_iterator<T>
void my_advance(T& p, int step) {
    std::cout << "random access" << std::endl;
    p += step;
}

template<typename T> requires std::input_iterator<T>
void my_advance(T& p, int step) {
    std::cout << "not random access" << std::endl;
    while (step--) ++p;
}

void function_overloading_with_requires2() {
    // std::vector c = {1,2,3,4,5,6,7,8,9,10};
    std::list c = {1,2,3,4,5,6,7,8,9,10};

    auto p = std::begin(c);
    // p의 type이 vector의 경우 바로 +5 jump, list의 경우 +1을 5번
    // std::advance(p, 5);

    // std::advance를 직접 구현해 봄
    // vector의 경우 random_access_iterator 이자 input_iterator 이지만
    // 우선순위에 따라 random_access_iterator로 선택됨
    // 우선순위는 concept에 따라 조정할 수 있음
    my_advance(p, 5);

    std::cout << *p << std::endl;
}

constexpr bool check() { return true; }

template<typename T> requires true // 항상 만들어짐
void f1(T a) {
}

template<typename T> requires false // error. 절대로 만들어지지 않음
void f2(T a) {
}

template<typename T> requires std::is_pointer_v<T>
void f3(T a) {
}

// template<typename T> requires 1 // error. requires 뒤에는 bool 형만 올 수 있음
// void f4(T a) {
// }

template<typename T> requires (check()) // requires 뒤에는 상수 표현식만 들어갈 수 있으므로 constexpr 함수 사용 가능
void f5(T a) {
}

template<typename T> requires (sizeof(T) > 4)
void f6(T a) {
}

void condition() {
    f1(0);
    // f2(0);
    // f3(0);
    // f4(0);
    f5(0);
    f6(0.3);
}

template<typename T>
void g(T a) requires (sizeof(T) > 4) { // requires 절을 template<typename T> 뒤에 쓰지 않고, 이렇게 함수 뒤에 써도 됨
}

void position() {
    g(3.5);
}

void require_clauses() {
    intro();
    error_message();
    not_create_type_code_from_template();
    function_overloading_with_requires();
    function_overloading_with_requires1();
    function_overloading_with_requires2();
    condition();
    position();
}
