#include <iostream>
#include <string>
#include <string_view>

static void starts_with_ends_with() {
    std::cout << std::endl << __FUNCTION__ << std::endl;
    
    std::string s{"file_name.txt"};
    std::string_view sv{s};

    std::cout << std::boolalpha;
    std::cout << s.starts_with("hello") << std::endl; // false
    std::cout << s.ends_with(".txt") << std::endl; // true
    std::cout << sv.starts_with("hello") << std::endl; // false
    std::cout << sv.ends_with(".txt") << std::endl; // true
}

#include <set>
static void contains() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::set<int> s = {1,2,3,4,5,6,7,8,9,10};
    std::multiset<int> ms {1,2,3,4,5,6,7,8,9,10};

    // c++20 이전에는 특정 값이 포함되어있는지 검사하기 위해
    // find()와 count()라는 함수 사용. 가독성 떨어짐
    auto r = s.find(3);
    if (r != s.end()) {
    }
    if (ms.count(3)) {
    }

    // c++20 부터는 contains() 함수로 통일됨
    // associative container 8개에 모두 추가되었음
    std::cout << s.contains(3) << std::endl;
    std::cout << ms.contains(3) << std::endl;
}

#include <vector>
#include <list>
#include <ranges>

// 반복자가 가리키는 타입의 변수 만들기
template<typename T>
void foo(const T& iterator) {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    // c++98 style
    // T가 그냥 포인터면 아래 에러남
    // typename T::value_type n;
    // T가 그냥 포인터면 아래와 같이 써야 함
    typename std::iterator_traits<T>::value_type n1;

    // c++20 부터는 그냥 포인터든 뭐든 그냥 아래처럼 쓰면 됨
    std::iter_value_t<T> n2;
    // c++20에 아래 추가 됨
    // iter_value_t
    // iter_reference_t
    // iter_difference_t
    // iter_rvalue_reference_t
    // iter_common_reference_t

    std::cout << typeid(n1).name() << std::endl;
    std::cout << typeid(n2).name() << std::endl;
}

// 컨테이너가 저장하는 타입 구하기
template<typename T>
void goo(const T& container) {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    // c++98
    // T가 그냥 배열이면 아래 에러남
    // typename T::value_type n;

    // c++20
    std::ranges::iterator_t<T> it;
    std::ranges::range_value_t<T> n1;

    std::cout << typeid(n1).name() << std::endl;
}

static void iterator_value_type() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::vector<int> v = {1,2,3,4,5};
    int x[5] = {1,2,3,4,5};

    foo(std::begin(v));
    foo(std::begin(x));

    goo(v);
    goo(x);
}

#include <type_traits>
// List는 생성자 2개를 가지는데,
// 1. 사이즈와 타입을 받는 생성자
// 2. 임의의 타입인 레퍼런스를 받는 생성자
template<typename T> class List {
public:
    List(std::size_t sz, T v) {
    }

    template<typename C>
    List(C&& c) {
    }
};
// c++17 class template type deduction guide에 따라
// 아래처럼 타입 추론을 정의해 주면 될 것 같지만...
// template<typename C> List(C&&) -> List<typename C::value_type>;
// forwarding reference는 레퍼런스를 넘기기 때문에 value_type을 쓸 수 없다
// 따라서 아래처럼 써줘야 한다
// template<typename C> List(C&&) -> List<typename std::remove_reference_t<C>::value_type>;
// c++20부터는 range_value_t를 사용해 더 쉽게 표현 가능하다(컨테이너는 range_value를 갖고 있으므로 가능)
template<typename C> List(C&&) -> List<std::ranges::range_value_t<C>>;

static void range_value_type() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    List<int> s1(10, 3); // c++17 이전에는 T를 명시해줘야 했다
    List s2(10, 3); // c++17 이후에는 생성자 인수롤 보고 T를 추론한다. 3이니까 int형

    std::vector v = {1,2,3};
    // 컨테이너를 받아서 생성하는 경우
    // 컨테이너에 담기는 요소의 타입인 T를 추론할 수 있게끔 해줘야 한다
    List s3(v);
    // List s4(v.begin(), v.end());
}

