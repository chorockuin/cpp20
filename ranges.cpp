#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>

void intro() {
    std::vector<int> v = {1,2,3,7,8,9,4,5,6,10};
    // 마치 리액티브 프로그래밍 처럼.. 
    // 특정 로직(람다 함수)을 담고 있는 필터에 데이터를 넘겨 결과를 얻음
    // 짝수 고르는 필터 + 3개만 pick하는 필터
    // 왼쪽 -> 오른쪽으로 적용된다고 보면 됨
    auto r = v | std::views::filter([](int a) {return a%2 == 0;}) | std::views::take(3);
    for (auto n : r) { // 리턴값 r은 range
        std::cout << n << std::endl;
    }

    std::ranges::reverse(r);

    // v의 값들이 원래대로 유지되지 않는 것을 보니
    // filter가 값이 아닌 참조로 동작하는 것을 알 수 있음
    for (auto n : v) {
        std::cout << n << ", ";
    }
    std::cout << std::endl;
}

void intro2() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    // r1, r2는 완전히 동일한 표현, 동일한 타입이다
    std::ranges::take_view r2(v, 3); // vector v에서 3개만 가지는 range를 만든다
    auto r1 = v | std::views::take(3); // '|' 연산자를 재정의해서 만든 것일 뿐

    std::cout << typeid(r1).name() << std::endl; // range type
    std::cout << typeid(r2).name() << std::endl; // range type
}

// range의 concept은 iterable 해야한다는 것(begin()과 end()를 사용할 수 있어야 함)
// 즉, 아래와 같다
// template<typename T>
// concept range = requires(T& t) {
    // std::ranges::begin(t);
    // std::ranges::end(t);
// }
void intro3() {
    std::cout << std::ranges::range<int> << std::endl; // false
    std::cout << std::ranges::range<int[5]> << std::endl; // true
    std::cout << std::ranges::range<std::vector<int>> << std::endl; // true

    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};
    auto r1 = v | std::views::take(3);
    std::cout << typeid(r1).name() << std::endl; // range type
    std::cout << std::ranges::range<decltype(r1)> << std::endl; // true
}

void ranges() {
    intro();
    intro2();
    intro3();
}
