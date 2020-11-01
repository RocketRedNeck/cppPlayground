
// Standard headers
#include <stdio.h>
#include <stdint.h>

// 3rd party headers

// Local headers
#include "templates.h"

namespace myspace
{
    uint32_t *pX = nullptr;
    uint32_t X[] = {1,2,3,4,5,6,7,8,9};
    uint32_t XX[] = {1,2,3,4,5,6,7,8,9,10};

    using X_type = uint32_t[10];
    X_type XXX = {10,9,8,7,6,5,4,3,2,1};

    using XY_type = uint64_t[10];
    XY_type XY = {10,9,8,7,6,5,4,3,2,1};

    Container<uint32_t, 10> x;           // Default constructor
    //Container<uint32_t, 10> px(pX);    // Won't compile due to type mismatch
    //Container<uint32_t, 10> x(X);      // Won't compile due to size mismatch
    Container<uint32_t, 10> xx(XX);      // Construct with matching input type(explicit)/size
    Container<uint32_t, 10> xxx(XXX);    // Construct with matching input type(implicit)/size
    //Container<uint32_t, 10> xy(XY);    // Won't compile due to type mismatch

    constexpr size_t ND = 13;
    using Y_type = double[ND];
    Y_type Y = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0};
    // Y_type YY = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0};  // Won't compile due to size error
    Container<double, ND> y(Y);
}

int main()
{
    printf("Templates!\n");

    printf("Displaying x...\n");
    myspace::x.display();

    printf("\nDisplaying xx...\n");
    myspace::xx.display();
   
    printf("\nDisplaying xxx...\n");
    myspace::xxx.display();

    printf("\nDisplaying y...\n");
    myspace::y.display();


}