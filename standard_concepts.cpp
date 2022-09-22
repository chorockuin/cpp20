#include <concepts> // core language concept 제공
#include <iterator> // 반복자 및 알고리즘 관련 concept 제공
#include <ranges> // range 관련 concept 제공
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

template<std::random_access_iterator T>
void sort(T first, T last) {
    std::cout << "random_access_iterator" << std::endl;
}

template<std::bidirectional_iterator T>
void sort(T first, T last) {
    std::cout << "bidirectional_iterator" << std::endl;
}

// 굳이 s.sort()라고 안하고 sort()로 통일해도 되었다는 얘기
void refactor_sort_function() {
    std::vector<int> v = {1,3,5,7,9,2,4,6,8,10};
    std::list<int> s = {1,3,5,7,9,2,4,6,8,10};

    std::sort(v.begin(), v.end()); // ok
    // std::sort(s.begin(), s.end()); // error
    s.sort();

    sort(v.begin(), v.end());
    sort(s.begin(), s.end());
}

// T2가 인자로 T1::value_type을 받을 수 있는 호출가능한 함수인 경우
template<typename T1, typename T2> requires std::predicate<T2, typename std::iterator_traits<T1>::value_type>
T1 find(T1 first, T1 last, T2 pred) {
    std::cout << "predicate version" << std::endl;
    return first;
}

template<typename T1, typename T2> requires (!std::predicate<T2, typename std::iterator_traits<T1>::value_type>)
T1 find(T1 first, T1 last, T2 pred) {
    std::cout << "value version" << std::endl;
    return first;
}

// 굳이 findif()라고 안하고 find()로 통일해도 됬었다는 얘기
void refactor_find_function() {
    int x[10]{1,2,3,4,5,6,7,8,9,10};
    auto p1 = find(std::begin(x), std::end(x), 3);
    auto p2 = find(std::begin(x), std::end(x), [](int n) { return n % 2 == 0;});
}

void standard_concepts() {
    refactor_sort_function();
    refactor_find_function();
}
