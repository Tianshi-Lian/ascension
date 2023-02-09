#include <iostream>

#include <erika/erika.h>
#include <yuki/yuki.h>

int main() {

    int e_result = erika::test();
    int y_result = yuki::test();

    std::cout << "erika::test: " << e_result << "\n"
              << "yuki::test: " << y_result << std::endl;

    return 0;
}
