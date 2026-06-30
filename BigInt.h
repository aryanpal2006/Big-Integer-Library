#ifndef BIGINT_H
#define BIGINT_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

class BigInt {
public:
    static const int BASE = 10000;

    BigInt();
    BigInt(long long value);
    BigInt(const std::string& value);
    BigInt(const BigInt& other) = default;
    BigInt& operator=(const BigInt& other) = default;

    bool isZero() const;
    bool isNegative() const;
    std::string toString() const;

    BigInt abs() const;
    BigInt operator+() const;
    BigInt operator-() const;

    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator%=(const BigInt& other);
    BigInt& operator^=(const BigInt& exponent);

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;
    BigInt operator^(const BigInt& exponent) const;

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    static BigInt factorial(std::size_t n);
    static BigInt fibonacci(std::size_t n);
    static BigInt catalan(std::size_t n);
    BigInt sqrt() const;

    friend std::ostream& operator<<(std::ostream& os, const BigInt& value);

private:
    std::vector<int> digits_;
    bool negative_;

    void normalize();
    int compareAbs(const BigInt& other) const;
    void addAbs(const BigInt& other);
    void subAbs(const BigInt& other);
    static std::pair<BigInt, BigInt> divMod(const BigInt& dividend, const BigInt& divisor);
};

#endif
