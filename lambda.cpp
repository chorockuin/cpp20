/*
1. 람다표현식에서 템플릿 사용 가능
2. 평가되지 않은 표현식에서 람다 표현식 사용 가능
3. 캡쳐하지 않은 람다 표현식에서 디폴트 생성자와 대입 연산자 사용 가능
4. 암시적인 this 캡쳐가 deprecated 됨
5. Parameter pack 캡쳐 가능
*/

#include <iostream>
auto add1 = [](int a, int b) {return a + b;};
auto add2 = [](auto a, auto b) {return a + b;}; // c++14. a와 b는 다른 type
auto add3 = [](auto a, decltype(a) b) {return a + b;}; // a와 b를 같은 a type으로 만들었으나 컴파일 시에 오류를 발생시킬 수는 없음
auto add4 = []<typename T>(T a, T b) {return a + b;}; // c++20. a와 b는 같은 type. 따라서 a와 b가 다른 타입으로 입력되면 컴파일 시에 오류를 발생시킴

static void lambda_template() {
    std::cout << add1(1, 2) << std::endl;
    std::cout << add1(1.1, 2.2) << std::endl;
    std::cout << add1(1, 2.2) << std::endl;

    std::cout << add2(1, 2) << std::endl;
    std::cout << add2(1.1, 2.2) << std::endl;
    std::cout << add2(1, 2.2) << std::endl;

    std::cout << add3(1, 2.2) << std::endl; // no error

    // std::cout << add4<int>(1, 2.2) << std::endl; // syntax error
    // std::cout << add4(1, 2.2) << std::endl; // type error
    // std::cout << add4.operator()(1, 2.2) << std::endl; // type error. 람다 표현식은 결국 () 연산자 호출하는 것
    std::cout << add4.operator()<int>(1, 2.2) << std::endl; // no error
}

#include <memory>
struct Freer
{
    inline void operator()(void *p) const noexcept
    {
        std::cout << "free" << std::endl;
        free(p);
    }  
};

static void lambda_unique_ptr1() {
    std::unique_ptr<int> up1(new int); // unique_ptr 소멸자에서 delete로 잘 소멸됨
    std::unique_ptr<int, Freer> up2(static_cast<int *>(malloc(100))); // unique_ptr 소멸자에서 delete로 소멸되면 안되므로, 함수 객체 type을 넣음
    // std::unique_ptr<int, [](int *p) {free(p);}> up3(static_cast<int *>(malloc(100))); // 람다는 임시 객체이므로 type에 들어갈 수 없음
    std::unique_ptr<int, decltype([](int *p) {free(p);})> up3(static_cast<int *>(malloc(100))); // 람다를 type으로 선언해서 넣으면 됨. c++17에선 안되지만 c++ 20에서는 됨
}

static int add5(int a, int b); // {return a + b;} // sizeof() 내에 들어가는 함수라 평가되지 않으므로 구현이 필요 없음
static void lambda_unevaluated_expression() {
    std::cout << sizeof(int) << std::endl;
    std::cout << sizeof(add5(1, 2)) << std::endl; // sizeof 안은 평가되지 않는 표현식(런타임에 평가 되지 않고 컴파일 타임에만 사용됨)
    decltype(add5(1, 2)) n; // 마찬가지로 평가되지 않는 표현식
    std::cout << sizeof([](int a, int b) {return a + b;}) << std::endl; // 람다릉 평가되지 않는 표현식으로 사용 가능. sizeofc++17에서는 에러, c++20에서는 됨
    std::cout << sizeof([](int a, int b) {return a + b;}(1, 2)) << std::endl; // 람다릉 평가되지 않는 표현식으로 사용 가능. sizeofc++17에서는 에러, c++20에서는 됨
    std::unique_ptr<int, decltype([](int *p) {delete p;})> up(new int); // 마찬가지로 람다를 type으로 선언해서 넣으면 됨. c++17에선 안되지만 c++ 20에서는 됨
}

