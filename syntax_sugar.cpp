#include <concepts>
#include <iostream>

template<typename T> requires std::integral<T>
void f1(T a) {
}

template<typename T>
void f2(T a) requires std::integral<T> {
}

template<std::integral T>
void f3(T a) {
}

// template이 아닌 것 같지만 위와 동일한 template이다
void f4(std::integral auto a) {
    std::cout << typeid(a).name() << std::endl;
}

// 이건 에러남. 핵심은 auto type
// void f5(std::integral a) {
// }

// 이건 template<typename T> void f6(T a)와 동일함
// auto의 정체는 template
void f6(auto a) {
    std::cout << typeid(a).name() << std::endl;
}

void syntax_sugar() {
    f4(int(0));
    f4(short(0));
    f6(10);
    f6(3.4);
}
