// ============================================================
//  Ways to cpp · 项目 05  表达式求值器（递归下降）
//  编译： g++ -std=c++14 -Wall p05_calculator.cpp -o p05
//  运行： ./p05
//  支持： + - * / ( ) 与小数
// ============================================================

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

// 三个函数互相调用，所以先在这里"报个到"（函数声明）
double parseExpression(const std::string& s, size_t& pos);
double parseTerm(const std::string& s, size_t& pos);
double parseFactor(const std::string& s, size_t& pos);

void skipSpaces(const std::string& s, size_t& pos) {
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
}

// 最外层：只认加号减号。位置 pos 用引用传递，于是三个函数共享同一个"光标"。
double parseExpression(const std::string& s, size_t& pos) {
    double value = parseTerm(s, pos);
    while (true) {
        skipSpaces(s, pos);
        if (pos < s.size() && (s[pos] == '+' || s[pos] == '-')) {
            const char op = s[pos];
            ++pos;                                    // 吃掉这个运算符
            const double rhs = parseTerm(s, pos);
            value = (op == '+') ? value + rhs : value - rhs;
        } else {
            return value;
        }
    }
}

// 中间层：只认乘号除号。它的优先级天生比加减高，因为它被更里层调用。
double parseTerm(const std::string& s, size_t& pos) {
    double value = parseFactor(s, pos);
    while (true) {
        skipSpaces(s, pos);
        if (pos < s.size() && (s[pos] == '*' || s[pos] == '/')) {
            const char op = s[pos];
            ++pos;
            const double rhs = parseFactor(s, pos);
            value = (op == '*') ? value * rhs : value / rhs;
        } else {
            return value;
        }
    }
}

// 最里层：数字、括号、负号
double parseFactor(const std::string& s, size_t& pos) {
    skipSpaces(s, pos);

    if (pos < s.size() && s[pos] == '(') {
        ++pos;                                        // 跳过 '('
        const double value = parseExpression(s, pos); // 括号里重新从最外层开始
        skipSpaces(s, pos);
        if (pos < s.size() && s[pos] == ')') ++pos;   // 跳过 ')'
        return value;
    }

    if (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) {
        const bool negative = (s[pos] == '-');
        ++pos;
        const double value = parseFactor(s, pos);     // 允许 --3、-(1+2) 这种写法
        return negative ? -value : value;
    }

    // 读一个数字：整数部分.小数部分
    size_t begin = pos;
    while (pos < s.size() && (std::isdigit(static_cast<unsigned char>(s[pos])) || s[pos] == '.')) {
        ++pos;
    }
    if (begin == pos) return 0.0;                     // 没读到数字，交给上层收尾
    return std::atof(s.substr(begin, pos - begin).c_str());
}

double evaluate(const std::string& expression) {
    size_t pos = 0;
    return parseExpression(expression, pos);
}

int main() {
    const std::string tests[] = {
        "1 + 2 * 3",
        "(1 + 2) * 3",
        "2 * (3 + 4) - 5",
        "10 / 4",
        "-3 + 10",
        "1 + 2 * (3 + 4 * (5 - 2))",
        "((2 + 3) * (7 - 4)) / 3"
    };

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "----- Expression Evaluator -----\n";
    for (const std::string& t : tests) {
        std::cout << std::left << std::setw(28) << t
                  << " = " << evaluate(t) << "\n";
    }
    return 0;
}
