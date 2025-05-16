#include "BigNumber.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

void BigNumber::clear() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

void BigNumber::reverse() {
    Node* prev = nullptr;
    Node* current = head;
    while (current) {
        Node* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head = prev;
}

void BigNumber::removeLeadingZeros() {
    reverse();
    while (head && head->digit == 0 && head->next) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    reverse();
    if (head && head->digit == 0 && !head->next) {
        isNegative = false;
        decimalPos = 0;
    }
}

int BigNumber::digitCount() const {
    int count = 0;
    Node* current = head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

BigNumber::BigNumber() : head(nullptr), isNegative(false), decimalPos(0) {}

BigNumber::BigNumber(const std::string& num) : head(nullptr), isNegative(false), decimalPos(0) {
    bool seenDecimal = false;
    for (char c : num) {
        if (c == '-') {
            isNegative = true;
            continue;
        }
        else if (c == '.') {
            seenDecimal = true;
            continue;
        }
        else if (std::isdigit(c)) {
            Node* newNode = new Node(c - '0');
            newNode->next = head;
            head = newNode;
            if (seenDecimal) decimalPos++;
        }
    }
    if (!head) head = new Node(0);
    reverse();
    removeLeadingZeros();
}

BigNumber::BigNumber(const BigNumber& other) : head(nullptr), isNegative(other.isNegative), decimalPos(other.decimalPos) {
    Node* current = other.head;
    while (current) {
        Node* newNode = new Node(current->digit);
        newNode->next = head;
        head = newNode;
        current = current->next;
    }
    reverse();
}

BigNumber::~BigNumber() {
    clear();
}

BigNumber& BigNumber::operator=(const BigNumber& other) {
    if (this != &other) {
        clear();
        isNegative = other.isNegative;
        decimalPos = other.decimalPos;
        Node* current = other.head;
        while (current) {
            Node* newNode = new Node(current->digit);
            newNode->next = head;
            head = newNode;
            current = current->next;
        }
        reverse();
    }
    return *this;
}

int BigNumber::compare(const BigNumber& other) const {
    int len1 = digitCount(), len2 = other.digitCount();
    if (len1 != len2) return len1 > len2 ? 1 : -1;
    Node* n1 = head;
    Node* n2 = other.head;
    while (n1 && n2) {
        if (n1->digit != n2->digit) return n1->digit > n2->digit ? 1 : -1;
        n1 = n1->next;
        n2 = n2->next;
    }
    return 0;
}

bool BigNumber::isZero() const {
    return head && head->digit == 0 && !head->next;
}

void BigNumber::normalizeDecimal(BigNumber& other) const {
    int maxDecimal = std::max(decimalPos, other.decimalPos);
    while (decimalPos < maxDecimal) {
        BigNumber& self = const_cast<BigNumber&>(*this); // Temporarily cast away const to modify self
        Node* zero = new Node(0);
        zero->next = self.head;
        self.head = zero;
        self.decimalPos++;
    }
    while (other.decimalPos < maxDecimal) {
        Node* zero = new Node(0);
        zero->next = other.head;
        other.head = zero;
        other.decimalPos++;
    }
}

BigNumber BigNumber::operator+(const BigNumber& other) const {
    BigNumber result, copyOther = other;
    result.decimalPos = std::max(decimalPos, other.decimalPos);
    normalizeDecimal(copyOther);

    if (isNegative == copyOther.isNegative) {
        Node* n1 = head;
        Node* n2 = copyOther.head;
        int carry = 0;
        Node* tail = nullptr;
        while (n1 || n2 || carry) {
            int sum = carry;
            if (n1) {
                sum += n1->digit;
                n1 = n1->next;
            }
            if (n2) {
                sum += n2->digit;
                n2 = n2->next;
            }
            carry = sum / 10;
            Node* newNode = new Node(sum % 10);
            if (!result.head) {
                result.head = tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        result.isNegative = isNegative;
    }
    else {
        copyOther.isNegative = isNegative;
        result = isNegative ? copyOther - *this : *this - copyOther;
    }
    result.removeLeadingZeros();
    return result;
}

BigNumber BigNumber::operator-(const BigNumber& other) const {
    BigNumber result, copyOther = other;
    result.decimalPos = std::max(decimalPos, other.decimalPos);
    normalizeDecimal(copyOther);

    if (isNegative != copyOther.isNegative) {
        copyOther.isNegative = isNegative;
        return *this + copyOther;
    }

    bool resultNegative = false;
    const BigNumber* larger = this;
    const BigNumber* smaller = &copyOther;
    if (compare(copyOther) < 0) {
        larger = &copyOther;
        smaller = this;
        resultNegative = !isNegative;
    }

    Node* n1 = larger->head;
    Node* n2 = smaller->head;
    int borrow = 0;
    Node* tail = nullptr;
    while (n1) {
        int diff = n1->digit - borrow;
        if (n2) {
            diff -= n2->digit;
            n2 = n2->next;
        }
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        Node* newNode = new Node(diff);
        if (!result.head) {
            result.head = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
        n1 = n1->next;
    }
    result.isNegative = resultNegative;
    result.removeLeadingZeros();
    return result;
}

BigNumber BigNumber::operator*(const BigNumber& other) const {
    if (digitCount() > 50 || other.digitCount() > 50) {
        return karatsubaMultiply(*this, other);
    }

    BigNumber result;
    result.isNegative = isNegative != other.isNegative;
    result.decimalPos = decimalPos + other.decimalPos;

    if (isZero() || other.isZero()) return result;

    Node* n2 = other.head;
    int position = 0;
    while (n2) {
        BigNumber partial;
        Node* n1 = head;
        int carry = 0;
        Node* tail = nullptr;
        while (n1 || carry) {
            int product = carry;
            if (n1) {
                product += n1->digit * n2->digit;
                n1 = n1->next;
            }
            carry = product / 10;
            Node* newNode = new Node(product % 10);
            if (!partial.head) {
                partial.head = tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        for (int i = 0; i < position; i++) {
            Node* zero = new Node(0);
            zero->next = partial.head;
            partial.head = zero;
        }
        result = result + partial;
        n2 = n2->next;
        position++;
    }
    result.removeLeadingZeros();
    return result;
}

BigNumber BigNumber::karatsubaMultiply(const BigNumber& a, const BigNumber& b) {
    BigNumber result;
    result.isNegative = a.isNegative != b.isNegative;
    result.decimalPos = a.decimalPos + b.decimalPos;

    int len = std::max(a.digitCount(), b.digitCount());
    if (len <= 50) return a * b; // Fallback to traditional for small numbers

    int k = len / 2;
    BigNumber aHigh, aLow, bHigh, bLow;
    Node* current = a.head;
    int count = 0;
    while (current && count < k) {
        Node* newNode = new Node(current->digit);
        newNode->next = aLow.head;
        aLow.head = newNode;
        current = current->next;
        count++;
    }
    while (current) {
        Node* newNode = new Node(current->digit);
        newNode->next = aHigh.head;
        aHigh.head = newNode;
        current = current->next;
    }
    aLow.reverse();
    aHigh.reverse();

    current = b.head;
    count = 0;
    while (current && count < k) {
        Node* newNode = new Node(current->digit);
        newNode->next = bLow.head;
        bLow.head = newNode;
        current = current->next;
        count++;
    }
    while (current) {
        Node* newNode = new Node(current->digit);
        newNode->next = bHigh.head;
        bHigh.head = newNode;
        current = current->next;
    }
    bLow.reverse();
    bHigh.reverse();

    BigNumber p1 = karatsubaMultiply(aHigh, bHigh);
    BigNumber p2 = karatsubaMultiply(aLow, bLow);
    BigNumber p3 = karatsubaMultiply(aHigh + aLow, bHigh + bLow);
    BigNumber mid = p3 - p1 - p2;

    for (int i = 0; i < 2 * k; i++) {
        Node* zero = new Node(0);
        zero->next = p1.head;
        p1.head = zero;
    }
    for (int i = 0; i < k; i++) {
        Node* zero = new Node(0);
        zero->next = mid.head;
        mid.head = zero;
    }
    result = p1 + mid + p2;
    result.removeLeadingZeros();
    return result;
}

BigNumber BigNumber::operator/(const BigNumber& other) const {
    if (other.isZero()) throw std::runtime_error("Division by zero");

    BigNumber quotient;
    quotient.isNegative = isNegative != other.isNegative;
    quotient.decimalPos = decimalPos - other.decimalPos;

    BigNumber dividend = *this;
    dividend.isNegative = false;
    BigNumber divisor = other;
    divisor.isNegative = false;

    if (dividend.compare(divisor) < 0) return quotient;

    BigNumber current;
    Node* n = dividend.head;
    while (n) {
        current = current * BigNumber("10") + BigNumber(std::to_string(n->digit));
        int q = 0;
        while (current.compare(divisor) >= 0) {
            current = current - divisor;
            q++;
        }
        Node* newNode = new Node(q);
        newNode->next = quotient.head;
        quotient.head = newNode;
        n = n->next;
    }
    quotient.reverse();
    quotient.removeLeadingZeros();
    return quotient;
}

BigNumber BigNumber::operator%(const BigNumber& other) const {
    BigNumber quotient = *this / other;
    BigNumber product = quotient * other;
    return *this - product;
}

BigNumber BigNumber::power(int n) const {
    if (n < 0) throw std::runtime_error("Negative exponent not supported");
    BigNumber result("1");
    BigNumber base = *this;
    while (n > 0) {
        if (n % 2 == 1) result = result * base;
        base = base * base;
        n /= 2;
    }
    return result;
}

std::string BigNumber::toString() const {
    if (isZero()) return "0";
    std::string result = isNegative ? "-" : "";
    std::string digits;
    Node* current = head;
    int pos = digitCount() - decimalPos;
    int i = 0;
    while (current) {
        if (decimalPos > 0 && i == pos && pos != 0) digits += ".";
        digits += std::to_string(current->digit);
        current = current->next;
        i++;
    }
    std::reverse(digits.begin(), digits.end());
    return result + digits;
}