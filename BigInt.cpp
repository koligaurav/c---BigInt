#include<iostream>
#include<string>
#include<cmath>
#include<algorithm>
#include<vector>

class BigInt {
private:
    std::string number;  // To store the large number as a string
    bool isNegative;     // To check if the number is negative

    // Helper function to remove leading zeros
    void removeLeadingZeros() {
        while (number.length() > 1 && number[0] == '0') {
            number.erase(0, 1);  // Remove leading zeros
        }
    }

    // Helper function to add two positive strings
    std::string addStrings(const std::string &num1, const std::string &num2) {
        std::string result = "";
        int carry = 0, sum = 0;
        int n1 = num1.size(), n2 = num2.size();
        
        std::string rev1(num1.rbegin(), num1.rend());
        std::string rev2(num2.rbegin(), num2.rend());

        for (int i = 0; i < std::max(n1, n2); i++) {
            int digit1 = (i < n1) ? rev1[i] - '0' : 0;
            int digit2 = (i < n2) ? rev2[i] - '0' : 0;
            sum = digit1 + digit2 + carry;
            carry = sum / 10;
            result += (sum % 10) + '0';
        }

        if (carry) result += carry + '0';
        std::reverse(result.begin(), result.end());
        return result;
    }

    // Helper function to subtract two positive strings (num1 >= num2)
    std::string subtractStrings(const std::string &num1, const std::string &num2) {
        std::string result = "";
        int borrow = 0, diff = 0;
        int n1 = num1.size(), n2 = num2.size();
        
        std::string rev1(num1.rbegin(), num1.rend());
        std::string rev2(num2.rbegin(), num2.rend());

        for (int i = 0; i < n1; i++) {
            int digit1 = rev1[i] - '0';
            int digit2 = (i < n2) ? rev2[i] - '0' : 0;
            diff = digit1 - digit2 - borrow;
            
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }

            result += diff + '0';
        }

        std::reverse(result.begin(), result.end());
        removeLeadingZeros();
        return result;
    }

    // Helper function for multiplication of two numbers
    std::string multiplyStrings(const std::string &num1, const std::string &num2) {
        std::vector<int> result(num1.size() + num2.size(), 0);
        for (int i = num1.size() - 1; i >= 0; i--) {
            for (int j = num2.size() - 1; j >= 0; j--) {
                int mul = (num1[i] - '0') * (num2[j] - '0');
                int sum = result[i + j + 1] + mul;
                result[i + j + 1] = sum % 10;
                result[i + j] += sum / 10;
            }
        }

        std::string resultStr;
        for (int num : result) {
            if (!(resultStr.empty() && num == 0)) {
                resultStr += std::to_string(num);
            }
        }
        return resultStr.empty() ? "0" : resultStr;
    }

    // Helper function for long division
    std::string divideStrings(const std::string &num1, const std::string &num2) {
        std::string quotient = "";
        BigInt dividend(num1);
        BigInt divisor(num2);
        BigInt current("0");

        if (dividend < divisor) return "0";

        for (char digit : num1) {
            current = current * BigInt("10") + BigInt(std::string(1, digit));
            int qDigit = 0;

            while (current >= divisor) {
                current = current - divisor;
                qDigit++;
            }

            quotient += std::to_string(qDigit);
        }

        return quotient.empty() ? "0" : quotient;
    }

