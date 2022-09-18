/*
1. 어떤 함수가 상수 표현식으로 표현 가능한지 확인할 수 있는 함수 std::is_constant_evaluated() 추가
2. 가상함수에도 constexpr 사용 가능
3. stl에 constexpr 적용되기 시작
4. constexpr std::string, std::vector 추가
5. consteval 추가
6. constinit 추가
*/

#include <iostream>
constexpr auto add(int x, int y) {
    bool b = std::is_constant_evaluated();
    return std::pair(x + y, b);
}

void is_constant_evaluated() {
    constexpr auto r1 = add(1, 2); // 1, 2 모두 상수이므로 컴파일 타임에 상수 표현식으로 표현 가능. 따라서 second : true
    auto r2 = add(1, 2); // 컴파일러 최적화에 따라 다름 second : true or false
    const auto r3 = add(1, 2); // 컴파일러 최적화에 따라 다름 second : true or false

    int x=1, y=2;
    auto r4 = add(x, y); // second : false
    const auto r5 = add(x, y); // second : false
    // constexpr auto r6 = add(x, y); // 변수 x, y 때문에 컴파일 타임에 상수 표현식으로 표현 불가능함에도 constexpr을 붙여놨으니 compile error

    std::cout << r1.second << std::endl;
    std::cout << r2.second << std::endl;
    std::cout << r3.second << std::endl;
    std::cout << r4.second << std::endl;
    std::cout << r5.second << std::endl;
}

class Base {
public:
    virtual int foo(int a, int b) {
        return a + b;
    }
};

class Derived : public Base {
public:
    constexpr virtual int foo(int a, int b) { // c++20 가능
        return a + b;
    }
};

void constexpr_virtual() {
    Derived d;
    constexpr int r1 = d.foo(1, 2); // c++20 가능

    Base *p = &d; // 런타임에 주소 값을 얻어와야 하므로
    // constexpr int r3 = p->foo(1, 2); // 컴파일 타임에 상수 표현식으로 표현 불가. 그런데 constexpr 붙어있으므로 error
    int r2 = p->foo(1, 2); // constexpr 안 붙이면 ok
}

#include <algorithm>
#include <numeric>
#include <array>
#include <vector>
void constexpr_algorithm() {
    // constexpr int x[10] = {1,2,3,4,5,6,7,8,9,10};
    constexpr std::array<int, 10> x = {1,2,3,4,5,6,7,8,9,10};
    // constexpr std::vector<int> x = {1,2,3,4,5,6,7,8,9,10}; c++ 표준에는 지원한다고 했으나 아직 cl에서 지원 안됨

    constexpr int c = std::accumulate(std::begin(x), std::end(x), 0);
    std::cout << c << std::endl;

    constexpr auto p = std::find(std::begin(x), std::end(x), 3);
    std::cout << *p << std::endl;
}

// constexpr은 c++11에 추가. 함수 또는 변수는 상수 표현식으로 나타낼 수도 있음
constexpr int add1(int a, int b) { return a+b; }
// consteval은 c++20 추가. 함수는 반드시 상수 표현식으로 나타낼 수 있어야 함. 이런 함수를 immediate function 이라고 부름
consteval int add2(int a, int b) { return a+b; } 

void const_evaluation() {
    int x=1, y=2;

    int r1 = add1(x, y); // ok
    constexpr int r2 = add1(1, 2); // ok

    // int r3 = add2(x, y); // error
    constexpr int r4 = add2(1, 2); // ok
}

int n=10;
constexpr int c1 = 10; // 초기값을 컴파일 타임에 알아야 함. 상수
// constexpr int c2 = n; // error

constinit int v1 = 10; // 초기값을 컴파일 타임에 알아야 함. 변수
// constinit int v2 = n; // error

void const_initialization() {
    // c1 = 20; // 상수를 변경하려고 했으니 error
    v1 = 20; // 변수를 변경하니 ok
}

// constinit을 왜쓰지?
// 전역 변수들은 원래 런타임 때 초기화 순서를 보장 받을 수 없으나
// constinit을 사용하면 컴파일 타임에 초기화 되는 것을 보장받을 수 있음

void const_expression() {
    is_constant_evaluated();
    constexpr_virtual();
    constexpr_algorithm();
    const_evaluation();
    const_initialization();
}