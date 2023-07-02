
#include <cstdio>

template <typename R, typename T>
R sum(const T& x)
{
    return static_cast<R>(x);
}

template <typename R, typename T, typename... Args>
R sum(const T& x, const Args&... args)
{
    return static_cast<R>(x) + sum<R>(args...);
}

int main()
{
    printf("Sum is %d\n", sum<int>(1,2,3,4,5));
    printf("Sum is %llu\n", sum<unsigned long long>(1,2,3,4,50000000000000ll,6,7,8,9));
    printf("Sum is %f\n", sum<float>(1.2, 3, 0.01));
}