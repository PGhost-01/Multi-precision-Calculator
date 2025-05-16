#ifndef BIGNUMBER_H
#define BIGNUMBER_H

#include <string>

struct Node {
    int digit;
    Node* next;
    Node(int d) : digit(d), next(nullptr) {}
};

class BigNumber {
private:
    Node* head; // Least significant digit at head
    bool isNegative; // Sign flag
    int decimalPos; // Number of digits after decimal point
    void clear(); // Free memory
    void reverse(); // Reverse linked list
    void removeLeadingZeros(); // Remove leading zeros
    static BigNumber karatsubaMultiply(const BigNumber& a, const BigNumber& b); // Karatsuba helper
    int compare(const BigNumber& other) const; // Compare magnitudes
    int digitCount() const; // Count digits
    void normalizeDecimal(BigNumber& other) const; // Align decimal points - Updated to const

public:
    BigNumber(); // Default constructor
    BigNumber(const std::string& num); // Constructor from string
    BigNumber(const BigNumber& other); // Copy constructor
    ~BigNumber(); // Destructor
    BigNumber& operator=(const BigNumber& other); // Assignment operator

    BigNumber operator+(const BigNumber& other) const; // Addition
    BigNumber operator-(const BigNumber& other) const; // Subtraction
    BigNumber operator*(const BigNumber& other) const; // Multiplication (traditional or Karatsuba)
    BigNumber operator/(const BigNumber& other) const; // Division (quotient)
    BigNumber operator%(const BigNumber& other) const; // Modulus (remainder)
    BigNumber power(int n) const; // Power function

    std::string toString() const; // Convert to string with decimal
    bool isZero() const; // Check if zero
    int getDecimalPos() const { return decimalPos; } // Get decimal position
};

#endif