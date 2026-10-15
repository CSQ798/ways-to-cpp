/* ============================================================================
   Ways to cpp —— 页面重建脚本
   ----------------------------------------------------------------------------
   把本目录下的 HTML 片段拼装成最终的 "Ways to cpp.html"，
   并把 ways-to-cpp-code/ 里真实的 C++ 源码与真实运行输出注入进去
   （带灰度语法高亮，不使用任何第三方库）。

   用法（在仓库根目录执行）：
       node ways-to-cpp-source/build.js index.html      # 生成网页（GitHub Pages 入口）
       node ways-to-cpp-source/build.js                 # 默认输出 "Ways to cpp.html"

   若要用 Edge 重新生成 PDF：
       msedge --headless=new --no-pdf-header-footer ^
              --print-to-pdf="Ways to cpp.pdf" "index.html"
   ========================================================================== */
const fs = require('fs');
const path = require('path');

const SRC = __dirname;                          // 本目录 = HTML 片段所在处
const ROOT = path.dirname(SRC);                 // 仓库根目录
const CODE = path.join(ROOT, 'ways-to-cpp-code');
const OUTDIR = path.join(CODE, '_out');         // 程序真实输出（可选）

// 输出文件名：默认 "Ways to cpp.html"，也可以指定，例如
//     node ways-to-cpp-source/build.js index.html
const TARGET = path.join(ROOT, process.argv[2] || 'Ways to cpp.html');

/* ---------------- HTML 转义 ---------------- */
function esc(s) {
  return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

/* ---------------- 灰度语法高亮 ---------------- */
const KEYWORDS = new Set(('alignas alignof and asm auto bool break case catch char class const constexpr continue ' +
  'decltype default delete do double else enum explicit export extern false float for friend goto if inline int ' +
  'long mutable namespace new noexcept nullptr operator private protected public register return short signed sizeof ' +
  'static struct switch template this throw true try typedef typename union unsigned using virtual void volatile while').split(' '));

const TYPES = new Set(('std string vector map set queue stack pair cout cin cerr clog endl ifstream ofstream ' +
  'size_t ostream istream ostringstream stringstream mt19937 normal_distribution uniform_real_distribution ' +
  'numeric_limits WordStats Student Grid Cell Matrix Perceptron NaiveBayes Sample Table Counter WordCount').split(' '));

const RE = /(\/\/[^\n]*|\/\*[\s\S]*?\*\/)|("(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')|(^[ \t]*#[^\n]*)|(\b[A-Za-z_]\w*\b)|(\b\d[\w.]*\b)/gm;

function highlight(src) {
  return esc(src).replace(RE, function (m, com, str, pre, ident, num) {
    if (com) return '<span class="tok-c">' + m + '</span>';
    if (str) return '<span class="tok-s">' + m + '</span>';
    if (pre) return '<span class="tok-p">' + m + '</span>';
    if (ident) {
      if (KEYWORDS.has(ident)) return '<span class="tok-k">' + m + '</span>';
      if (TYPES.has(ident))    return '<span class="tok-t">' + m + '</span>';
      return m;
    }
    if (num) return '<span class="tok-n">' + m + '</span>';
    return m;
  });
}

/* ---------------- 读取（去掉 BOM，统一换行） ---------------- */
function readText(p) {
  let s = fs.readFileSync(p, 'utf8');
  if (s.charCodeAt(0) === 0xFEFF) s = s.slice(1);
  return s.replace(/\r\n/g, '\n').replace(/\s+$/, '');
}

/* ---------------- 项目编号 -> 源文件 ---------------- */
const MAP = {
  '01': 'p01_grade_analyzer.cpp',
  '02': 'p02_word_count.cpp',
  '03': 'p03_game_of_life.cpp',
  '04': 'p04_maze_bfs.cpp',
  '05': 'p05_calculator.cpp',
  '06': 'p06_tictactoe.cpp',
  '07': 'p07_matrix.cpp',
  '08': 'p08_perceptron.cpp',
  '09': 'p09_naive_bayes.cpp',
  '10': 'p10_mandelbrot.cpp'
};

function codeFor(id) {
  if (id === 'pgm') return highlight(readText(path.join(SRC, 'snippet_pgm.cpp')));
  const f = MAP[id];
  if (!f) throw new Error('未知的代码编号: ' + id);
  return highlight(readText(path.join(CODE, f)));
}

function outFor(id) {
  const f = MAP[id];
  if (!f) throw new Error('未知的输出编号: ' + id);
  const file = path.join(OUTDIR, f.replace(/\.cpp$/, '.txt'));
  if (!fs.existsSync(file)) {
    // 还没有跑过程序时，放一句提示，页面依然能正常生成
    return '（尚未生成运行输出：请先编译并运行 ' + f + '，把输出保存到 _out/ 下）';
  }
  let text = readText(file);

  // 项目 03 的完整输出有 90 行，PDF 里太长：只留第 0 代与第 4 代
  if (id === '03') {
    const lines = text.split('\n');
    const head = lines.slice(0, 18);
    const tail = lines.slice(lines.length - 18);
    text = head.concat(['', '        ( ...... 中间第 1 ~ 3 代略 ...... )', '']).concat(tail).join('\n');
  }
  return esc(text);
}

/* ---------------- 拼装 ---------------- */
const parts = fs.readdirSync(SRC).filter(f => /^\d+_.*\.html$/.test(f)).sort();
if (!parts.length) throw new Error('没有找到任何 HTML 片段');

let html = parts.map(f => readText(path.join(SRC, f))).join('\n\n');

let codeCount = 0, outCount = 0;
html = html.replace(/<!--CODE:([0-9a-z]+)-->/g, (_, id) => { codeCount++; return codeFor(id); });
html = html.replace(/<!--OUT:([0-9a-z]+)-->/g,  (_, id) => { outCount++;  return outFor(id);  });

const left = html.match(/<!--(CODE|OUT):[^>]*-->/g);
if (left) throw new Error('还有未替换的占位符: ' + left.join(', '));

fs.writeFileSync(TARGET, html, 'utf8');

console.log('片段     : ' + parts.join('  '));
console.log('注入代码 : ' + codeCount + ' 段');
console.log('注入输出 : ' + outCount + ' 段');
console.log('输出文件 : ' + TARGET);
console.log('大小     : ' + (Buffer.byteLength(html, 'utf8') / 1024).toFixed(1) + ' KB');
