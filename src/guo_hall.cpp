// Guo & Hall (1989), "Parallel thinning with two-subiteration
// algorithms". Two-pass per iteration. Following the formulation used in
// OpenCV's ximgproc (which implements the original paper faithfully) for
// the connectivity number C(p) and the N1 / N2 / N(p) counts.

#include <Rcpp.h>
using namespace Rcpp;

// Neighbour layout:
//   P9 P2 P3
//   P8 P1 P4
//   P7 P6 P5
static inline int gh_can_delete(int p2, int p3, int p4, int p5,
                                int p6, int p7, int p8, int p9, int sub) {
  // C(p): number of 0-blocks where a transition into foreground occurs,
  // counted on every other neighbour. Connectivity number in the sense
  // of Guo-Hall (not the Zhang-Suen A count).
  int C = ((!p2) && (p3 || p4))
        + ((!p4) && (p5 || p6))
        + ((!p6) && (p7 || p8))
        + ((!p8) && (p9 || p2));
  if (C != 1) return 0;

  int N1 = (p9 || p2) + (p3 || p4) + (p5 || p6) + (p7 || p8);
  int N2 = (p2 || p3) + (p4 || p5) + (p6 || p7) + (p8 || p9);
  int N  = (N1 < N2) ? N1 : N2;
  if (N < 2 || N > 3) return 0;

  int m;
  if (sub == 0) {
    m = (p2 || p3 || !p5) && p4;
  } else {
    m = (p6 || p7 || !p9) && p8;
  }
  if (m != 0) return 0;

  return 1;
}

// [[Rcpp::export(.guo_hall_cpp)]]
IntegerMatrix guo_hall_cpp(IntegerMatrix img, int max_iter) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  IntegerMatrix m = clone(img);
  IntegerMatrix mark(nrow, ncol);

  for (int it = 0; it < max_iter; it++) {
    bool changed = false;

    for (int sub = 0; sub < 2; sub++) {
      std::fill(mark.begin(), mark.end(), 0);

      for (int r = 1; r < nrow - 1; r++) {
        for (int c = 1; c < ncol - 1; c++) {
          if (m(r, c) != 1) continue;
          int p2 = m(r - 1, c);
          int p3 = m(r - 1, c + 1);
          int p4 = m(r,     c + 1);
          int p5 = m(r + 1, c + 1);
          int p6 = m(r + 1, c);
          int p7 = m(r + 1, c - 1);
          int p8 = m(r,     c - 1);
          int p9 = m(r - 1, c - 1);
          if (gh_can_delete(p2, p3, p4, p5, p6, p7, p8, p9, sub)) {
            mark(r, c) = 1;
          }
        }
      }

      for (int i = 0; i < nrow * ncol; i++) {
        if (mark[i]) {
          m[i] = 0;
          changed = true;
        }
      }
    }

    if (!changed) break;
  }

  return m;
}
