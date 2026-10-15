// ============================================================
//  Ways to cpp · 项目 03  康威生命游戏
//  编译： g++ -std=c++14 -Wall p03_game_of_life.cpp -o p03
//  运行： ./p03
// ============================================================

#include <iostream>
#include <string>
#include <vector>

// 给类型起个好名字：以后看到 Grid 就知道是"细胞的方阵"
using Grid = std::vector<std::vector<int> >;

const int ROWS = 14;
const int COLS = 34;

// ---------- 一、造一块空棋盘 ----------
Grid makeGrid(int rows, int cols) {
    return Grid(rows, std::vector<int>(cols, 0));
}

// ---------- 二、八个方向，写成一张表 ----------
// 把"八个方向"变成数据而不是八段 if，这是算法里非常常用的一步。
const int DR[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
const int DC[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

int liveNeighbors(const Grid& g, int r, int c) {
    int count = 0;
    for (int k = 0; k < 8; ++k) {
        const int nr = r + DR[k];
        const int nc = c + DC[k];
        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {  // 越界就跳过
            count += g[nr][nc];
        }
    }
    return count;
}

// ---------- 三、下一代：读旧格子，写新格子 ----------
// 关键在于"整体同时更新"。若在原地改，后面的格子会读到已经变过的邻居。
Grid nextGeneration(const Grid& current) {
    Grid next = makeGrid(ROWS, COLS);
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            const int n = liveNeighbors(current, r, c);
            if (current[r][c] == 1) {
                next[r][c] = (n == 2 || n == 3) ? 1 : 0;   // 活细胞：太挤或太孤单都会死
            } else {
                next[r][c] = (n == 3) ? 1 : 0;             // 死细胞：正好三个邻居就复活
            }
        }
    }
    return next;
}

// ---------- 四、打印 ----------
void printGrid(const Grid& g, int generation) {
    std::cout << "generation " << generation << "\n";
    std::cout << "+" << std::string(COLS, '-') << "+\n";
    for (int r = 0; r < ROWS; ++r) {
        std::cout << "|";
        for (int c = 0; c < COLS; ++c) {
            std::cout << (g[r][c] ? '#' : '.');
        }
        std::cout << "|\n";
    }
    std::cout << "+" << std::string(COLS, '-') << "+\n\n";
}

// ---------- 五、一块著名的初始图案：滑翔机 ----------
void placeGlider(Grid& g, int top, int left) {
    const int pattern[3][3] = {
        {0, 1, 0},
        {0, 0, 1},
        {1, 1, 1}
    };
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            g[top + r][left + c] = pattern[r][c];
        }
    }
}

int main() {
    Grid grid = makeGrid(ROWS, COLS);
    placeGlider(grid, 1, 1);

    for (int generation = 0; generation <= 4; ++generation) {
        printGrid(grid, generation);
        grid = nextGeneration(grid);
    }
    return 0;
}
