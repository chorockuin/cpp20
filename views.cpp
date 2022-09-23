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

void ref_view() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};
    // rv는 v의 참조자
    std::ranges::ref_view rv(v);
    auto p1 = rv.begin();
    auto p2 = v.begin();

    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    std::cout << &v << std::endl;
    std::cout << &(rv.base()) << std::endl;

    // 근데 왜 아래 처럼 진짜 참조를 안쓰고 ref_view를 쓰지? ref_view1()를 보자
    std::vector<int>& r = v;
}

void ref_view1() {
    std::vector<int> v1 = {1,2,3,4,5};
    std::vector<int> v2 = {6,7,8,9,10};

    // c++의 참조 : 이동(move) 불가능한 참조 = const
    // 참조는 컴파일 타임에 결정된다는 것과도 일맥상통
    std::vector<int>& r1(v1);
    std::vector<int>& r2(v2);

    // r1은 참조자로 변할 수가 없다
    // 따라서 v2의 내용이 v1으로 복사되어 버린다
    r1 = r2;

    std::cout << v1[0] << std::endl; // 6
    std::cout << v2[0] << std::endl; // 6

    // 그러나 이렇게 ref_view를 쓰면 참조 "변수"를 만드는 것과 같다
    // 즉 포인터 변수를 만드는 것과 같다
    // c++11의 std::reference_wrapper와 같은 메커니즘
    std::vector<int> v3 = {1,2,3,4,5};
    std::vector<int> v4 = {6,7,8,9,10};

    std::ranges::ref_view r3(v3);
    std::ranges::ref_view r4(v4);

    r3 = r4;

    std::cout << v3[0] << std::endl; // 1
    std::cout << v4[0] << std::endl; // 6
    std::cout << r1[0] << std::endl; // 6
    std::cout << r2[0] << std::endl; // 6
}

template<typename R>
class take_view {
    // 이렇게 참조로 해 놓으면 변경할 수가 없기 때문에... 이렇게 쓰면 안됨
    // R& rg;

    // 이렇게 변경 가능한 참조를 쓰거나
    // std::ranges::ref_view<R> rg;

    // 단순히 이렇게 쓰고 ref_view로 추론되도록 타입 추론(deduction)을 만들어 주어야 함
    R rg; // std::ranges::ref_view<std::vector<int>>
    int count;
public:
    template<typename A>
    take_view(A&& r, int cnt) : rg(std::forward<A>(r)), count(cnt) {
    }
    // ... begin(), end() 등
};

// 타입 추론(deduction)
// take_view로 들어온 A type 참조는 A type의 ref_view로 추론하겠다
// A type 참조를 ref_view에 넣으려면 A type 참조를 A type으로 바꿔야 함
// 따라서 std::remove_reference_t<A> 를 사용해 참조를 제거하고 넣어준다
template<typename A>
take_view(A&&, int) -> take_view<std::ranges::ref_view<std::remove_reference_t<A>>>;

void ref_view2() {
    std::vector<int> v1 = {1,2,3,4,5};
    std::vector<int> v2 = {6,7,8,9,0};

    take_view tv1(v1, 3);
    take_view tv2(v2, 3);

    tv1 = tv2;
}

void ref_view3() {
    std::vector<int> v = {1,2,3,4,5};
    std::ranges::ref_view<std::vector<int>> rv1(v);
    std::ranges::ref_view rv2(v); // c++17 부터는 v를 보고 deduction 할 수 있으므로, 이렇게 써도 됨
    auto rv3 = std::views::all(v); // 모두 볼 수 있는 view = ref_view

    std::cout << typeid(rv1).name() << std::endl;
    std::cout << typeid(rv2).name() << std::endl;
    std::cout << typeid(rv3).name() << std::endl;
}

void reverse_view() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    // view class template을 직접 써서 view를 만듬
    std::ranges::reverse_view rv1(v);

    // 아래의 경우 친절하게 type까지 적었지만 예상과 다르게 에러난다. 왜냐?
    // 내부적으로 vector<int>가 아니라 ref_view로 추론되기 때문이다
    // std::ranges::reverse_view<std::vector<int>> rv2(v);
    // 그래서 이렇게 해야 에러가 안난다
    std::ranges::reverse_view<std::ranges::ref_view<std::vector<int>>> rv2(v);

    // 이렇게 range adaptor object를 써도 된다
    // range adaptor object는 함수 객체다
    auto rv3 = std::views::reverse(v);

    for (auto n : rv1) {
        std::cout << n << ",";
    }
    std::cout << std::endl;
}

void filter_view() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    std::ranges::filter_view fv1(v, [](int n) {return n%2 == 0;});
    std::ranges::take_view tv1(fv1, 3);
    std::ranges::reverse_view rv1(tv1);

    auto fv2 = std::views::filter(v, [](int n) {return n%2 == 0;});
    auto tv2 = std::views::take(fv2, 3);
    auto rv2 = std::views::reverse(tv2);

    // 파이프 라인 형식으로 사용하는게 젤 편하다
    auto rv3 = v | std::views::filter([](int n) {return n%2 == 0;}) 
                 | std::views::take(3)
                 | std::views::reverse; // 파라미터가 없을 경우에는 () 없이 그냥 함수 객체를 그대로 쓴다

    for (auto n : rv1) std::cout << n << ", ";
    std::cout << std::endl;
    for (auto n : rv2) std::cout << n << ", ";
    std::cout << std::endl;
    for (auto n : rv3) std::cout << n << ", ";
    std::cout << std::endl;
}

#include <algorithm>
void range_sort() {
    std::vector<int> v = {1,3,5,9,2,4,6,8,10};
    // 이건 vector 자체를 sort
    std::sort(v.begin(), v.end());

    // range의 sort() 이므로 인자로 range로 전달
    // v를 직접 전달하기 때문에 결국 vector도 range임을 알 수 있음
    std::ranges::sort(v);
    auto fv = v | std::views::filter([](int n) {return n%2==0;});
    std::ranges::replace_if(fv, [](int n) {return n>0;}, 0);
    for (auto n : v) std::cout << n << ", ";
    std::cout << std::endl;
}

struct People {
    std::string name;
    int age;
};

void range_projection() {
    std::vector<People> v;
    v.emplace_back("a", 20);
    v.emplace_back("s", 30);
    v.emplace_back("c", 10);
    v.emplace_back("x", 50);
    v.emplace_back("b", 40);

    // stl로 이렇게 정렬할 수도 있음
    // std::ranges::sort(v, [](const People& p1, const People& p2) {return p1.age>p2.age;});

    // c++20 range에서 이런 방법 지원함
    // 여기서 People::age, People::name 넘기는 걸 projection이라고 부름
    std::ranges::sort(v, std::less{}, &People::age);
    for (auto&& p:v) std::cout << p.name << "(" << p.age << ")" << std::endl;
    std::ranges::sort(v, std::greater{}, &People::name);
    for (auto&& p:v) std::cout << p.name << "(" << p.age << ")" << std::endl;
}

void views() {
    view_is_pointer();
    view_simple_expression();
    ref_view();
    ref_view1();
    ref_view2();
    ref_view3();
    reverse_view();
    filter_view();
    range_sort();
    range_projection();
}
