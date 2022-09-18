#include <iostream>
#include <type_traits>

struct Point1 {
    int x, y;
};

struct Point2 {
    int x, y;
    virtual void foo() {
    }
};

void aggregate_init() {
    int x[2] = {1, 2}; // 배열 x는 Aggregate 초기화 가능
    Point1 p1 = {1, 2}; // Point1 구조체도 Aggregate 초기화 가능
    // Point2 p2 = {1, 2}; // Point2 구조체는 virtual 메소드를 가지고 있으므로 Aggregate 초기화 불가능. 따라서 에러

    // Aggregate 초기화가 가능한 배열, 구조체(클래스)의 조건
    // 1. private, protected, static 데이터가 없을 것
    // 2. 사용자가 구현하거나 상속받은 생성자가 없을 것
    // 3. virtual, private, protected base 클래스가 없을 것
    // 4. virtual 메소드가 없을 것
    // 요약하면, 접근할 수 없는 암시적인 데이터가 없어야 한다는 것. 사실 당연함

    // c++17에 추가된 is_aggregate_v trait으로 Aggregate 초기화 가능한 타입인지 검사 가능
    std::cout << std::is_aggregate_v<Point1> << std::endl;
    std::cout << std::is_aggregate_v<Point2> << std::endl;
}

struct Point3 {
    int x;
    int y;
    int z{20};
};

void printPoint3(Point3& p) {
    printf("%d %d %d\n", p.x, p.y, p.z);
}

// Aggregate 초기화가 가능한 배열, 구조체(클래스)만 Designated 초기화 가능함
void desinated_init1() {
    Point3 p1;
    Point3 p2 = {1,2,3};
    Point3 p3 = {1};

    Point3 p4(1,2,3); // c++20에 추가. Point3에 생성자가 없음에도 불구하고 생성자가 있는 것처럼 초기화 가능
    Point3 p5(1);

    Point3 p6 = {.x=1, .y=2, .z=3}; // c++20에 추가. Designed 초기화
    Point3 p7 = {.y=2};

    printPoint3(p1);
    printPoint3(p2);
    printPoint3(p3);
    printPoint3(p4);
    printPoint3(p5);
    printPoint3(p6);
    printPoint3(p7);
}

struct Data {
    int arr[3];
};

struct Point {
    int x;
    int y;
};

struct Rect {
    Point left_top;
    Point right_bottom;
};

// c++에서는 무조건 생성자를 통해 초기화가 되는데, 생성자는 멤버 순서대로 초기화 함
// 따라서 이에 위배되는 Designated 초기화를 하면 에러남
void desinated_init2() {
    Point p1 = {.x=1, .y=2}; // ok
    // Point p2 = {.y=2, .x=1}; // c에서는 ok, c++에서는 error
    // Point p3 = {.x=1, 2}; // c에서는 ok, c++에서는 error

    Rect r1 = {1, 2, 3, 4}; // ok
    Rect r2 = {{1}, {3}}; // ok
    // Rect r3 = {left_top.x=10}; // c에서는 ok, c++에서는 error

    Data d1 = {1,2}; // ok
    // Data d2 = {[1]=2}; // c에서는 ok, c++에서는 error
}

void designated_init() {
    aggregate_init();
    desinated_init1();
    desinated_init2();
}
