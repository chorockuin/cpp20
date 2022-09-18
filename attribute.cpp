/*
[[nodiscard("reason")]]
[[likely]]
[[unlikely]]
[[no_unique_address]]
*/

#include <future>
[[nodiscard("memory leak may occur.")]] int *allocate(int size) { // c++20에서는 [[nodiscard]] 안에 경고 메세지를 넣을 수 있음
    return new int[size];
}

static int foo() { return 0; }

static void no_discard() {
    allocate(10); // [[nodiscard]] 명시하지 않으면 new한 포인터를 핸들링하지 않아도 아무런 경고가 뜨지 않음

    // std::future<int> r =
    std::async(std::launch::async, foo); // std::async 함수가 [[nodiscard]]가 붙은 전형적인 함수
}

struct MyType {
    [[nodiscard]] MyType(int n, bool b) {} // 생성자 앞에도 선언 가능
};

enum class [[nodiscard]] ErrorCode {OK, WARNING, CRITICAL, FATAL}; // enum 앞에도 선언 가능

ErrorCode goo() { return ErrorCode::CRITICAL; }

static void no_discard2() {
    goo(); // [[nodiscard]] 덕분에 경고 발생
    MyType(1, true); // [[nodiscard]] 덕분에 경고 발생
}

static int likely(int i) {
    if (i > 0) [[likely]] // 여기를 탈 확률이 높다고 컴파일러에게 알려줘서 컴파일러가 최적화 가능하게 함
    // if (__bulitin_expect(i>0, 1)) // 리눅스 커널 소스에는 위와 동일한 의미로 이렇게 썼었음
        i += 2;
    else
        i -= 2;
    return i;
}

static int unlikely(int i) {
    if (i > 0) [[unlikely]] // 여기를 탈 확률이 적다고 컴파일러에게 알려줘서 컴파일러가 최적화 가능하게 함
    // if (__bulitin_expect(i>0, 0)) // 리눅스 커널 소스에는 위와 동일한 의미로 이렇게 썼었음
        i += 2;
    else
        i -= 2;
    return i;
}

// non-static 멤버 데이터와 가상함수가 없는 클래스가 empty 클래스
struct Empty {
    void foo() {}
};

// 어떤 클래스로 객체를 만들던 각 객체를 가리키는 메모리 주소는 고유해야 함(C++ 표준)
// 따라서 empty 클래스라고 하더라도 크기는 1
struct Data {
    Empty e; // 1 byte + 3 byte padding
    int n; // 4 byte
};

// 그러나 [[no_unique_address]] 옵션을 사용하면
// 객체를 가리키는 메모리 주소가 고유할 필요가 없다고 선언하는 것
// c++20 이전에는 EBCO(Empty Base Class Optimization)의 compressed pair 기법을 사용했었음
struct EmptyData {
    [[no_unique_address]] Empty e; // 0 byte(cl에서는 4 byte)
    int n; // 4 byte
};

#include <iostream>
static void no_unique_address() {
    std::cout << sizeof(Empty) << std::endl;
    std::cout << sizeof(Data) << std::endl;
    std::cout << sizeof(EmptyData) << std::endl;
}

template<typename T, typename U> struct PAIR {
    [[no_unique_address]] T first;
    [[no_unique_address]] U second;

    template<typename A, typename B>
    PAIR(A&& a, B&& b) : first(std::forward<A>(a)), second(std::forward<B>(b)) {
    }
};

// C++17 클래스 템플릿 타입 deduction 가이드
template<typename A, typename B> PAIR(A&& a, B&& b) -> PAIR<A, B>;

static void no_unique_address2() {
    // 메모리 주소와 삭제자를 보관
    // 삭제자는 캡쳐하지 않은 lambda 표현식이기 때문에 empty class
    PAIR pair(malloc(100), [](void *p) {free(p);});

    std::cout << sizeof(pair) << std::endl; // 4 byte + 0 byte(cl에서는 4 byte) = 4 byte
}

void attributes() {
    no_discard();
    no_discard2();
    likely(1);
    unlikely(-1);
    no_unique_address();
    no_unique_address2();
}