public:
    // Constructor to initialize BigInt from string
    BigInt(const std::string &num) {
        if (num[0] == '-') {
            isNegative = true;
            number = num.substr(1);  // Remove the negative sign
        } else {
            isNegative = false;
            number = num;
        }
        removeLeadingZeros();
    }

    // Addition operator
    BigInt operator+(const BigInt &other) {
        if (isNegative == other.isNegative) {
            BigInt result(addStrings(number, other.number));
            result.isNegative = isNegative;
            return result;
        } else {
            if (number >= other.number) {
                BigInt result(subtractStrings(number, other.number));
                result.isNegative = isNegative;
                return result;
            } else {
                BigInt result(subtractStrings(other.number, number));
                result.isNegative = other.isNegative;
                return result;
            }
        }
    }

    // Subtraction operator
    BigInt operator-(const BigInt &other) {
        if (isNegative != other.isNegative) {
            BigInt result(addStrings(number, other.number));
            result.isNegative = isNegative;
            return result;
        } else {
            if (number >= other.number) {
                BigInt result(subtractStrings(number, other.number));
                result.isNegative = isNegative;
                return result;
            } else {
                BigInt result(subtractStrings(other.number, number));
                result.isNegative = !isNegative;
                return result;
            }
        }
    }

    // Multiplication operator
    BigInt operator*(const BigInt &other) {
        BigInt result(multiplyStrings(number, other.number));
        result.isNegative = (isNegative != other.isNegative);
        return result;
    }

    // Division operator
    BigInt operator/(const BigInt &other) {
        if (other == BigInt("0")) throw std::invalid_argument("Division by zero!");
        BigInt result(divideStrings(number, other.number));
        result.isNegative = (isNegative != other.isNegative);
        return result;
    }
// Division operator
    BigInt operator/(const BigInt &other) {
        if (other == BigInt("0")) throw std::invalid_argument("Division by zero!");
        BigInt result(divideStrings(number, other.number));
        result.isNegative = (isNegative != other.isNegative);
        return result;
    }

    // Modulo operator
    BigInt operator%(const BigInt &other) const {
        BigInt remainder("0");
        BigInt divisor = other;

        if (*this < divisor) {
            return *this;
        }

        for (char digit : number) {
            remainder = remainder * BigInt("10") + BigInt(std::string(1, digit));
            while (remainder >= divisor) {
                remainder = remainder - divisor;
            }
        }

        return remainder;
    }

    // Greater than operator (BigInt > BigInt)
    bool operator>(const BigInt &other) const {
        if (isNegative != other.isNegative) {
            return other.isNegative;
        }

        if (number.length() != other.number.length()) {
            return (number.length() > other.number.length()) ^ isNegative;
        }

        return (number > other.number) ^ isNegative;
    }

    // Equality operator (==)
    bool operator==(const BigInt &other) const {
        return (isNegative == other.isNegative && number == other.number);
    }

    // Less than operator (BigInt < BigInt)
    bool operator<(const BigInt &other) const {
        return !(*this > other) && !(*this == other);
    }

    // Greater than or equal to operator (BigInt >= BigInt)
    bool operator>=(const BigInt &other) const {
        return (*this > other) || (*this == other);
    }

    // Less than or equal to operator (BigInt <= BigInt)
    bool operator<=(const BigInt &other) const {
        return (*this < other) || (*this == other);
    }

    // Display function
    void display() const {
        if (isNegative) std::cout << "-";
        std::cout << number << std::endl;
    }

    // Static Fibonacci function
    static BigInt fibonacci(int n) {
        if (n == 0) return BigInt("0");
        if (n == 1) return BigInt("1");
        BigInt a("0"), b("1"), c("1");
        for (int i = 2; i <= n; i++) {
            c = a + b;
            a = b;
            b = c;
        }
        return c;
    }

    // Static Catalan number function
    static BigInt catalan(int n) {
        BigInt result = factorial(2 * n) / (factorial(n + 1) * factorial(n));
        return result;
    }

    // Static factorial function
    static BigInt factorial(int n) {
        BigInt result("1");
        for (int i = 2; i <= n; i++) {
            result = result * BigInt(std::to_string(i));
        }
        return result;
    }
};

int main() {
    BigInt a("123456789123456789");
    BigInt b("987654321987654321");

    BigInt sum = a + b;
    BigInt diff = a - b;
    BigInt multi = a * b;

    sum.display();  // Display sum
    diff.display(); // Display difference
    multi.display();// Display product

    BigInt fib50 = BigInt::fibonacci(50);  // 50th Fibonacci number
    fib50.display();

    BigInt catalan10 = BigInt::catalan(10);  // 10th Catalan number
    catalan10.display();

    return 0;
}
