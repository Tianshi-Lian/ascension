#include <iostream>

#include <erika/erika.h>
#include <yuki/yuki.h>

int main() {

    i32 e_result = erika::test();
    i32 y_result = yuki::test();

    std::cout << "erika::test: " << e_result << "\n"
              << "yuki::test: " << y_result << std::endl;

    return 0;
}
