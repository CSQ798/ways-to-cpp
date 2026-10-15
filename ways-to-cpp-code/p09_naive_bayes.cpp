// ============================================================
//  Ways to cpp · 项目 09  朴素贝叶斯垃圾信息分类器
//  编译： g++ -std=c++14 -Wall p09_naive_bayes.cpp -o p09
//  运行： ./p09
// ============================================================

#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// 一个词在两边的"案底"
struct WordStats {
    int spam;
    int ham;
    WordStats() : spam(0), ham(0) {}
    int total() const { return spam + ham; }
};

typedef std::map<std::string, WordStats> Table;

// 把一句话切成小写单词（和项目 02 是同一件事，直接拿来用）
std::vector<std::string> splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::string current;
    for (char raw : text) {
        const unsigned char ch = static_cast<unsigned char>(raw);
        if (std::isalpha(ch)) {
            current += static_cast<char>(std::tolower(ch));
        } else if (!current.empty()) {
            words.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) words.push_back(current);
    return words;
}

// ---------- 分类器 ----------
class NaiveBayes {
public:
    NaiveBayes() : spamDocs_(0), hamDocs_(0), spamWords_(0), hamWords_(0) {}

    void train(const std::string& text, bool isSpam) {
        const std::vector<std::string> words = splitWords(text);
        if (isSpam) ++spamDocs_; else ++hamDocs_;

        for (const std::string& w : words) {
            WordStats& s = table_[w];          // 没见过的词会自动建一条空记录
            if (isSpam) { ++s.spam; ++spamWords_; }
            else        { ++s.ham;  ++hamWords_;  }
        }
    }

    // 返回对数概率：log P(垃圾) + Σ log P(词 | 垃圾)
    double spamScore(const std::string& text) const {
        const double prior = std::log(static_cast<double>(spamDocs_) /
                                      (spamDocs_ + hamDocs_));
        return prior + logLikelihood(text, true);
    }

    double hamScore(const std::string& text) const {
        const double prior = std::log(static_cast<double>(hamDocs_) /
                                      (spamDocs_ + hamDocs_));
        return prior + logLikelihood(text, false);
    }

    // 预测：哪边的分数大就归哪边
    bool isSpam(const std::string& text) const {
        return spamScore(text) > hamScore(text);
    }

    size_t vocabularySize() const { return table_.size(); }

private:
    // 拉普拉斯平滑：分子 +1，分母 + 词表大小，
    // 这样"从没见过的词"也不会把概率算成 0（一旦是 0，取对数就炸了）
    double logLikelihood(const std::string& text, bool spam) const {
        const std::vector<std::string> words = splitWords(text);
        const double vocab = static_cast<double>(table_.size());
        const double denom = (spam ? spamWords_ : hamWords_) + vocab;
        double sum = 0.0;

        for (const std::string& w : words) {
            const Table::const_iterator it = table_.find(w);
            const int count = (it == table_.end()) ? 0 : (spam ? it->second.spam
                                                               : it->second.ham);
            sum += std::log((count + 1.0) / denom);
        }
        return sum;
    }

    Table table_;
    int spamDocs_;
    int hamDocs_;
    int spamWords_;
    int hamWords_;
};

int main() {
    // ---------- 训练集：12 条短消息 ----------
    struct Row { const char* text; bool spam; };
    const Row rows[] = {
        {"win a free iphone now click the link",              true},
        {"cheap loan approved instantly no credit check",     true},
        {"congratulations you won a lottery prize claim",     true},
        {"buy discount pills online free shipping now",       true},
        {"urgent your account will be closed click here",     true},
        {"free entry to win cash prize limited offer",        true},
        {"meeting moved to three pm in room b",               false},
        {"please review the attached report before friday",   false},
        {"lunch today at the campus canteen",                 false},
        {"the library book is due next week",                 false},
        {"can you send me the notes from lecture",            false},
        {"the exam schedule is posted on the website",        false}
    };

    NaiveBayes model;
    for (int i = 0; i < 12; ++i) {
        model.train(rows[i].text, rows[i].spam);
    }
    std::cout << "Trained on 12 messages, vocabulary = "
              << model.vocabularySize() << " words\n\n";

    // ---------- 测试 ----------
    const std::string tests[] = {
        "free cash prize click now",
        "can we meet after the lecture",
        "your loan is approved with no credit check",
        "the report is attached for review"
    };

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "----- Predictions -----\n";
    for (const std::string& t : tests) {
        const double s = model.spamScore(t);
        const double h = model.hamScore(t);
        std::cout << std::left << std::setw(44) << t
                  << " spam=" << std::setw(8) << s
                  << " ham="  << std::setw(8) << h
                  << "  ->  " << (model.isSpam(t) ? "SPAM" : "ham") << "\n";
    }
    return 0;
}
