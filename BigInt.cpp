#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdexcept>

class BigInt {
private:
    std::string number;
    bool isNegative;

    void removeLeadingZeros() {
        size_t nonZeroIndex = number.find_first_not_of('0');
        if (nonZeroIndex == std::string::npos) {
            number = "0";
            isNegative = false;
        } else {
            number = number.substr(nonZeroIndex);
        }
    }

    static std::string addStrings(const std::string &num1, const std::string &num2) {
        std::string result;
        int carry = 0;
        int i = num1.length() - 1;
        int j = num2.length() - 1;

        while (i >= 0 || j >= 0 || carry > 0) {
            int digit1 = (i >= 0) ? num1[i--] - '0' : 0;
            int digit2 = (j >= 0) ? num2[j--] - '0' : 0;
            int sum = digit1 + digit2 + carry;
            carry = sum / 10;
            result.push_back(sum % 10 + '0');
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    static std::string subtractStrings(const std::string &num1, const std::string &num2) {
        std::string result;
        int borrow = 0;
        int i = num1.length() - 1;
        int j = num2.length() - 1;

        while (i >= 0) {
            int digit1 = num1[i--] - '0';
            int digit2 = (j >= 0) ? num2[j--] - '0' : 0;
            int diff = digit1 - digit2 - borrow;

            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }

            result.push_back(diff + '0');
        }

        std::reverse(result.begin(), result.end());
        size_t nonZeroIndex = result.find_first_not_of('0');
        if (nonZeroIndex != std::string::npos) {
            result = result.substr(nonZeroIndex);
        } else {
            result = "0";
        }
        return result;
    }

    static bool isSmaller(const std::string &num1, const std::string &num2) {
        if (num1.length() < num2.length()) return true;
        if (num1.length() > num2.length()) return false;
        return num1 < num2;
    }

    static std::string multiplyStrings(const std::string &num1, const std::string &num2) {
        if (num1 == "0" || num2 == "0") return "0";
        
        std::vector<int> result(num1.size() + num2.size(), 0);
        
        for (int i = num1.size() - 1; i >= 0; i--) {
            for (int j = num2.size() - 1; j >= 0; j--) {
                int product = (num1[i] - '0') * (num2[j] - '0');
                int sum = product + result[i + j + 1];
                result[i + j + 1] = sum % 10;
                result[i + j] += sum / 10;
            }
        }

        std::string res;
        for (int num : result) {
            if (!(res.empty() && num == 0)) {
                res.push_back(num + '0');
            }
        }
        return res.empty() ? "0" : res;
    }

    static std::pair<std::string, std::string> divideStrings(std::string dividend, std::string divisor) {
        if (divisor == "0") throw std::runtime_error("Division by zero");
        
        if (isSmaller(dividend, divisor)) {
            return std::make_pair("0", dividend);
        }

        std::string quotient;
        std::string current;
        size_t index = 0;

        while (index < dividend.size()) {
            current.push_back(dividend[index++]);
            
            // Remove leading zeros
            current.erase(0, current.find_first_not_of('0'));
            if (current.empty()) current = "0";
            
            if (isSmaller(current, divisor)) {
                if (!quotient.empty()) {
                    quotient.push_back('0');
                }
                continue;
            }
            
            int count = 0;
            std::string tempDivisor = divisor;
            while (!isSmaller(current, tempDivisor)) {
                current = subtractStrings(current, tempDivisor);
                count++;
            }
            
            quotient.push_back(count + '0');
        }
        
        if (quotient.empty()) quotient = "0";
        
        current.erase(0, current.find_first_not_of('0'));
        if (current.empty()) current = "0";
        
        return std::make_pair(quotient, current);
    }

public:
    BigInt(const std::string &num = "0") {
        if (num.empty()) {
            number = "0";
            isNegative = false;
            return;
        }

        size_t start = 0;
        isNegative = false;
        
        if (num[0] == '-') {
            isNegative = true;
            start = 1;
        }
        
        number = num.substr(start);
        removeLeadingZeros();
        
        if (number == "0") isNegative = false;
    }

    BigInt operator+(const BigInt &other) const {
        if (isNegative == other.isNegative) {
            BigInt result(addStrings(number, other.number));
            result.isNegative = isNegative;
            return result;
        }
        
        if (isSmaller(number, other.number)) {
            BigInt result(subtractStrings(other.number, number));
            result.isNegative = other.isNegative;
            return result;
        } else {
            BigInt result(subtractStrings(number, other.number));
            result.isNegative = isNegative;
            return result;
        }
    }

    BigInt operator-(const BigInt &other) const {
        if (isNegative != other.isNegative) {
            BigInt result(addStrings(number, other.number));
            result.isNegative = isNegative;
            return result;
        }
        
        if (isSmaller(number, other.number)) {
            BigInt result(subtractStrings(other.number, number));
            result.isNegative = !isNegative;
            return result;
        } else {
            BigInt result(subtractStrings(number, other.number));
            result.isNegative = isNegative;
            return result;
        }
    }

    BigInt operator*(const BigInt &other) const {
        BigInt result(multiplyStrings(number, other.number));
        result.isNegative = isNegative != other.isNegative;
        if (result.number == "0") result.isNegative = false;
        return result;
    }

    BigInt operator/(const BigInt &other) const {
        std::pair<std::string, std::string> divResult = divideStrings(number, other.number);
        BigInt result(divResult.first);
        result.isNegative = isNegative != other.isNegative;
        if (result.number == "0") result.isNegative = false;
        return result;
    }

    BigInt operator%(const BigInt &other) const {
        std::pair<std::string, std::string> divResult = divideStrings(number, other.number);
        BigInt result(divResult.second);
        result.isNegative = isNegative;
        if (result.number == "0") result.isNegative = false;
        return result;
    }

    bool operator==(const BigInt &other) const {
        return number == other.number && isNegative == other.isNegative;
    }

    bool operator!=(const BigInt &other) const {
        return !(*this == other);
    }

    bool operator<(const BigInt &other) const {
        if (isNegative != other.isNegative) {
            return isNegative;
        }
        
        if (isNegative) {
            if (number.length() != other.number.length()) {
                return number.length() > other.number.length();
            }
            return number > other.number;
        } else {
            if (number.length() != other.number.length()) {
                return number.length() < other.number.length();
            }
            return number < other.number;
        }
    }

    bool operator<=(const BigInt &other) const {
        return *this < other || *this == other;
    }

    bool operator>(const BigInt &other) const {
        return !(*this <= other);
    }

    bool operator>=(const BigInt &other) const {
        return !(*this < other);
    }

    friend std::ostream& operator<<(std::ostream &os, const BigInt &bi) {
        if (bi.isNegative) os << '-';
        os << bi.number;
        return os;
    }

    static BigInt fibonacci(int n) {
        if (n < 0) throw std::invalid_argument("Fibonacci not defined for negative numbers");
        if (n == 0) return BigInt("0");
        
        BigInt a("0"), b("1");
        for (int i = 2; i <= n; ++i) {
            BigInt temp = a + b;
            a = b;
            b = temp;
        }
        return b;
    }

    static BigInt factorial(int n) {
        if (n < 0) throw std::invalid_argument("Factorial not defined for negative numbers");
        BigInt result("1");
        for (int i = 2; i <= n; ++i) {
            result = result * BigInt(std::to_string(i));
        }
        return result;
    }

    static BigInt catalan(int n) {
        if (n < 0) throw std::invalid_argument("Catalan not defined for negative numbers");
        BigInt a = factorial(2 * n);
        BigInt b = factorial(n + 1);
        BigInt c = factorial(n);
        return a / (b * c);
    }
};

int main() {
    try {
        // Test basic operations
        BigInt a("123456789123456789");
        BigInt b("987654321987654321");
        
        std::cout << "a = " << a << std::endl;
        std::cout << "b = " << b << std::endl;
        
        std::cout << "a + b = " << (a + b) << std::endl;
        std::cout << "a - b = " << (a - b) << std::endl;
        std::cout << "b - a = " << (b - a) << std::endl;
        std::cout << "a * b = " << (a * b) << std::endl;
        std::cout << "b / a = " << (b / a) << std::endl;
        std::cout << "a % b = " << (a % b) << std::endl;
        
        // Test comparison operators
        std::cout << "a < b: " << (a < b) << std::endl;
        std::cout << "a > b: " << (a > b) << std::endl;
        std::cout << "a == b: " << (a == b) << std::endl;
        
        // Test special functions
        std::cout << "Fibonacci(50) = " << BigInt::fibonacci(50) << std::endl;
        std::cout << "Catalan(10) = " << BigInt::catalan(10) << std::endl;
        std::cout << "Factorial(20) = " << BigInt::factorial(20) << std::endl;
        
        // Test negative numbers
        BigInt c("-123456789");
        BigInt d("987654321");
        
        std::cout << "c = " << c << std::endl;
        std::cout << "d = " << d << std::endl;
        
        std::cout << "c + d = " << (c + d) << std::endl;
        std::cout << "c - d = " << (c - d) << std::endl;
        std::cout << "d - c = " << (d - c) << std::endl;
        std::cout << "c * d = " << (c * d) << std::endl;
        std::cout << "d / c = " << (d / c) << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
