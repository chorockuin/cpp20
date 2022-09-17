/*
1. three way comparison operator(spaceship operator) 추가
*/

#include <iostream>
#include <compare>
class Int32 {
    int value;
public:
    explicit Int32(int v = 0) : value(v) {
    }

    // 기존에는 아래 < 연산자를 오버라이딩 한 것 처럼 나머지 모든 연산자(>, >=, <=, ==, !=)를 일일이 오버라이딩 해야 했음
    // friend bool operator < (const Int32& obj1, const Int32& obj2) { return obj1.value < obj2.value; }
    // friend bool operator < (const Int32& obj, int n) { return obj.value < n; }
    // friend bool operator < (int n, const Int32& obj) { return n < obj.value; }

    // c++20 부터는 <=> 하나로 해결
    auto operator <=> (const Int32& other) const { return value <=> other.value; }
    bool operator == (const Int32& other) const { return value == other.value; }

    auto operator <=> (int other) const { return value <=> other; }
    bool operator == (int other) const { return value == other; }
};

void three_way_comparison_operator() {
    int a = 10, b = 20;
    bool r1 = a < b;
    auto r2 = a <=> b;

    if (r2 > 0) std::cout << "a > b" << std::endl;
    else if (r2 < 0) std::cout << "a < b" << std::endl;
    else if (r2 == 0) std::cout << "a == b" << std::endl;

    std::cout << typeid(r2).name() << std::endl; // strong_ordering

    Int32 n1{10};
    Int32 n2{20};

    bool b1 = n1 < n2; // operator < (Int32, Int32) 또는 n1.operator < (Int32)
    bool b2 = n1 < 20; // operator < (Int32, int) 또는 n1.operator < (int)
    bool b3 = 10 < n1; // operator < (int, Int32) 만 가능. < 연산자가 int로 시작하므로 Int32의 멤버 함수로는 만들지 못함
    
    // 아래와 주석과 같이 컴파일 타임에 컴파일러가 원본 코드를 <=> 연산자를 사용한 코드로 바꿔준다
    // 이를 Rewrite Expression 이라고 한다
    bool c1 = n1 == n2; // n1.operator == (n2)
    bool c2 = n1 != n2; // !(n1 == n2)
    bool c3 = n1 < n2; // (n1 <=> n2) < 0
    bool c4 = n1 > n2; // (n1 <=> n2) > 0
    bool c5 = n1 < 10; // (n1 <=> 10) < 0
    bool c6 = 10 < n1; // (10 <=> n1) < 0 이건 실패할 텐데, 컴파일러가 (n1 <=> 10) > 0 로 변환함
}

class Point3D {
    int x;
    int y;
    int z;
public:
    Point3D(int x=0, int y=0, int z=0) : x{x}, y{y}, z{z} {
    }

    // 아래와 같이 <=> 연산자 오버라이딩을 구현하면 됨
    // int x, int y, int z도 <=> 연산을 지원해야 한다는 것에 주의
    auto operator <=> (const Point3D& p) const {
        if (auto r = x <=> p.x; r != 0) return r;
        if (auto r = y <=> p.y; r != 0) return r;
        return z <=> p.z;
    }

    // 위 <=> 연산자 구현에서 == 연산자 구현도 커버가 되는데 왜 따로 구현해야 할까?
    // 예를 들어 비교 대상인 클래스 멤버 변수 타입이 string이라고 했을 때
    // <==> 연산자 구현을 그대로 사용해 == 연산을 하면 메모리의 내용을 직접 비교하는데
    // 그 전에 string의 길이부터 비교하면 최적화가 가능함. 최적화 가능성을 열어두기 위해 구현하라고 강제하는 것
    // 구현하기 싫으면 default로 지정하면 됨
    auto operator == (const Point3D& p) const {
        return x == p.x && y == p.y && z == p.z;
    }

    // 그러나 <=> 연산자 연산자 오버라이딩을 default로 지정하면 컴파일러가 default 구현해 줌. == 연산자도 default 구현해 줌
    // default 구현은 lexicographical compare 방식으로 구현되는데
    // 위 코드 처럼 멤버의 순서대로 비교를 하는 방식으로 구현한다는 뜻
    // auto operator <=> (const Point3D& p) const = default; 
};

