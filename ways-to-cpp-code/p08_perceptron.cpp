// ============================================================
//  Ways to cpp · 项目 08  训练你的第一个模型：感知机
//  编译： g++ -std=c++14 -Wall p08_perceptron.cpp -o p08
//  运行： ./p08
//  说明： 训练完会把参数存进 perceptron.txt，再读回来验证一次。
// ============================================================

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

struct Sample {
    double x1;
    double x2;
    int    label;      // +1 或 -1
};

// ---------- 模型本身：三个数字就是它的全部 ----------
class Perceptron {
public:
    Perceptron(int epochs, double learningRate)
        : w1_(0.0), w2_(0.0), b_(0.0),
          epochs_(epochs), learningRate_(learningRate) {}

    // 一次前向计算：把点投影到一个方向上，看它落在哪一边
    int predict(double x1, double x2) const {
        return (w1_ * x1 + w2_ * x2 + b_ >= 0.0) ? 1 : -1;
    }

    // 训练：错一次，就朝着"少错一点"的方向挪一小步
    void train(const std::vector<Sample>& data) {
        for (int epoch = 1; epoch <= epochs_; ++epoch) {
            int mistakes = 0;
            for (const Sample& s : data) {
                const int guess = predict(s.x1, s.x2);
                if (guess == s.label) continue;

                ++mistakes;
                w1_ += learningRate_ * s.label * s.x1;
                w2_ += learningRate_ * s.label * s.x2;
                b_  += learningRate_ * s.label;
            }
            if (epoch <= 5 || mistakes == 0) {
                std::cout << "  epoch " << std::setw(3) << epoch
                          << "   mistakes = " << mistakes << "\n";
            }
            if (mistakes == 0) break;      // 全对了，提前收工
        }
    }

    double accuracy(const std::vector<Sample>& data) const {
        if (data.empty()) return 0.0;
        int correct = 0;
        for (const Sample& s : data) {
            if (predict(s.x1, s.x2) == s.label) ++correct;
        }
        return 100.0 * correct / static_cast<double>(data.size());
    }

    void print() const {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "  w1 = " << w1_ << "   w2 = " << w2_ << "   b = " << b_ << "\n";
        std::cout << std::setprecision(4)
                  << "  boundary: " << w1_ << "*x1 "
                  << (w2_ >= 0.0 ? "+ " : "- ") << std::fabs(w2_) << "*x2 "
                  << (b_  >= 0.0 ? "+ " : "- ") << std::fabs(b_)  << " = 0\n"
                  << std::setprecision(3);
    }

    void save(const std::string& path) const {
        std::ofstream fout(path);
        fout << std::setprecision(17) << w1_ << " " << w2_ << " " << b_ << "\n";
    }

    bool load(const std::string& path) {
        std::ifstream fin(path);
        if (!fin) return false;
        fin >> w1_ >> w2_ >> b_;
        return true;
    }

private:
    double w1_, w2_, b_;
    int    epochs_;
    double learningRate_;
};

int main() {
    // ---------- 1. 造一批线性可分的数据：两团点 ----------
    std::mt19937 engine(20180101);                      // 固定种子 -> 每次运行结果一致
    std::normal_distribution<double> spread(0.0, 0.8);

    std::vector<Sample> trainSet;
    for (int i = 0; i < 40; ++i) {
        Sample s;
        s.x1 = 2.0 + spread(engine);
        s.x2 = 2.0 + spread(engine);
        s.label = 1;
        trainSet.push_back(s);
    }
    for (int i = 0; i < 40; ++i) {
        Sample s;
        s.x1 = -2.0 + spread(engine);
        s.x2 = -2.0 + spread(engine);
        s.label = -1;
        trainSet.push_back(s);
    }

    std::cout << "----- Training -----\n";
    std::cout << "  samples = " << trainSet.size() << "\n";

    Perceptron model(200, 0.01);
    model.train(trainSet);

    std::cout << "\n----- Learned parameters -----\n";
    model.print();
    std::cout << "  Training accuracy = " << model.accuracy(trainSet) << "%\n";

    // ---------- 2. 存盘，再读回来，确认模型能"隔天继续用" ----------
    const std::string path = "perceptron.txt";
    model.save(path);
    std::cout << "\n----- Persistence -----\n";
    std::cout << "  saved to " << path << "\n";

    Perceptron reloaded(200, 0.01);
    if (reloaded.load(path)) {
        std::cout << "  reloaded accuracy = " << reloaded.accuracy(trainSet) << "%\n";
    }

    // ---------- 3. 拿几个新点试试 ----------
    std::cout << "\n----- Predictions -----\n";
    const double probes[][2] = {{3.0, 3.0}, {-3.0, -3.0}, {3.0, -0.5}, {-0.5, -3.0}};
    for (int i = 0; i < 4; ++i) {
        std::cout << "  (" << std::setw(5) << probes[i][0] << ","
                  << std::setw(5) << probes[i][1] << ")"
                  << "  ->  label " << reloaded.predict(probes[i][0], probes[i][1]) << "\n";
    }
    return 0;
}
