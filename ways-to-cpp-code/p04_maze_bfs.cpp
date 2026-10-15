// ============================================================
//  Ways to cpp · 项目 04  迷宫最短路径（广度优先搜索）
//  编译： g++ -std=c++14 -Wall p04_maze_bfs.cpp -o p04
//  运行： ./p04
// ============================================================

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using Grid = std::vector<std::string>;
using Cell = std::pair<int, int>;          // (行, 列)

const int DR[4] = {-1, 1, 0, 0};           // 上 下 左 右
const int DC[4] = {0, 0, -1, 1};

void printGrid(const Grid& g) {
    for (const std::string& row : g) {
        std::cout << "  " << row << "\n";
    }
}

// 广度优先搜索：像往水里丢石头，一圈一圈向外扩散。
// 第一次到达终点时走过的圈数，就是最短路的长度。
int shortestPath(Grid& grid, Cell start, Cell goal) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    std::vector<std::vector<int> > dist(rows, std::vector<int>(cols, -1));
    std::vector<std::vector<Cell> > parent(rows, std::vector<Cell>(cols, Cell(-1, -1)));

    std::queue<Cell> frontier;
    frontier.push(start);
    dist[start.first][start.second] = 0;

    while (!frontier.empty()) {
        const Cell cur = frontier.front();   // 取出队首（先进先出）
        frontier.pop();

        if (cur == goal) break;              // 到终点了，可以收工

        for (int k = 0; k < 4; ++k) {
            const int nr = cur.first + DR[k];
            const int nc = cur.second + DC[k];

            const bool inside = (nr >= 0 && nr < rows && nc >= 0 && nc < cols);
            if (!inside) continue;                        // 撞墙外
            if (grid[nr][nc] == '#') continue;            // 撞墙
            if (dist[nr][nc] != -1) continue;             // 已经来过（这才是 BFS 不绕圈的关键）

            dist[nr][nc] = dist[cur.first][cur.second] + 1;
            parent[nr][nc] = cur;
            frontier.push(Cell(nr, nc));
        }
    }

    if (dist[goal.first][goal.second] == -1) return -1;   // 走不到

    // 从终点顺着 parent 一路回溯到起点，把路画在图上
    Cell step = goal;
    while (!(step == start)) {
        if (grid[step.first][step.second] == '.') grid[step.first][step.second] = '*';
        step = parent[step.first][step.second];
    }
    return dist[goal.first][goal.second];
}

int main() {
    Grid grid = {
        "###############################",
        "#S..#.......#.....#...........#",
        "###.#####.#.###.#.#######.###.#",
        "#.#.#.....#...#.#...#.....#...#",
        "#.#.#.#######.#.###.#.#####.###",
        "#.#.#.#.#.....#...#...#...#...#",
        "#.#.#.#.#.#####.#.#####.#.###.#",
        "#...#.#...#...#.#.....#.#.....#",
        "#.###.#.###.#.#######.#.#######",
        "#.....#.....#.........#......E#",
        "###############################"
    };

    const Cell start(1, 1);
    const Cell goal(9, 29);

    std::cout << "Maze:\n";
    printGrid(grid);
    std::cout << "\n";

    const int steps = shortestPath(grid, start, goal);

    if (steps < 0) {
        std::cout << "No path found.\n";
    } else {
        std::cout << "Shortest distance : " << steps << " steps\n\n";
        std::cout << "One shortest path ('*'):\n";
        printGrid(grid);
    }
    return 0;
}