void three_way_comparison_operator_impl() {
    Point3D p1{1, 2, 3}, p2{3, 2, 1};

    bool b1 = p1 == p2;
    bool b2 = p1 < p2;
    bool b3 = p1 <= p2;
    auto r = p1 <=> p2;
}

void three_way_comparison_operator_impl2() {
}

// std::strong_ordering : == 연산자의 의미를 상식적(메모리 내 값이 서로 같음)으로 정의하고 싶을 때 사용
// std::weak_ordering : == 연산자의 의미를 클래스 작성자가 원하는 대로 정의하고 싶을 때 사용
void three_way_comparison_operator_ordering1() {
    int n1=10, n2=20;
    double d1=10, d2=20;

    auto r1 = (n1 <=> n2);
    auto r2 = (d1 <=> d2);
    auto r3 = (n1 <=> d2);

    if (r1 < 0) { // r1 == std::strong_ordering::less
    }
    if (r1 > 0) { // r1 == std::strong_ordering::greater
    }
    if (r1 == 0) { // r1 == std::strong_ordering::equal
    }

    std::cout << typeid(r1).name() << std::endl;
    std::cout << typeid(r2).name() << std::endl;
    std::cout << typeid(r3).name() << std::endl;
}

struct Rect {
    int x, y, w, h;
    Rect(int x, int y, int w, int h) : x{x}, y{y}, w{w}, h{h} {
    }

    // == 연산을 두 Rect의 넓이 비교로 구현한 것은 사용자가 맘대로 한 것
    // 따라서 std::weak_ordering type으로 정의하는 것이 바람직
    // 만약 std::strong_ordering type으로 정의해버리면
    // 사용자들이 == 연산자를 사용할 때, 두 Rect의 메모리 내 값이 같다는 의미로 오해할 가능성 있음(그냥 넓이가 같은 것일 뿐임!)
    std::weak_ordering operator <=> (const Rect& r) const {
        return (w * h) <=> (r.w * r.h);
    }
};

void three_way_comparison_operator_ordering2() {
    Rect rt1(1, 1, 5, 2);
    Rect rt2(1, 1, 2, 5);

    auto r = rt1 <=> rt2;
    // 만약 r의 type이 std::strong_ordering 이라면, 사용자들은 rt1, rt2의 메모리 내 값이 서로 같다고 오해할 수 있음
    if (r == 0) {
    }
}

void three_way_comparison_operator_ordering3() {
}

#include <string>
struct People {
    std::string name;
    int age;

    People(std::string name, int age) : name{name}, age{age} {
    }

    // People을 age 순으로 비교한다는 것은 클래스 작성자가 맘대로 정한 것이므로 std::weak_ordering이 어울림
    // std::weak_ordering operator <=> (const People& p) const {

    // 그런데 만약 age 값이 음수가 들어온다면? 비교 자체가 불가할 것임
    // 즉, == 연산을 적용할 수 없는 입력 값이 존재할 수도 있다는 얘기
    // 즉, 일부 입력 값(partial)에만 == 연산을 적용할 수 있다는 얘기
    // 이 때 std::partial_ordering을 사용하여 적절히 처리해 줌
    std::partial_ordering operator <=> (const People& p) const {
        if (age < 1 || p.age < 1)
            return std::partial_ordering::unordered;
        return age <=> p.age;
    }
};

void three_way_comparison_operator_ordering4() {
    People p1("kim", 20);
    People p2("Lee", -10);

    auto r = (p1 <=> p2);
    if (r == std::partial_ordering::unordered) {
        std::cout << "can't ordered" << std::endl;
    }

    // std::partial_ordering 이 사용되는 대표적인 예가 실수 비교임
    // 실수 값은 경우에 따라 NaN(Not a Number) 값이 되어 <=> 연산을 못하는 경우가 발생할 수 있기 때문에
    // <=> 연산에 std::partial_ordering 리턴 type이 사용됨
    auto r1 = (3.4 <=> 2.1);
    std::cout << typeid(r1).name() << std::endl;
}

void comparison() {
    three_way_comparison_operator();
    three_way_comparison_operator_impl();
    three_way_comparison_operator_impl2();
    three_way_comparison_operator_ordering1();
    three_way_comparison_operator_ordering2();
    three_way_comparison_operator_ordering3();
    three_way_comparison_operator_ordering4();
}
