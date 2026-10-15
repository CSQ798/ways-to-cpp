// ============================================================
//  Ways to cpp · 项目 07  一个像数学一样写字的矩阵类
//  编译： g++ -std=c++14 -Wall p07_matrix.cpp -o p07
//  运行： ./p07
// ============================================================

#include <iomanip>
#include <iostream>
#include <vector>

// 类的意义：把"数据"和"操作这些数据的函数"捆在一起，
// 让外面的人只看到 C = A * B，而不是一堆循环。
class Matrix {
public:
    // 构造函数：对象一诞生就是合法可用的（全部填 0）
    Matrix(int rows, int cols)
        : rows_(rows), cols_(cols), data_(static_cast<size_t>(rows) * cols, 0.0) {}

    static Matrix identity(int n) {
        Matrix m(n, n);
        for (int i = 0; i < n; ++i) m.at(i, i) = 1.0;
        return m;
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    // 两个版本：非 const 版本可读可写，const 版本只读。
    // 编译器靠对象本身是否 const 来挑，我们什么都不用管。
    double&       at(int r, int c)       { return data_[static_cast<size_t>(r) * cols_ + c]; }
    double        at(int r, int c) const { return data_[static_cast<size_t>(r) * cols_ + c]; }

    Matrix operator+(const Matrix& other) const {
        Matrix result(rows_, cols_);
        for (int r = 0; r < rows_; ++r)
            for (int c = 0; c < cols_; ++c)
                result.at(r, c) = at(r, c) + other.at(r, c);
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        Matrix result(rows_, other.cols_);
        for (int r = 0; r < rows_; ++r) {
            for (int k = 0; k < cols_; ++k) {
                const double a = at(r, k);
                for (int c = 0; c < other.cols_; ++c) {
                    result.at(r, c) += a * other.at(k, c);
                }
            }
        }
        return result;
    }

    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (int r = 0; r < rows_; ++r)
            for (int c = 0; c < cols_; ++c)
                result.at(c, r) = at(r, c);
        return result;
    }

private:
    int rows_;
    int cols_;
    std::vector<double> data_;   // 用一维数组存放，行主序
};

// 重载 << 之后，cout << A 就能像打印一个数字那样打印一个矩阵
std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    for (int r = 0; r < m.rows(); ++r) {
        os << "[ ";
        for (int c = 0; c < m.cols(); ++c) {
            os << std::setw(8) << m.at(r, c) << " ";
        }
        os << "]\n";
    }
    return os;
}

int main() {
    Matrix a(3, 3);
    Matrix b(3, 3);
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            a.at(r, c) = r * 3 + c + 1;          // 1..9
            b.at(r, c) = (r == c) ? 2.0 : 0.0;   // 2 倍单位阵
        }
    }

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "A =\n" << a;
    std::cout << "A + A =\n" << (a + a);
    std::cout << "A * B  (B = 2I) =\n" << (a * b);
    std::cout << "A transposed =\n" << a.transpose();
    std::cout << "A * A^T  (对称矩阵，神经网络里的协方差就是这个形状) =\n"
              << (a * a.transpose());
    return 0;
}
