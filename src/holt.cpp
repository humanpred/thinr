// Holt, Stewart, Clint & Perrott (1987), "An improved parallel
// thinning algorithm", Communications of the ACM 30(2):156-160.
//
// Reference for the verified form: Lam, Lee & Suen (1992), "Thinning
// Methodologies - A Comprehensive Survey", IEEE TPAMI 14(9):869-885,
// page 877.
//
// One-subcycle parallel algorithm. A pixel p is deleted iff Holt's
// condition H is true:
//
//   H = edge(p) AND
//       (~edge(x_1) OR ~x_3 OR ~x_7) AND
//       (~edge(x_7) OR ~x_5 OR ~x_3) AND
//       (~edge(x_1) OR ~edge(x_8) OR ~edge(x_7))
//
// where edge(q) means q has at least one 4-cardinal background
// neighbour. In thinr's labelling, x_1 = E (p4), x_3 = N (p2),
// x_5 = W (p8), x_7 = S (p6), x_8 = SE (p5).
//
// The survey notes Holt is "almost equivalent" to the Rutovitz R1-R4
// parallel form except that Holt uses edge-information on neighbours
// rather than crossing-number information, and adds the third
// compound expression (the 3-edge condition on E, SE, S).
//
// Implementation note: evaluating edge(x_1), edge(x_7), edge(x_8)
// requires reading a 5x5 window around p (we need each neighbour's
// own 4-cardinals). Out-of-bounds reads are treated as background.

#include <Rcpp.h>
using namespace Rcpp;

namespace {

// edge(q): true iff q is foreground AND has at least one 4-cardinal
// background neighbour. Out-of-image positions are treated as
// background.
inline bool is_edge_at(const IntegerMatrix& m, int r, int c,
                       int nrow, int ncol) {
  if (r < 0 || r >= nrow || c < 0 || c >= ncol) return false;
  if (m(r, c) != 1) return false;
  int N = (r > 0)        ? m(r - 1, c) : 0;
  int E = (c < ncol - 1) ? m(r, c + 1) : 0;
  int S = (r < nrow - 1) ? m(r + 1, c) : 0;
  int W = (c > 0)        ? m(r, c - 1) : 0;
  return (N == 0) || (E == 0) || (S == 0) || (W == 0);
}

}  // namespace

// [[Rcpp::export(.holt_cpp)]]
IntegerMatrix holt_cpp(IntegerMatrix img, int max_iter) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  IntegerMatrix m = clone(img);
  IntegerMatrix mark(nrow, ncol);

  for (int it = 0; it < max_iter; it++) {
    bool changed = false;
    std::fill(mark.begin(), mark.end(), 0);

    for (int r = 1; r < nrow - 1; r++) {
      for (int c = 1; c < ncol - 1; c++) {
        if (m(r, c) != 1) continue;
        int p2 = m(r - 1, c);
        int p4 = m(r,     c + 1);
        int p5 = m(r + 1, c + 1);
        int p6 = m(r + 1, c);
        int p8 = m(r,     c - 1);

        int p3 = m(r - 1, c + 1);
        int p7 = m(r + 1, c - 1);
        int p9 = m(r - 1, c - 1);

        // edge(p): p has at least one 4-cardinal BG neighbour.
        bool edge_p = (p2 == 0) || (p4 == 0) || (p6 == 0) || (p8 == 0);
        if (!edge_p) continue;

        // Survey-implicit precondition (Lam-Lee-Suen 1992, page 872):
        // p is "not an isolated or end point" i.e. b(p) >= 2. Without
        // this guard, Holt's H would delete isolated foreground pixels.
        int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
        if (B < 2) continue;

        bool edge_x1 = is_edge_at(m, r,     c + 1, nrow, ncol);  // E  = p4
        bool edge_x7 = is_edge_at(m, r + 1, c,     nrow, ncol);  // S  = p6
        bool edge_x8 = is_edge_at(m, r + 1, c + 1, nrow, ncol);  // SE = p5

        // (~edge(x_1) OR ~x_3 OR ~x_7) - x_3 = N = p2, x_7 = S = p6
        bool a = (!edge_x1) || (p2 == 0) || (p6 == 0);
        // (~edge(x_7) OR ~x_5 OR ~x_3) - x_5 = W = p8
        bool b = (!edge_x7) || (p8 == 0) || (p2 == 0);
        // (~edge(x_1) OR ~edge(x_8) OR ~edge(x_7))
        bool c_ = (!edge_x1) || (!edge_x8) || (!edge_x7);

        if (a && b && c_) mark(r, c) = 1;
      }
    }

    for (int i = 0; i < nrow * ncol; i++) {
      if (mark[i]) {
        m[i] = 0;
        changed = true;
      }
    }

    if (!changed) break;
  }

  return m;
}
