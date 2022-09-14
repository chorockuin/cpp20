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

int foo() { return 0; }

void no_discard() {
    allocate(10); // [[nodiscard]] 명시하지 않으면 아무런 경고가 뜨지 않음

    // std::future<int> r =
    std::async(std::launch::async, foo); // std::async 함수가 [[nodiscard]]가 붙은 전형적인 함수
}

struct MyType {
    [[nodiscard]] MyType(int n, bool b) {} // 생성자 앞에도 선언 가능
};

enum class [[nodiscard]] ErrorCode {OK, WARNING, CRITICAL, FATAL}; // enum 앞에도 선언 가능

ErrorCode goo() { return ErrorCode::CRITICAL; }

void no_discard2() {
    goo(); // [[nodiscard]] 덕분에 경고 발생
    MyType(1, true); // [[nodiscard]] 덕분에 경고 발생
}

int likely(int i) {
    if (i > 0) [[likely]] // 여기를 탈 확률이 높다고 컴파일러에게 알려줘서 컴파일러가 최적화 가능하게 함
    // if (__bulitin_expect(i>0, 1)) // 리눅스 커널 소스에는 위와 동일한 의미로 이렇게 썼었음
        i += 2;
    else
        i -= 2;
    return i;
}

int unlikely(int i) {
    if (i > 0) [[unlikely]] // 여기를 탈 확률이 적다고 컴파일러에게 알려줘서 컴파일러가 최적화 가능하게 함
    // if (__bulitin_expect(i>0, 0)) // 리눅스 커널 소스에는 위와 동일한 의미로 이렇게 썼었음
        i += 2;
    else
        i -= 2;
    return i;
}

void attributes() {
    no_discard();
    no_discard2();
    likely(1);
    unlikely(-1);
}
