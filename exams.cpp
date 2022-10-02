/*
1. 다음중 C++ 20 에서 추가된 문법이 아닌 것은 ?
explicit(bool) 을 사용하면 특정 조건에 따라 생성자를 explicit 로 만들수 있다.
range-for 에도 초기화 구문을 사용할 수 있다.
-> 클래스 템플릿의 타입인자를 생략 할 수 있다.
consteval 키워드를 사용하면 반드시 컴파일 시간에 수행되어야 하는 함수를 만들 수 있다.

2. 함수가 컴파일 시간에 실행되는지 조사하기 위한 함수는 무엇인가요 ?
-> std::is_constant_evaluated
std::is_constant 
std::is_constexpr
std::is_runtime

3. 다음중 C++20 에서 추가된 람다 표현식의 특징이 아닌 것은 ?
평가되지 않은 표현식에서 람다 표현식을 사용할수 있다.
-> 모든 람다 표현식에서 디폴트 생성자와 대입연산자를 사용할수 있다. : 캡쳐하지 않은 람다 표현식에서만 가능
템플릿 람다 표현식을 만들수 있게 되었다.
parameter pack 을 캡쳐 할수 있다.

4. 다음중 C++20 에서 추가된 attribute 가 아닌 것은 ?
[[likely]]
[[no_unique_address]]
[[nodiscard("reason")]]
-> [[deprecated]]

5. 다음중 C++20의 Concept 에 대한 설명으로 잘못된 것은 ?
타입이 가져야 하는 조건을 정의 하는 문법이다.
템플릿 코드 작성시 requires clauses 를 사용하면 조건을 만족 할때 만 템플릿을 사용할 수 있다.
-> SFINAE 조건이 적용되지는 않는다.
concepts 헤더에는 미리 정의된 많은 concept을 제공한다.

6. range 의 view 대한 reference 역활을 하는 view 는 ?
std::take_view
std::all
-> std::ref_view
std::view_interface

7. 연속된 메모리의 시작주소와 갯수를 관리하는 도구로서, string_view 의 배열 버전 역활을하는 C++20 표준 클래스 템플릿은 ?
std::array
-> std::span
std::variant
std::memory

8. 다음 중 C++20에서 추가된 반복자가 가리키는 타입을 나타내는 것은 ?
value_type
-> iter_value_t
iterator_type
traits_type

9. 다음중 C++20 에서 추가된 키워드가 아닌 것은 ?
co_yield
-> decltype
consteval
requires

10. 다음중 C++20 에서 추가된 헤더파일이 아닌 것은 ?
-> string_view : C++17
concepts
coroutine
ranges

11. 다음중 C++ 20 에 대한 설명중 잘못된 것은 ?
coroutine, concept, ranges, module 등의 특징이 추가 되었다.
-> 표준 라이브러리(STL) 에 대한 모든 요소가 module 로 지원 된다.
three-way-comparision 연산자를 사용하면 비교 연산자를 쉽게 만들수 있다.
람다표현식에서 템플릿 을 사용할수 있게 되었다.

12. template 의 requires clauses 에 대한 설명으로 잘못된 것은 ?
템플릿인자 T가 가져야 하는 제약조건을 표기하는 문법이다.
-> 조건을 만족하지 않은 경우는 항상 에러가 발생한다 : SFINAE
type traits, concept 뿐 아니라 compile 시간에 bool 로 계산될 수 있는 표현식으로 나타낼수 있다.
잘 활용하면 타입이 가져야 하는 조건에 따른 오버로딩을 구현할수 있다.

13. std::ranges::begin 에 대한 설명으로 잘못된 것은?
-> 사용자 정의 컨테이너 설계시 멤버로 되어 있는 begin 만 검색이 가능하다.
borrowed range 가 아닌 rvalue 컨테이너를 전달하면 에러가 발생한다.
begin 함수의 반환값이 "iterator" concept 을 만족하지 않은 경우 에러가 발생한다.
함수가 아닌 함수 객체로 구현되어 있다.

14. borrowed range 에 대한 설명으로 틀린것은 ?
string_view, take_view 등은 borrowed range 이다.
자신이 요소를 소유 하지 않은 range 이다.
std::array 는 borrowed range 이다.
-> std::ranges::borrowed_range<T>; 를 사용하면 조사할수 있다. : 

15. C++20에서 추가된 클래스로, 파일이름, 라인 번호등을 담은 클래스는 ?
std::chrono
std::path
std::file
-> std::source_location
*/

#include <iostream>
void exam1() {
    auto swap = []<typename T>(T& x, T& y) {T t=x; x=y; y=t;};

    int x=1, y=2;
    swap(x, y);

    std::cout << x << "," << y << std::endl;
}

#include <coroutine>
class Generator {
public:
    class promise {
    public:
        unsigned int odd_number;
        promise() : odd_number(1) {}
        Generator get_return_object() {return Generator{std::coroutine_handle<promise>::from_promise(*this)};}
        auto initial_suspend() {return std::suspend_always{};}
        auto return_void() noexcept {return std::suspend_never{};}
        auto final_suspend() noexcept {return std::suspend_always{};}
        void unhandled_exception() {std::exit(1);}
    };
    using promise_type = promise;
    std::coroutine_handle<promise_type> cr_h;

    Generator(std::coroutine_handle<promise_type> cr_h) : cr_h(cr_h) {}
    ~Generator() {if(cr_h) cr_h.destroy();}

    struct get_odd_number {
        constexpr bool await_ready() const noexcept {return false;}
        void await_suspend(std::coroutine_handle<promise_type> cr_h) {cr_h.promise().odd_number += 2;}
        constexpr void await_resume() const noexcept {}
    };

    class iterator {
    public:
        void operator ++() {cr_h.resume();}
        const unsigned int& operator *() const {return cr_h.promise().odd_number;}
        bool operator ==(std::default_sentinel_t) const {return !cr_h || cr_h.done();}
        explicit iterator(std::coroutine_handle<promise_type> cr_h) : cr_h{cr_h} {}
    private:
        std::coroutine_handle<promise_type> cr_h;
    };

    iterator begin() {return iterator{cr_h};}
    std::default_sentinel_t end() {return {};}
};

Generator get_odd_number_generator() {
    while (1)
        co_await Generator::get_odd_number{};
}

void exam2() {
    Generator g = get_odd_number_generator();
    Generator::iterator odd_number_i = g.begin();
    for (int i=0; i<10; ++i) {
        std::cout << *odd_number_i << ",";
        ++odd_number_i;
    }
    std::cout << std::endl;
}

#include <ranges>
#include <vector>
void exam3() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};
    std::ranges::drop_view dv(v, 3);
    std::ranges::filter_view fv(dv, [](int n) {return n%2==0;});
    std::ranges::transform_view tv(fv, [](int n) {return n*2;});
    for (auto n : tv)
        std::cout << n << ",";
    std::cout << std::endl;
}

#include <iterator>
#include <type_traits>

template<typename T>
concept is_random_access_iterator = std::is_same<typename T::category, std::random_access_iterator_tag>::value;

template<typename T>
class Base {
public:
    void foo() {
        std::cout << "foo()" << std::endl;
    }
    void goo() requires is_random_access_iterator<T>  {
        std::cout << "goo()" << std::endl;
    }
};

class A : public Base<A> {
public:
    using category = std::random_access_iterator_tag;
};

class B : public Base<B> {
public:
    using category = std::bidirectional_iterator_tag;
};

void exam4() {
    A a;
    B b;
    a.foo();
    a.goo();
    b.foo();
    // b.goo();
}

void exams() {
    exam1();
    exam2();
    exam3();
    exam4();
}
