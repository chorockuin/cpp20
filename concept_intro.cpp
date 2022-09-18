#include <concepts>

// 4이상의 값을 가지는 컨셉(concept)
template<typename T>
concept GreaterThan4 = sizeof(T) >= 4;

// template 인자 T가 4이상의 값을 가지는 컨셉(concept)을 요구(requires)함
// requires 이후에는 concept 외에 trats도 올 수가 있음
template<typename T> requires GreaterThan4<T>
static void foo(T arg) {
}

static void intro() {
    int i = 10;
    short s = 10;
    double d = 3.4;

    foo(i); // >= 4 ok
    // foo(s); // < 4 error
    foo(d); // >= 4 ok
}

void concept_intro() {
    intro();
}
