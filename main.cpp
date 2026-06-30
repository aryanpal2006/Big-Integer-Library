#include "BigInt.h"

#include <iostream>
#include <string>

int main() {
    BigInt a("123456789012345678901234567890");
    BigInt b("987654321098765432109876543210");

    std::cout << "a + b = " << (a + b) << '\n';
    std::cout << "a - b = " << (a - b) << '\n';
    std::cout << "a * b = " << (a * b) << '\n';
    std::cout << "a / b = " << (a / b) << '\n';
    std::cout << "a % b = " << (a % b) << '\n';
    std::cout << "2^20 = " << (BigInt(2) ^ BigInt(20)) << '\n';
    std::cout << "sqrt(12345678901234567890) = " << BigInt("12345678901234567890").sqrt() << '\n';

    BigInt fact1000 = BigInt::factorial(1000);
    std::cout << "1000! has " << fact1000.toString().size() << " digits\n";
    std::cout << "1000! starts with " << fact1000.toString().substr(0, 20) << '\n';

    BigInt fib20 = BigInt::fibonacci(20);
    std::cout << "Fibonacci(20) = " << fib20 << '\n';

    BigInt cat10 = BigInt::catalan(10);
    std::cout << "Catalan(10) = " << cat10 << '\n';

    return 0;
}
