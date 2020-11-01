// Standard headers
#include <iostream>     // Used instead of <stdio.h> because it is less dependent on formal typing

// 3rd party headers

// Local headers
template <typename T>
T math(const T& x, const T& y)
{
    return x + y
}

template <typename T>
class Math
{
protected:
    T x_;
    T y_;
    T z_;
public:
    Math(const T& x, const T& y)
    : x_(x)
    , y_(y)
    , z_(x+y)
    {
    }

    T f(void)
    {
        return z_;
    }

    T g(void)
    {
        return x_ - y;
    }
};

template <typename T, size_t N>
class Container
{
protected:
    T x_[N];
public:
    Container()
    {
        std::cout << "DEFAULT CONSTRUCTOR\n";
        for (auto i = 0; i < N; ++i)
        {
            x_[i] = 0;
        }
    }
    Container(T (&anArray)[N])
    {
        std::cout << "ARRAY CONSTRUCTOR...\n";
        for (auto i = 0; i < N; ++i)
        {
            std::cout <<  i << ") anArray --> " << anArray[i] << std::endl;
            x_[i] = anArray[i];
        }
    }

    void display(void)
    {
        for (auto i = 0; i < N; ++i)
        {
            std::cout <<  i << ") " << x_[i] << std::endl;
        }
    }
};