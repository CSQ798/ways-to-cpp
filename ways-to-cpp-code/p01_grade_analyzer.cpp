// ============================================================
//  Ways to cpp · 项目 01  学生成绩分析器
//  编译： g++ -std=c++14 -Wall p01_grade_analyzer.cpp -o p01
//  运行： ./p01        (Windows 下是 p01.exe)
// ============================================================

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// ---------- 1. 先描述"数据长什么样" ----------
// 结构体只负责装数据，不负责做事，这是最省心的分工。
struct Student {
    std::string name;
    int         score;
};

// ---------- 2. 再做"数据上的动作"，一个函数只做一件事 ----------

// const 引用：既不拷贝（快），又向读代码的人承诺"我不会改你"
double averageOf(const std::vector<Student>& students) {
    if (students.empty()) return 0.0;
    double sum = 0.0;
    for (const Student& s : students) {   // 范围 for：不用再关心下标
        sum += s.score;
    }
    return sum / static_cast<double>(students.size());
}

// 返回最高分学生的下标；调用者保证容器非空
int indexOfBest(const std::vector<Student>& students) {
    int best = 0;
    for (int i = 1; i < static_cast<int>(students.size()); ++i) {
        if (students[i].score > students[best].score) best = i;
    }
    return best;
}

// 标准差：衡量"分数有多散"。这就是算法，只不过只有六行。
double deviationOf(const std::vector<Student>& students) {
    if (students.size() < 2) return 0.0;
    const double mean = averageOf(students);
    double acc = 0.0;
    for (const Student& s : students) {
        const double d = s.score - mean;
        acc += d * d;
    }
    return std::sqrt(acc / static_cast<double>(students.size()));
}

// 排序规则单独写成函数，名字就是说它的那句话，比写在调用处好读得多
bool higherScoreFirst(const Student& a, const Student& b) {
    return a.score > b.score;
}

// 一条 if-else 链，其实就是一张查表
std::string gradeOf(int score) {
    if (score >= 90) return "A";
    if (score >= 80) return "B";
    if (score >= 70) return "C";
    if (score >= 60) return "D";
    return "F";
}

void printTable(const std::vector<Student>& students) {
    std::cout << "----- Ranked -----\n";
    std::cout << std::left  << std::setw(4)  << "#"
              << std::left  << std::setw(10) << "Name"
              << std::right << std::setw(6)  << "Score"
              << std::right << std::setw(7)  << "Grade" << "\n";
    for (int i = 0; i < static_cast<int>(students.size()); ++i) {
        const Student& s = students[i];
        std::cout << std::left  << std::setw(4)  << (i + 1)
                  << std::left  << std::setw(10) << s.name
                  << std::right << std::setw(6)  << s.score
                  << std::right << std::setw(7)  << gradeOf(s.score) << "\n";
    }
}

int main() {
    // 演示数据。真实使用时，把这一段换成 cin 读入即可，
    // 上面所有函数一行都不用改 —— 这就是"把数据和逻辑分开"的回报。
    std::vector<Student> students = {
        {"Alice",  92}, {"Bob",    67}, {"Cathy",  85},
        {"David",  58}, {"Eve",   100}, {"Frank",  74},
        {"Grace",  81}, {"Henry",  46}
    };

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Students : " << students.size() << "\n";
    std::cout << "Average  : " << averageOf(students) << "\n";
    std::cout << "StdDev   : " << deviationOf(students) << "\n";
    std::cout << "Best     : " << students[indexOfBest(students)].name
              << " (" << students[indexOfBest(students)].score << ")\n\n";

    std::sort(students.begin(), students.end(), higherScoreFirst);
    printTable(students);
    return 0;
}
