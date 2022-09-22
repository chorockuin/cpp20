#include <iostream>
#include <ranges>
#include <vector>

void view_is_pointer() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    // view = 본다. 즉, 바라본다는 뜻이며 이는 값을 복사하겠다가 아닌, 참조하겠다라는 뜻
    auto r1 = v | std::views::take(3);
    // 벡터 v의 요소 3개를 참조하겠다
    std::ranges::take_view r2(v, 3);
    // 벡터 v의 요소 3개를 참조하는 r2를 참조하겠다
    // 그리고 r2가 참조하는 벡터 v의 요소 3개를 참조하여 그 값에 람다 함수를 적용하겠다
    std::ranges::transform_view r3(r2, [](int a) { std::cout << "operation!!" << std::endl; return a*2;});
    std::cout << "start iterating" << std::endl;
    auto p1 = std::ranges::begin(r3);
    // r3를 만들 때 바로 람다 함수가 적용되는 것이 아니라, 실제로 역참조 연산(*)이 발생할 때
    // 즉, 아래와 같이 값을 꺼내려고 역참조 할 때 람다 함수가 적용된다는 것
    std::cout << *p1 << std::endl;
}

void view_simple_expression() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};
    std::ranges::take_view r1(v, 3);
    std::ranges::transform_view r2(r1, [](int a) { return a*2; });

    auto r = v | std::views::take(3) | std::views::transform([](int a) {return a*2;});

    for (auto n : r) {
        std::cout << n << std::endl;
    }
}

void views() {
    view_is_pointer();
    view_simple_expression();
}