#include <algorithm>
static void new_algorithm() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::vector<int> v1 = {1,2,3,4,5,6,7,8,9,10};

    // c++98 remove-erase idioms
    // 아래 코드를 수행하면 vector의 값들은 바뀌지만 vector의 size는 그대로다
    auto p = std::remove_if(v1.begin(), v1.end(), [](int n) {return n%2==0;});
    // 이렇게 erase()까지 해야 실제로 vector size가 정리된다
    v1.erase(p, v1.end());

    std::vector<int> v2 = {1,2,3,4,5,6,7,8,9,10};

    // c++20에서는 한번에 size도 정리된다
    std::erase(v2, 3);
    std::erase_if(v2, [](int n) {return n%2==0;});

    for (auto n : v2)
        std::cout << n << ", ";
    std::cout << std::endl;

    // 하지만 실제로 메모리 크기가 줄었을까? vector type이기 때문에 사이즈 변수 값만 바뀐 것이다
    std::cout << v2.size() << std::endl;
    // 메모리 크기는 그대로 10으로 잡혀있다
    std::cout << v2.capacity() << std::endl;
    // vector가 아닌 list를 사용했다면 메모리 크기도 4로 줄었을 것이다
}

class Point {
    int x, y;
public:
    Point(int x, int y) : x(x), y(y) {
        std::cout << "Point()" << std::endl;
    }
    ~Point() {
        std::cout << "~Point()" << std::endl;
    }
};

static void construct_at() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    // operator new를 사용하면 생성자는 호출하지 않고 메모리만 할당할 수 있음
    Point *p1 = static_cast<Point *> (operator new(sizeof(Point)));

    // c++98 가독성이 떨어지는 표현
    new(p1) Point(1,2); // 생성자 호출
    p1->~Point(); // 소멸자 호출

    // c++20 가독성이 좋아짐
    std::construct_at(p1, 1, 2); // c++20
    std::destroy_at(p1); // c++17

    operator delete(p1);
}

template<typename T>
void show(T& c) {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    for (auto n:c) std::cout << n << ",";
    std::cout << std::endl;
}

static void shift() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::vector v1{1,2,3,4,5,6,7,8,9,10};
    show(v1);

    std::shift_left(std::begin(v1), std::end(v1), 3);
    show(v1);

    std::shift_right(std::begin(v1), std::end(v1), 3);
    show(v1);
}

#include <utility>
static void compare_less() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    int sn = 0;
    unsigned int un = 0;

    std::cout << (-1 < sn) << std::endl;
    // sign과 unsign을 비교한 것 자체가 잘못이긴 하지만 암튼 false라는 결과가 나옴
    std::cout << (-1 < un) << std::endl;
    // c++20의 cmp_less() 사용하면 비교를 정확하게 해줌
    std::cout << std::cmp_less(-1, un) << std::endl;
}

#include <memory>
static void make_shared() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::shared_ptr<int> sp1(new int); // worse
    std::shared_ptr<int> sp2 = std::make_shared<int>(); // better
    // https://infoscoco.com/48

    // c++17
    std::shared_ptr<int[]> sp3(new int[10]); // worse
    // c++20 make_shared()의 배열 버전 제공
    std::shared_ptr<int[]> sp4 = std::make_shared<int[10]>(); // better
    auto sp5 = std::make_shared<int[]>(3); // new int[3]
    auto sp6 = std::make_shared<int[3]>(); // new int[3]
    auto sp7 = std::make_shared<int[3]>(4); // new int[3]{4,4,4,4}

    // sp1[0] = 10; // error
    *sp1 = 10; // ok
    sp3[0] = 10; // ok
    // *sp3 = 10; // error
}

struct Rect {int l, t, r, b;};

static void make_shared1() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    Rect *p1 = new Rect; // default 초기화(쓰레기 값)
    Rect *p2 = new Rect{}; // value 초기화(0으로 초기화)

    std::cout << p1->l << std::endl; // 쓰레기 값
    std::cout << p2->l << std::endl; // 0

    auto sp1 = std::make_shared<Rect>(); // value 초기화
    std::cout << sp1->l << std::endl; // 0

    // c++20
    auto sp2 = std::make_shared_for_overwrite<Rect>(); // 성능을 위해 default 초기화 
    auto sp3 = std::make_shared_for_overwrite<Rect[]>(3); // 성능을 위해 default 초기화
}

