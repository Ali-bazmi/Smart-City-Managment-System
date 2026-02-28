#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <cfloat>

using namespace std;

// String to int 
int stringToInt(const string& str) {
    if (str.empty()) return 0;
    int res = 0;
    int sign = 1;
    long long i = 0;

    while (i < str.length() && (str[i] == ' ' || str[i] == '\t')) i++;

    if (i < str.length() && str[i] == '-') {
        sign = -1;
        i++;
    }
    else if (i < str.length() && str[i] == '+') {
        i++;
    }

    for (; i < str.length(); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
        }
        else {
            break;
        }
    }
    return res * sign;
}

// Converts String to double 
double stringToDouble(const string& str) {
    if (str.empty()) return 0.0;
    double res = 0.0;
    double factor = 1.0;
    bool decimalFound = false;
    int sign = 1;
    long long i = 0;

    while (i < str.length() && (str[i] == ' ' || str[i] == '\t')) i++;

    if (i < str.length() && str[i] == '-') {
        sign = -1;
        i++;
    }
    else if (i < str.length() && str[i] == '+') {
        i++;
    }

    for (; i < str.length(); i++) {
        if (str[i] == '.') {
            if (decimalFound) break;
            decimalFound = true;
            continue;
        }
        if (str[i] >= '0' && str[i] <= '9') {
            if (decimalFound) {
                factor /= 10.0;
                res += (str[i] - '0') * factor;
            }
            else {
                res = res * 10.0 + (str[i] - '0');
            }
        }
        else {
            break;
        }
    }
    return res * sign;
}

// Converts int to string 
string intToString(int num) {
    if (num == 0) return "0";
    string result = "";
    bool negative = false;
    if (num < 0) {
        negative = true;
        num = -num;
    }
    while (num > 0) {
        result = char('0' + num % 10) + result;
        num /= 10;
    }
    if (negative) result = "-" + result;
    return result;
}

// STRING HELPERS

int splitString(const string& str, char delimiter, string outputArray[], int maxTokens) {
    int count = 0;
    string currentToken = "";

    for (long long i = 0; i < str.length(); i++) {
        if (str[i] == delimiter) {
            if (count < maxTokens) {
                // Trim whitespace
                while (!currentToken.empty() && currentToken[0] == ' ')
                    currentToken = currentToken.substr(1);
                while (!currentToken.empty() && currentToken[currentToken.length() - 1] == ' ')
                    currentToken = currentToken.substr(0, currentToken.length() - 1);
                outputArray[count++] = currentToken;
                currentToken = "";
            }
        }
        else {
            currentToken += str[i];
        }
    }

    if (count < maxTokens && !currentToken.empty()) {
        // Remove trailing \r for Windows files
        while (!currentToken.empty() && (currentToken.back() == '\r' || currentToken.back() == '\n'))
            currentToken = currentToken.substr(0, currentToken.length() - 1);
        // Trim whitespace
        while (!currentToken.empty() && currentToken[0] == ' ')
            currentToken = currentToken.substr(1);
        while (!currentToken.empty() && currentToken[currentToken.length() - 1] == ' ')
            currentToken = currentToken.substr(0, currentToken.length() - 1);
        outputArray[count++] = currentToken;
    }
    return count;
}

// Case Insensitive 
string toLowerStr(const string& s) {
    string result = "";
    for (long long i = 0; i < s.length(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z')
            result += (s[i] + 32);
        else
            result += s[i];
    }
    return result;
}

string trimString(const string& s) {
    string result = s;
    while (!result.empty() && (result[0] == ' ' || result[0] == '\t' || result[0] == '\r' || result[0] == '\n'))
        result = result.substr(1);
    while (!result.empty() && (result[result.length() - 1] == ' ' || result[result.length() - 1] == '\t' ||
        result[result.length() - 1] == '\r' || result[result.length() - 1] == '\n'))
        result = result.substr(0, result.length() - 1);
    return result;
}

bool stringsEqualIgnoreCase(const string& a, const string& b) {
    if (a.length() != b.length()) return false;
    for (long long i = 0; i < a.length(); i++) {
        char c1 = a[i];
        char c2 = b[i];
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        if (c1 != c2) return false;
    }
    return true;
}

// HASH FUNCTIONS

// Polynomial Rolling Hash
unsigned int polynomialHash(const string& key, int tableSize) {
    unsigned long long hash = 0;
    unsigned long long p = 31;
    unsigned long long p_pow = 1;
    string cleanKey = toLowerStr(key);

    for (long long i = 0; i < cleanKey.length(); i++) {
        char c = cleanKey[i];
        if (c >= 'a' && c <= 'z') {
            hash = (hash + (c - 'a' + 1) * p_pow) % tableSize;
            p_pow = (p_pow * p) % tableSize;
        }
        else if (c >= '0' && c <= '9') {
            hash = (hash + (c - '0' + 27) * p_pow) % tableSize;
            p_pow = (p_pow * p) % tableSize;
        }
    }
    return (unsigned int)(hash % tableSize);
}

// Simple sum hash for CNICs
unsigned int sumHash(const string& key, int tableSize) {
    unsigned int sum = 0;
    for (long long i = 0; i < key.length(); i++) {
        sum += (unsigned char)key[i];
    }
    return sum % tableSize;
}

// CNIC VALIDATION

bool isValidCNICFormat(const string& cnic) {
    if (cnic.length() != 15) return false;
    if (cnic[5] != '-' || cnic[13] != '-') return false;

    for (int i = 0; i < 15; i++) {
        if (i == 5 || i == 13) continue;
        if (cnic[i] < '0' || cnic[i] > '9') return false;
    }
    return true;
}

string getGenderFromCNIC(const string& cnic) {
    if (!isValidCNICFormat(cnic)) return "Unknown";
    int lastDigit = cnic[14] - '0';
    return (lastDigit % 2 == 0) ? "Female" : "Male";
}

// COORDINATE & DISTANCE HELPERS

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    return sqrt(pow(lat2 - lat1, 2) + pow(lon2 - lon1, 2));
}

bool coordinatesMatch(double lat1, double lon1, double lat2, double lon2, double epsilon = 0.0001) {
    return (abs(lat1 - lat2) < epsilon && abs(lon1 - lon2) < epsilon);
}

// INPUT HELPERS


void clearInputBuffer() {
    cin.clear();
    while (cin.peek() != '\n' && cin.peek() != EOF) {
        cin.get();
    }
    if (cin.peek() == '\n') cin.get();
}

int getIntInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return stringToInt(input);
}

double getDoubleInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return stringToDouble(input);
}

string getStringInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return trimString(input);
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue... ";
    string dummy;
    getline(cin, dummy);
}
