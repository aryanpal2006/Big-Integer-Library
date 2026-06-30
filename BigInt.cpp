#include "BigInt.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

int digitValue(char ch) {
    return ch - '0';
}

}  // namespace

BigInt::BigInt() : digits_(1, 0), negative_(false) {}

BigInt::BigInt(long long value) : negative_(value < 0) {
    unsigned long long magnitude = static_cast<unsigned long long>(value < 0 ? -value : value);
    if (magnitude == 0) {
        digits_ = {0};
        negative_ = false;
        return;
    }

    digits_.clear();
    while (magnitude > 0) {
        digits_.push_back(static_cast<int>(magnitude % BASE));
        magnitude /= BASE;
    }
    normalize();
}

BigInt::BigInt(const std::string& value) : digits_(1, 0), negative_(false) {
    std::string s = value;
    if (s.empty()) {
        digits_ = {0};
        negative_ = false;
        return;
    }

    if (s[0] == '+' || s[0] == '-') {
        negative_ = (s[0] == '-');
        s = s.substr(1);
    }

    if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos) {
        throw std::invalid_argument("BigInt constructor expects a valid decimal string");
    }

    size_t index = 0;
    while (index < s.size() && s[index] == '0') {
        ++index;
    }
    if (index == s.size()) {
        digits_ = {0};
        negative_ = false;
        return;
    }
    s = s.substr(index);

    digits_.clear();
    for (size_t i = s.size(); i > 0; i -= 4) {
        size_t start = (i >= 4) ? i - 4 : 0;
        int chunk = 0;
        for (size_t j = start; j < i; ++j) {
            chunk = chunk * 10 + digitValue(s[j]);
        }
        digits_.push_back(chunk);
    }
    normalize();
}

bool BigInt::isZero() const {
    return digits_.size() == 1 && digits_[0] == 0;
}

bool BigInt::isNegative() const {
    return negative_;
}

std::string BigInt::toString() const {
    if (isZero()) {
        return "0";
    }

    std::ostringstream out;
    if (negative_) {
        out << '-';
    }

    for (std::size_t i = digits_.size(); i-- > 0;) {
        if (i == digits_.size() - 1) {
            out << digits_[i];
        } else {
            out << std::setw(4) << std::setfill('0') << digits_[i];
        }
    }
    return out.str();
}

BigInt BigInt::abs() const {
    BigInt copy(*this);
    copy.negative_ = false;
    return copy;
}

BigInt BigInt::operator+() const {
    return *this;
}

BigInt BigInt::operator-() const {
    BigInt copy(*this);
    copy.negative_ = !copy.isZero() && !copy.negative_;
    if (copy.isZero()) {
        copy.negative_ = false;
    }
    return copy;
}

void BigInt::normalize() {
    std::size_t last = digits_.size();
    while (last > 1 && digits_[last - 1] == 0) {
        --last;
    }
    digits_.resize(last);
    if (digits_.empty() || digits_[0] == 0) {
        digits_ = {0};
        negative_ = false;
    }
}

int BigInt::compareAbs(const BigInt& other) const {
    if (digits_.size() != other.digits_.size()) {
        return digits_.size() < other.digits_.size() ? -1 : 1;
    }
    for (std::size_t i = digits_.size(); i-- > 0;) {
        if (digits_[i] != other.digits_[i]) {
            return digits_[i] < other.digits_[i] ? -1 : 1;
        }
    }
    return 0;
}

void BigInt::addAbs(const BigInt& other) {
    std::vector<int> result(std::max(digits_.size(), other.digits_.size()) + 1, 0);
    int carry = 0;
    for (std::size_t i = 0; i < result.size() - 1; ++i) {
        long long value = static_cast<long long>(carry)
            + (i < digits_.size() ? digits_[i] : 0)
            + (i < other.digits_.size() ? other.digits_[i] : 0);
        result[i] = static_cast<int>(value % BASE);
        carry = static_cast<int>(value / BASE);
    }
    result.back() = carry;
    digits_ = std::move(result);
    normalize();
}

void BigInt::subAbs(const BigInt& other) {
    std::vector<int> result(digits_.size(), 0);
    int borrow = 0;
    for (std::size_t i = 0; i < digits_.size(); ++i) {
        long long diff = static_cast<long long>(digits_[i]) - borrow
            - (i < other.digits_.size() ? other.digits_[i] : 0);
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[i] = static_cast<int>(diff);
    }
    digits_ = std::move(result);
    normalize();
}

BigInt& BigInt::operator+=(const BigInt& other) {
    if (negative_ == other.negative_) {
        addAbs(other);
        negative_ = negative_;
    } else {
        int cmp = compareAbs(other);
        if (cmp == 0) {
            digits_ = {0};
            negative_ = false;
        } else if (cmp > 0) {
            subAbs(other);
            negative_ = negative_;
        } else {
            BigInt tmp(other);
            tmp.subAbs(*this);
            *this = tmp;
            negative_ = other.negative_;
        }
    }
    normalize();
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
    return *this += -other;
}

