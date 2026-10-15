// PGM 版本：比 BMP 短得多，但 Windows 默认打不开
std::ofstream out("mandelbrot.pgm", std::ios::binary);
out << "P5\n" << WIDTH << " " << HEIGHT << "\n255\n";
for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
        out.put(static_cast<char>(gray[y * WIDTH + x]));
    }
}
