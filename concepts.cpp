#include <iostream>
#include <type_traits>
#include <concepts>

// 4이상의 값을 가지는 컨셉(concept)
template<typename T>
concept GreaterThan4 = sizeof(T) >= 4;

// template 인자 T가 4이상의 값을 가지는 컨셉(concept)을 요구(requires)함
// requires 이후에는 concept 외에 trats도 올 수가 있음
template<typename T> requires GreaterThan4<T>
static void foo(T arg) {
}

template<typename T>
concept Integral = std::is_integral_v<T>;

// template<typename T> requires std::is_integral_v<T>
template<typename T> requires Integral<T>
static void bar(T a) {
}

static void intro() {
    int i = 10;
    short s = 10;
    double d = 3.4;

    foo(i); // >= 4 ok
    // foo(s); // < 4 error
    foo(d); // >= 4 ok

    std::cout << Integral<int> << std::endl;
    std::cout << Integral<float> << std::endl;
}

// why concept?
// 1. 여러 개의 제약 조건을 하나의 이름으로 관리 가능
// 2. 제약 조건을 표현하는 requires expression 문법 제공
// 3. 조건을 만족하는 2개 이상의 템플릿이 있을 때 ambiguous error 해결 가능

template<typename T> concept True = true;

// T type의 2개 변수는 % 연산을 할 수 있어야 한다는 concept
template<typename T>
concept Modulus = requires(T a, T b) { // requires expression
    a % b;
};

// 아래 Mod 함수는 T type이 % 연산이 가능해야 한다는 concept을 requires 함
template<typename T> requires Modulus<T> // requires clauses
T Mod(T a, T b) {
    return a % b;
}

static void requires_expression() {
    Mod(10, 3); // ok
    // Mod(3.4, 2.1); error
}

template<typename T>
concept LessThanComparable1 = requires(T a, T b) {
    a < b;
};

// < 연산이 가능해야 하고 결과는 bool이어야 한다는 concept
template<typename T>
concept LessThanComparable2 = requires(T a, T b) {
    {a < b} -> std::convertible_to<bool>;
};

// ==, != 연산이 가능하고 결과는 bool이어야 한다는 concept
template<typename T>
concept Equality = requires(T a, T b) {
    {a == b} -> std::convertible_to<bool>;
    {a != b} -> std::convertible_to<bool>;
};

// c.begin(), c.end()를 호출할 수 있어야 한다는 concept
template<typename T>
concept Container = requires(T c) {
    c.begin();
    c.end();
};

template<typename T>
// T에는 value_type이 있어야 한다는 concept
concept HasValueType = requires {
    typename T::value_type;
};

template<typename T> requires Container<T>
static void goo(T a) {
}

template<typename T> requires HasValueType<T>
static void hoo(T a) {
}

#include <vector>
static void requires_expression1() {
    std::vector<int> v = {1,2,3};
    goo(v); // ok
    hoo(v); // ok
    // goo(1); // error
    // hoo(1); // error
}

template<typename T>
concept Concept1 = sizeof(T) > 1;

template<typename T>
concept Concept2 = sizeof(T) > 1 && sizeof(T) < 8;

template<typename T> requires Concept1<T>
static void ioo(T a) {
    std::cout << "1" << std::endl;
}

template<typename T> requires Concept2<T>
static void ioo(T a) {
    std::cout << "2" << std::endl;
}

template<typename T>
concept Concept3 = Concept1<T> && sizeof(T) < 8;

template<typename T> requires Concept1<T>
static void joo(T a) {
    std::cout << "1" << std::endl;
}

template<typename T> requires Concept3<T>
static void joo(T a) {
    std::cout << "3" << std::endl;
}

template<typename T> requires Concept1<T>
static void koo(T a) {
    std::cout << "1" << std::endl;
}

template<typename T> requires Concept1<T> && (sizeof(T) < 8)
static void koo(T a) {
    std::cout << "1 requires" << std::endl;
}

static void ordering() {
    // Concept1과 Concept2를 모두 만족하기 때문에 모호성 발생하여 에러남
    // ioo(3);

    // Concept1과 Concept3을 모두 만족하지만, 
    // Concept3 조건은 Concept1 조건을 포함하므로 제약이 더 많음
    // 제약이 더 많은 것이 선택됨
    joo(3);

    // requires 절에 바로 조건을 적어도 되는데
    // Concept1 보다 Concept1 + sizeof(T) < 8 이 제약이 더 많음
    // 제약이 더 많은 것이 선택됨
    koo(3);
}

/*
반복자의 카테고리(참고)
input iterator
forward iterator
bidirectional iterator
ramdom access iterator
contiguous iterator
*/
#include <vector>
#include <list>

template<typename T> requires std::input_iterator<T>
static void my_advance(T p, int N) {
    std::cout << "input_iterator" << std::endl;
    while(N--) ++p;
}

template<typename T> requires std::random_access_iterator<T>
static void my_advance(T p, int N) {
    std::cout << "random_access_iterator" << std::endl;
    p += N;
}

static void example() {
    std::vector<int> c = {1,2,3,4,5,6,7,8,9,10};
    // std::list<int> c = {1,2,4,5,6,7,8,9,10};

    auto p = c.begin();
    // vector같이 random access iterator 이냐(p+=3 가능)
    // list같이 bidirectional iteratoir 이냐(p+=3 불가능, ++p; ++p; ++p;만 가능)
    // 를 따져서 최적의 연산을 선택해서 동작한다
    // std::advance(p, 3);

    // random_access_iterator 가 제약이 더 많기 때문에 제약이 많은 쪽을 선택
    // 따라서 vector는 random_access_iterator를, list는 input_iterator를 선택
    my_advance(p, 3);
}

void concepts() {
    intro();
    requires_expression();
    requires_expression1();
    ordering();
    example();
}