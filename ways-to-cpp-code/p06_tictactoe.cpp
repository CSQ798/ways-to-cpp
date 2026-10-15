// ============================================================
//  Ways to cpp · 项目 06  井字棋与极小极大搜索
//  编译： g++ -std=c++14 -Wall p06_tictactoe.cpp -o p06
//  运行： ./p06
// ============================================================

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

const char EMPTY = '.';
const char HUMAN = 'X';   // 先手
const char AI    = 'O';

const int LINES[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},   // 三行
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},   // 三列
    {0, 4, 8}, {2, 4, 6}               // 两条对角线
};

char winnerOf(const std::vector<char>& b) {
    for (int i = 0; i < 8; ++i) {
        const int a = LINES[i][0], m = LINES[i][1], c = LINES[i][2];
        if (b[a] != EMPTY && b[a] == b[m] && b[m] == b[c]) return b[a];
    }
    return EMPTY;                       // 没人赢
}

bool boardFull(const std::vector<char>& b) {
    for (char c : b) {
        if (c == EMPTY) return false;
    }
    return true;
}

void printBoard(const std::vector<char>& b) {
    for (int r = 0; r < 3; ++r) {
        std::cout << "  ";
        for (int c = 0; c < 3; ++c) {
            std::cout << b[r * 3 + c];
            if (c < 2) std::cout << " | ";
        }
        std::cout << "\n";
        if (r < 2) std::cout << " ---+---+---\n";
    }
}

// ---------- 极小极大搜索 ----------
// board 是"按值传递"的：每次递归都拿到一份自己的副本，
// 于是我们可以在里面随便改，返回时外面的世界毫发无损。
// 这是用拷贝换来的、极其干净的一段递归。
int minimax(std::vector<char> board, char player, int depth) {
    const char w = winnerOf(board);
    if (w == AI)    return 10 - depth;      // 赢得越早，分数越高
    if (w == HUMAN) return depth - 10;      // 输得越晚，分数越接近 0
    if (boardFull(board)) return 0;         // 平局

    const char next = (player == AI) ? HUMAN : AI;
    int best = (player == AI) ? std::numeric_limits<int>::min()
                              : std::numeric_limits<int>::max();

    for (int i = 0; i < 9; ++i) {
        if (board[i] != EMPTY) continue;

        board[i] = player;                          // 试着走一步
        const int score = minimax(board, next, depth + 1);
        board[i] = EMPTY;                           // 撤销这一步

        if (player == AI) best = std::max(best, score);   // 我尽力拿高分
        else              best = std::min(best, score);   // 对手尽力压低我的分
    }
    return best;
}

// 在所有空位里挑一个"最坏情况下的最好结果"
int bestMove(const std::vector<char>& board, char player, int& outScore) {
    const char next = (player == AI) ? HUMAN : AI;
    int bestIndex = -1;
    int best = (player == AI) ? std::numeric_limits<int>::min()
                              : std::numeric_limits<int>::max();

    for (int i = 0; i < 9; ++i) {
        if (board[i] != EMPTY) continue;
        std::vector<char> trial = board;
        trial[i] = player;
        const int score = minimax(trial, next, 1);
        const bool better = (player == AI) ? (score > best) : (score < best);
        if (better) {
            best = score;
            bestIndex = i;
        }
    }
    outScore = best;
    return bestIndex;
}

int main() {
    std::cout << "----- Demo 1 : must block -----\n";
    std::vector<char> mid = {
        'X', '.', '.',
        '.', 'X', '.',
        'O', '.', '.'
    };
    printBoard(mid);
    int score = 0;
    const int move = bestMove(mid, AI, score);
    std::cout << "AI ('O') chooses cell " << move
              << " (row " << (move / 3 + 1) << ", col " << (move % 3 + 1) << ")"
              << ", evaluated score = " << score << "\n\n";

    std::cout << "----- Demo 2 : perfect vs perfect -----\n";
    std::vector<char> board(9, EMPTY);
    char player = HUMAN;                 // X 先手
    while (winnerOf(board) == EMPTY && !boardFull(board)) {
        int s = 0;
        const int m = bestMove(board, player, s);
        board[m] = player;
        player = (player == HUMAN) ? AI : HUMAN;
    }
    printBoard(board);
    const char w = winnerOf(board);
    std::cout << (w == EMPTY ? "Result: draw" : std::string("Winner: ") + w) << "\n";
    return 0;
}
