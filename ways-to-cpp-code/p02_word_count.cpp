// ============================================================
//  Ways to cpp · 项目 02  英文词频统计器
//  编译： g++ -std=c++14 -Wall p02_word_count.cpp -o p02
//  运行： ./p02
//  说明： 第一次运行会在当前目录生成 sample.txt，然后统计它。
// ============================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

// 词 -> 次数。map 的下标访问会自动建表，天生适合数数。
typedef std::map<std::string, int> Counter;
typedef std::pair<std::string, int> WordCount;

// ---------- 一、把一坨文本切成一个个小写单词 ----------
std::vector<std::string> splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::string current;
    for (char raw : text) {
        const unsigned char ch = static_cast<unsigned char>(raw);
        if (std::isalpha(ch)) {
            current += static_cast<char>(std::tolower(ch));  // 统一成小写
        } else if (!current.empty()) {
            words.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) words.push_back(current);   // 别忘了最后一个
    return words;
}

// ---------- 二、数数本身就是一行 ----------
Counter countWords(const std::vector<std::string>& words) {
    Counter counter;
    for (const std::string& w : words) {
        ++counter[w];              // 第一次见到它是 0 -> 1，之后一直加
    }
    return counter;
}

// ---------- 三、排序规则：次数多的在前，同次数按字典序，保证输出稳定 ----------
bool moreFrequentFirst(const WordCount& a, const WordCount& b) {
    if (a.second != b.second) return a.second > b.second;
    return a.first < b.first;
}

// ---------- 四、文件读写：先把素材落盘，再读回来 ----------
void saveSampleIfMissing(const std::string& path) {
    std::ifstream probe(path);
    if (probe.good()) return;          // 已经存在就不覆盖，免得改坏你的文件

    std::ofstream fout(path);
    fout << "It was the best of times, it was the worst of times.\n"
            "It was the age of wisdom, it was the age of foolishness.\n"
            "It was the season of light, it was the season of darkness.\n"
            "We had everything before us, we had nothing before us.\n";
}

std::string readAll(const std::string& path) {
    std::ifstream fin(path);
    if (!fin) return std::string();    // 空串表示读失败

    std::string all, line;
    while (std::getline(fin, line)) {  // 一行一行读，不会吞掉换行
        all += line;
        all += ' ';
    }
    return all;
}

int main() {
    const std::string path = "sample.txt";
    saveSampleIfMissing(path);

    const std::string text = readAll(path);
    if (text.empty()) {
        std::cerr << "Cannot read " << path << "\n";
        return 1;
    }

    const std::vector<std::string> words = splitWords(text);
    const Counter counter = countWords(words);

    // map 天生按 key 有序，但我们要按"次数"排，于是搬进 vector 再排
    std::vector<WordCount> ranked(counter.begin(), counter.end());
    std::sort(ranked.begin(), ranked.end(), moreFrequentFirst);

    std::cout << "File      : " << path << "\n";
    std::cout << "Words     : " << words.size() << "\n";
    std::cout << "Distinct  : " << counter.size() << "\n\n";
    std::cout << "----- Top 10 -----\n";

    const int top = (ranked.size() < 10) ? static_cast<int>(ranked.size()) : 10;
    for (int i = 0; i < top; ++i) {
        std::cout << std::left  << std::setw(14) << ranked[i].first
                  << std::right << std::setw(3)  << ranked[i].second << "  "
                  << std::string(ranked[i].second, '#') << "\n";
    }
    return 0;
}
