// ============================================================
//  Ways to cpp · 项目 10  曼德博集合：把数学画成一张图
//  编译： g++ -std=c++14 -Wall p10_mandelbrot.cpp -o p10
//  运行： ./p10
//  产物： mandelbrot.bmp（双击就能看）
// ============================================================

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const int WIDTH  = 480;
const int HEIGHT = 320;
const int MAX_ITER = 200;

// 复数 z 的平方加 c。C++ 标准库里有 <complex>，
// 但这里手写四行，反而更清楚每一次乘法去了哪里。
void squareAndAdd(double& zr, double& zi, double cr, double ci) {
    const double newR = zr * zr - zi * zi + cr;
    const double newI = 2.0 * zr * zi + ci;
    zr = newR;
    zi = newI;
}

// 点 (cr, ci) 逃逸得越慢，说明它越"靠近"集合，颜色越黑。
// 返回值 0..255，0 表示被判定为在集合内。
int escapeTime(double cr, double ci) {
    double zr = 0.0, zi = 0.0;
    for (int i = 0; i < MAX_ITER; ++i) {
        squareAndAdd(zr, zi, cr, ci);
        if (zr * zr + zi * zi > 4.0) {          // 跑出半径 2 的圆，就再也回不来了
            return 255 - (i * 255) / MAX_ITER;  // 跑得越快越白，慢的越黑
        }
    }
    return 0;
}

// ---------- 把灰度缓冲区写成一张 BMP 图片 ----------
// BMP 头是"老式但极其直白"的二进制格式，正好用来学"文件不过是字节"。
void writeInt(std::ofstream& out, int value) {
    for (int i = 0; i < 4; ++i) {                // 小端序：低位字节在前
        out.put(static_cast<char>(value & 0xFF));
        value >>= 8;
    }
}

void writeShort(std::ofstream& out, int value) {
    out.put(static_cast<char>(value & 0xFF));
    out.put(static_cast<char>((value >> 8) & 0xFF));
}

bool saveBmp(const std::string& path, int w, int h, const std::vector<unsigned char>& gray) {
    const int rowBytes = (w + 3) / 4 * 4;        // BMP 规定每行字节数必须是 4 的倍数
    const int dataSize = rowBytes * h;
    const int fileSize = 54 + 1024 + dataSize;   // 头 54 字节 + 256 级灰度调色板

    std::ofstream out(path.c_str(), std::ios::binary);
    if (!out) return false;

    out.put('B'); out.put('M');                  // 文件头
    writeInt(out, fileSize);
    writeInt(out, 0);
    writeInt(out, 54 + 1024);

    writeInt(out, 40);                           // 信息头
    writeInt(out, w);
    writeInt(out, h);
    writeShort(out, 1);
    writeShort(out, 8);                          // 每像素 8 位 = 灰度图
    writeInt(out, 0);
    writeInt(out, dataSize);
    writeInt(out, 2835);
    writeInt(out, 2835);
    writeInt(out, 256);
    writeInt(out, 256);

    for (int i = 0; i < 256; ++i) {              // 调色板：第 i 号颜色就是灰度 i
        out.put(static_cast<char>(i));
        out.put(static_cast<char>(i));
        out.put(static_cast<char>(i));
        out.put(static_cast<char>(0));
    }

    for (int y = h - 1; y >= 0; --y) {           // BMP 的像素是自下而上存的
        for (int x = 0; x < w; ++x) {
            out.put(static_cast<char>(gray[y * w + x]));
        }
        for (int p = w; p < rowBytes; ++p) out.put(static_cast<char>(0));
    }
    return true;
}

// ---------- 在终端里先看一眼：把图缩小成字符画 ----------
void printPreview(const std::vector<unsigned char>& gray, int w, int h) {
    const int stepX = 12;
    const int stepY = 16;
    const std::string ramp = "@%#*+=-:. ";       // 从黑到白
    for (int y = 0; y < h; y += stepY) {
        std::cout << "  ";
        for (int x = 0; x < w; x += stepX) {
            const int level = gray[y * w + x] * (static_cast<int>(ramp.size()) - 1) / 255;
            std::cout << ramp[level];
        }
        std::cout << "\n";
    }
}

int main() {
    std::vector<unsigned char> gray(static_cast<size_t>(WIDTH) * HEIGHT, 0);

    std::cout << "Rendering " << WIDTH << "x" << HEIGHT << " ...\n";
    for (int y = 0; y < HEIGHT; ++y) {
        const double ci = -1.15 + 2.30 * y / HEIGHT;      // 把像素坐标映射到复平面
        for (int x = 0; x < WIDTH; ++x) {
            const double cr = -2.20 + 3.00 * x / WIDTH;
            gray[static_cast<size_t>(y) * WIDTH + x] =
                static_cast<unsigned char>(escapeTime(cr, ci));
        }
    }

    const std::string path = "mandelbrot.bmp";
    if (!saveBmp(path, WIDTH, HEIGHT, gray)) {
        std::cerr << "Cannot write " << path << "\n";
        return 1;
    }
    std::cout << "Saved " << path << " (" << (54 + 1024 + WIDTH * HEIGHT) << " bytes)\n\n";
    std::cout << "----- ASCII preview -----\n";
    printPreview(gray, WIDTH, HEIGHT);
    return 0;
}