BigInt& BigInt::operator*=(const BigInt& other) {
    std::vector<long long> product(digits_.size() + other.digits_.size() + 1, 0);
    for (std::size_t i = 0; i < digits_.size(); ++i) {
        for (std::size_t j = 0; j < other.digits_.size(); ++j) {
            product[i + j] += static_cast<long long>(digits_[i]) * other.digits_[j];
        }
    }

    std::vector<int> result(product.size(), 0);
    long long carry = 0;
    for (std::size_t i = 0; i < product.size(); ++i) {
        long long value = product[i] + carry;
        result[i] = static_cast<int>(value % BASE);
        carry = value / BASE;
    }

    digits_ = std::move(result);
    negative_ = (!isZero() && negative_ != other.negative_);
    normalize();
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) {
    if (other.isZero()) {
        throw std::invalid_argument("division by zero");
    }

    std::pair<BigInt, BigInt> quotientRemainder = divMod(*this, other);
    *this = quotientRemainder.first;
    return *this;
}

BigInt& BigInt::operator%=(const BigInt& other) {
    if (other.isZero()) {
        throw std::invalid_argument("modulo by zero");
    }

    std::pair<BigInt, BigInt> quotientRemainder = divMod(*this, other);
    *this = quotientRemainder.second;
    return *this;
}

BigInt& BigInt::operator^=(const BigInt& exponent) {
    if (exponent.negative_) {
        throw std::invalid_argument("negative exponent is not supported");
    }

    BigInt base(*this);
    BigInt result(1);
    BigInt power(exponent);
    while (!power.isZero()) {
        if ((power % BigInt(2)).isZero()) {
            power /= BigInt(2);
            base *= base;
        } else {
            result *= base;
            power -= BigInt(1);
        }
    }
    *this = result;
    return *this;
}

BigInt BigInt::operator+(const BigInt& other) const {
    BigInt result(*this);
    result += other;
    return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
    BigInt result(*this);
    result -= other;
    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result(*this);
    result *= other;
    return result;
}

BigInt BigInt::operator/(const BigInt& other) const {
    BigInt result(*this);
    result /= other;
    return result;
}

BigInt BigInt::operator%(const BigInt& other) const {
    BigInt result(*this);
    result %= other;
    return result;
}

BigInt BigInt::operator^(const BigInt& exponent) const {
    if (exponent.negative_) {
        throw std::invalid_argument("negative exponent is not supported");
    }

    BigInt result(1);
    BigInt base(*this);
    BigInt power(exponent);
    while (!power.isZero()) {
        if ((power % BigInt(2)).isZero()) {
            power /= BigInt(2);
            base *= base;
        } else {
            result *= base;
            power -= BigInt(1);
        }
    }
    return result;
}

bool BigInt::operator==(const BigInt& other) const {
    return negative_ == other.negative_ && compareAbs(other) == 0;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
    if (negative_ != other.negative_) {
        return negative_ && !isZero();
    }
    int cmp = compareAbs(other);
    if (cmp == 0) {
        return false;
    }
    return negative_ ? cmp > 0 : cmp < 0;
}

bool BigInt::operator<=(const BigInt& other) const {
    return *this < other || *this == other;
}

bool BigInt::operator>(const BigInt& other) const {
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

BigInt BigInt::factorial(std::size_t n) {
    BigInt result(1);
    for (std::size_t i = 2; i <= n; ++i) {
        result *= BigInt(static_cast<long long>(i));
    }
    return result;
}

BigInt BigInt::fibonacci(std::size_t n) {
    BigInt a(0);
    BigInt b(1);
    for (std::size_t i = 0; i < n; ++i) {
        BigInt c = a + b;
        a = b;
        b = c;
    }
    return a;
}

BigInt BigInt::catalan(std::size_t n) {
    return factorial(2 * n) / (factorial(n) * factorial(n + 1));
}

BigInt BigInt::sqrt() const {
    if (negative_) {
        throw std::invalid_argument("sqrt is not defined for negative numbers");
    }
    if (isZero()) {
        return BigInt(0);
    }

    BigInt lo(0);
    BigInt hi(*this);
    while (lo < hi) {
        BigInt mid = (lo + hi + BigInt(1)) / BigInt(2);
        BigInt square = mid * mid;
        if (square <= *this) {
            lo = mid;
        } else {
            hi = mid - BigInt(1);
        }
    }
    return lo;
}

std::pair<BigInt, BigInt> BigInt::divMod(const BigInt& dividend, const BigInt& divisor) {
    if (divisor.isZero()) {
        throw std::invalid_argument("division by zero");
    }

    if (dividend < divisor) {
        return {BigInt(0), dividend};
    }

    BigInt quotient(0);
    std::vector<int> quotientDigits(dividend.digits_.size(), 0);
    BigInt remainder(0);

    for (std::size_t idx = dividend.digits_.size(); idx-- > 0;) {
        remainder *= BigInt(BASE);
        remainder += BigInt(dividend.digits_[idx]);

        int digit = 0;
        while (remainder >= divisor) {
            remainder -= divisor;
            ++digit;
        }
        quotientDigits[idx] = digit;
    }

    quotient.digits_ = std::move(quotientDigits);
    quotient.normalize();
    return {quotient, remainder};
}

std::ostream& operator<<(std::ostream& os, const BigInt& value) {
    return os << value.toString();
}