static void lmabda_default_constructor() {
    auto f = [](int a, int b) {return a + b;};

    decltype(f) f1; // c++17 에러. c++20 됨. 람다에 디폴트 생성자가 생겼다는 얘기
    decltype(f) f2 = f1; // c++17, c++20 모두 됨. 람다에 복사 생성자는 원래 있었다는 얘기
    f2 = f1; // c++17 에러, c++20 됨. 람다에 대입 연산자 원래 있었다는 얘기

    int v = 10;
    auto ff = [v](int a, int b) {return a + b;}; // 람다가 지역변수 v를 캡쳐했음

    // decltype(ff) ff1; // 지역변수 v 캡쳐해서 안됨
    decltype(ff) ff2 = ff; // c++17, c++20 모두 됨
    // ff2 = ff1; // 지역변수 v 캡쳐해서 안됨
}

template<typename T, typename D> 
class unique_ptr {
    T* obj;
public:
    explicit unique_ptr(T* p = 0) : obj(p) {}
    ~unique_ptr() {
        D d; // type D의 디폴트 생성자 필요함
        d(obj);
    }
};

static void lambda_unique_ptr2() {
    // unique_ptr<int, decltype([](int *p){delete p;})> p1(new int); // c++17 에러, c++20 됨
    auto f = [](int *p){delete p;};
    unique_ptr<int, decltype(f)> p1(new int); // 이렇게 하면 c++17(심지어 c++11에서도)될 것 같지만, unique_ptr 소멸자에서 type D의 디폴트 생성자가 없기 때문에 안됨. c++20에서만 됨
}

#include <functional>
struct Sample {
    int value = 0;
    auto foo() {
        int n = 10;
        // auto f = [=](int a) { return a + n + value; }; // [=] 표현은 모든 지역 변수를 캡쳐. 그리고 암시적으로 this 포인터도 캡쳐
        auto f = [=, this](int a) { return a + n + value; }; // 이렇게 명시적으로 this 포인터 캡쳐를 알려야 경고가 뜨지 않음
        std::cout << sizeof(f) << std::endl; // n + this 포인터. 따라서 size는 8
        return f;
    }

    auto bar() {
        int n = 10;
        auto f = [=, *this](int a) { return a + n + value; }; // 명시적으로 this를 써 주자
        std::cout << sizeof(f) << std::endl; // 모든 지역 변수 뿐 아니라 암시적 this 포인터도 캡쳐. 따라서 size는 8
        return f;

    }
};

std::function<int(int)> f;

static void goo() {
    Sample s;
    f = s.foo();
    std::cout << f(10) << std::endl; // s가 캡쳐되어 a=10, n=10, value=0이 됨. 따라서 결과는 20
}

static void hoo() {
    Sample s;
    f = s.bar();
    std::cout << f(10) << std::endl; // s가 캡쳐되어 a=10, n=10, value=0이 됨. 따라서 결과는 20
}


static void lambda_this_capture() {
    goo();
    std::cout << f(10) << std::endl; // s는 이미 파괴되었으므로, n=10, value=0을 가리키는 포인터들은 유효하지 않음. 따라서 결과는 쓰레기 값

    hoo();
    std::cout << f(10) << std::endl; // s를 값으로 캡쳐했으므로, n=10, value=0은 복제되어 있음. 따라서 결과는 20
}

template<typename ... Types> // 가변길이 템플릿. 가변의 타입을 Args로 추상화 시켜 받음
auto f1(Types&&... args) { // Types 중에 r-value가 있을 수도 있기 때문에 r-value 참조 문법 && 사용
    return [...lambda_args = std::forward<Types>(args)]() { // 파라미터 팩 args를 값으로 캡쳐해서 그대로 인자로 사용하는 lambda 정의
        (std::cout << ... << lambda_args); 
    };
}

template<typename ... Types> 
auto f2(Types&&... args) {
    return [&...lambda_args = std::forward<Types>(args)]() { // 파라미터 팩 args를 참조로 캡쳐
        (std::cout << ... << lambda_args); 
    };
}

static void lambda_capture_parameter_pack() {
    f1(1, 2, 3)();
    std::cout << std::endl;
    
    // f2(1, 2, 3)(); // r-value 파라미터 팩을 값으로 캡쳐하지 않고 참조로 캡쳐했으므로 에러 남

    int a=1, b=2, c=3;
    f2(a, b, c)();
    std::cout << std::endl;
}

void lambda() {
    lambda_template();
    lambda_unique_ptr1();
    lambda_unevaluated_expression();
    lmabda_default_constructor();
    lambda_unique_ptr2();
    lambda_this_capture();
    lambda_capture_parameter_pack();
}
