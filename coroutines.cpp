#include <iostream>
#include <coroutine>
#include <thread>
#include <chrono>

// Generator는 어떤 규칙을 따라야 함(Coroutine Framework)
// 1. promise_type이 있어야 하고, 최소 아래 함수들을 구현해야 함
//    get_return_object()
//    initial_suspend()
//    unhandled_exception()
//    return_void()
//    final_suspend()
// 2. std::coroutine_handle<> 멤버가 있어야 함
// 3. 인자가 한 개인 생성자
// 4. 소멸자에서 coroutine 파괴
template<typename T>
struct Generator {
    struct Promise {
        T value; // yield value
        Generator get_return_object() {
            return Generator{std::coroutine_handle<Promise>::from_promise(*this)};
        }
        auto initial_suspend() {return std::suspend_always{};}
        auto yield_value(T x) {value=x; return std::suspend_always{}; }
        auto return_void() noexcept {return std::suspend_never{};}
        auto final_suspend() noexcept {return std::suspend_always{};}
        void unhandled_exception() {std::exit(1);}
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> cr;

    Generator(std::coroutine_handle<promise_type> h) : cr(h) {}
    ~Generator() {if(cr) cr.destroy();}

    class Iter {
    public:
        void operator ++() {cr.resume();}
        const T& operator *() const {return cr.promise().value;}
        bool operator ==(std::default_sentinel_t) const {
            return !cr || cr.done();
        }
        explicit Iter(std::coroutine_handle<promise_type> cr) : cr{cr} {}
    private:
        std::coroutine_handle<promise_type> cr;
    };

    Iter begin() {
        if (cr) {
            cr.resume();
        } 
        return Iter{cr};
    }
    std::default_sentinel_t end() {return {};}
};

Generator<int> foo(int n) {
    std::cout << "\tRun 1 :" << std::this_thread::get_id() << std::endl;
    co_await std::suspend_always{};
    // 위 코드는 아래와 같은 표현
    // co_await std::suspend_always{};
    std::cout << "\tRun 2 :" << std::this_thread::get_id() << std::endl;
    co_yield 10;
    std::cout << "\tRun 3 :" << std::this_thread::get_id() << std::endl;

    // 위 코드들은 컴파일러가 아래와 같이 변환함
    // Generator::promise_type pm;
    // Generator g = pm.get_return_object();
    // co_await pm.initial_suspend();
    // try {
    //     std::cout << "\tRun 1 :" << std::endl;
    //     std::suspend_always awaiter;
    //     if (!awaiter.await_ready()) {
    //        awaiter.await_suspend(g.cr)
    //     }
    //     awaiter.await_resume();
    //     std::cout << "\tRun 2 :" << std::endl;
    //     co_await pm.yield_value(10)
    //     std::cout << "\tRun 3 :" << std::endl;
    // }
    // catch (...) {
    //     pm.unhandled_exception();
    // }
    // pm.return_void();
    // co_await pm.final_suspend();
}

static void intro() {
    Generator<int> g = foo(10);
    std::cout << "Main 1 : " << std::this_thread::get_id() << " " << g.cr.promise().value << std::endl;
    g.cr.resume();
    std::cout << "Main 2 : " << std::this_thread::get_id() << " " << g.cr.promise().value << std::endl;
    g.cr.resume();
    std::cout << "Main 3 : " << std::this_thread::get_id() << " " << g.cr.promise().value << std::endl;
    g.cr.resume();
}

Generator<int> Gen(int first, int last) {
    for (int i = first; i <= last; ++i) {
        co_yield i;
    }
}

static void extend() {
    Generator<int> g1 = Gen(10, 20);
    while (!g1.cr.done()) {
        g1.cr.resume();
        std::cout << g1.cr.promise().value << std::endl;
    }

    // 아래와 같이 편하게 사용하려면
    // Generator 안에 begin(), end(), 반복자를 구현하면 됨
    Generator<int> g2 = Gen(10, 20);
    for (auto n : g2) {
        std::cout << n << std::endl;
    }
}

struct resume_new_thread {
    void await_suspend(std::coroutine_handle<> cr) {
        std::thread t([cr]() {cr.resume();});
        t.detach();
    }
    constexpr bool await_ready() const noexcept {return false;}
    constexpr void await_resume() const noexcept {}
};

Generator<int> hoo() {
    std::cout << "Run1 : " << std::this_thread::get_id() << std::endl;
    // 원래라면 그냥 코루틴이 suspend되겠으나
    // 커스터마이징 한 resume_new_thread::await_suspend() 함수 내에서
    // 새로운 쓰레드를 만들고 코루틴을 resume()하고 있음
    // 코루틴이 resume된 쓰레드는 새로 생성된 쓰레드 이므로
    // 원래 코루틴이 시작된 쓰레드와는 id 값이 다름
    co_await resume_new_thread{};
    std::cout << "Run2 : " << std::this_thread::get_id() << std::endl;
}

static void custom_await_object() {
    Generator<int> g = hoo();
    g.cr.resume();
    std::cout << "Main : " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void coroutine() {
    intro();
    extend();
    custom_await_object();
}