#include <array>
static void to_array() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    int x[10] = {1,2,3,4,5,6,7,8,9,10};
    auto a1 = std::to_array(x); // std::array<int, 10>
    auto a2 = std::to_array("foo"); // std::array<char, 4>
    auto a3 = std::to_array({1,2,3}); // std::array<int, 3>

    std::cout << typeid(a1).name() << std::endl;
    std::cout << typeid(a2).name() << std::endl;
    std::cout << typeid(a3).name() << std::endl;
}

#include <numeric>
static void midpoint() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::cout << std::midpoint(1,3) << std::endl;
    std::cout << std::midpoint(1,10) << std::endl;
    std::cout << std::midpoint(10,2344453) << std::endl;
    std::cout << std::midpoint(1.3,3.4) << std::endl;
}

#include <functional>

static void hoo(int a, int b, int c) {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    printf("hoo:%d %d %d\n", a, b, c);
}

static void bind_front() {
    // f1함수의 첫번째 인자는 3이고 뒤에 2개 인자는 받아서 쓸거다
    auto f1 = std::bind(&hoo, 3, std::placeholders::_1, std::placeholders::_2); // c++11

    auto f2 = std::bind_front(&hoo, 3);
    auto f3 = std::bind_front(&hoo, 3, 4);
    auto f4 = std::bind_front(&hoo, 3, 4, 5);
    f2(1,2); // a=3, b=1, c=2
    f3(1); // a=3, b=4, c=1
    f4(); // a=3, b=4, c=5
}

#include <span>
// 포인터 + 크기를 넘겨야 하는 불편을 위해 span 탄생
// span는 포인터 + 사이즈로 구성되어 있음
// 연속된 메모리 자료 구조는 다 넣을 수 있음(vector, array)
// std::string_view와 유사
// std::string_view는 읽기만 가능하지만, std::span은 읽기/쓰기 다 가능
static void ioo(std::span<int> sp) {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::cout << sp.size() << std::endl;
    sp[0] = 10;
}

static void span1() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::vector<int> x{1,2,3,4,5,6,7,8,9,10};
    ioo(x);
}

static void span2() {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    int x[10] = {1,2,3,4,5,6,7,8,9,10};
    std::cout << typeid(x).name() << ": " << sizeof(x) << std::endl; // int array type, size = 40

    std::span<int> sp1(x); // ok T : int
    std::span<int, 10> sp2(x); // ok T : int[10]

    std::cout << typeid(sp1).name() << ": " << sizeof(sp1) << std::endl; // type저장(8) + size저장(8) = 16

    // type이 int[10]이므로 size는 저장할 필요가 없음
    std::cout << typeid(sp2).name() << ": " << sizeof(sp2) << std::endl; // type저장(8) = 8

    // sp1의 경우 몇 개의 요소를 가지고 있는지 명시하지 않았기 때문에 -1
    std::cout << sp1.extent << std::endl; // -1
    std::cout << sp2.extent << std::endl; // 10

    std::cout << sp1.size() << std::endl; // 10
    std::cout << sp1.size_bytes() << std::endl; // 40

    std::cout << sp2.size() << std::endl; // 10
    std::cout << sp2.size_bytes() << std::endl; // 40

    int *p = new int[10]{1,2,3,4,5,6,7,8,9,10};
    std::cout << typeid(p).name() << ": " << sizeof(p) << std::endl; // int pointer type, size = 8

    // std::span<int> sp3(p); // error
    std::span<int> sp4(p, 10); // ok
    // std::span<int, 10> sp5(p); // error
}

void joo(std::span<int> sp) {
    std::cout << std::endl << __FUNCTION__ << std::endl;

    // std::span<byte> 타입으로 변환
    // auto p = std::as_bytes(sp); // p는 const
    auto p = std::as_writable_bytes(sp);
    p[3] = static_cast<std::byte>(0x33);
    std::cout << std::hex << sp[0] << std::endl; // 0x33000001
}

static void span3() {
    std::cout << std::endl << __FUNCTION__ << std::endl;
    
    int x[10]{1,2,4,5,6,7,8,9,10};
    int *p = new int[10]{1,2,4,5,6,7,8,9,10};

    joo(x); // int[10]
    // joo(p); // int*    error
    joo({p, 10}); // c++11
}

void stl() {
    starts_with_ends_with();
    contains();
    iterator_value_type();
    range_value_type();
    new_algorithm();
    construct_at();
    shift();
    make_shared();
    make_shared1();
    to_array();
    midpoint();
    bind_front();
    span1();
    span2();
    span3();
}
