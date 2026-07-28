// Zhang & Suen (1984), "A fast parallel algorithm for thinning digital
// patterns". Two sub-iterations per pass; one pass removes border pixels
// from the N-W and S-E quadrants, the other from N-E and S-W.

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

// Neighbour layout, P2 = north, going clockwise:
//   P9 P2 P3
//   P8 P1 P4
//   P7 P6 P5
static inline int zs_can_delete(int p2, int p3, int p4, int p5,
                                int p6, int p7, int p8, int p9, int sub) {
  // B(P1): non-zero neighbour count.
  int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
  if (B < 2 || B > 6) return 0;

  // A(P1): 0->1 transitions in the cyclic sequence P2,P3,...,P9,P2.
  int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
  if (A != 1) return 0;

  if (sub == 0) {
    if (p2 * p4 * p6 != 0) return 0;
    if (p4 * p6 * p8 != 0) return 0;
  } else {
    if (p2 * p4 * p8 != 0) return 0;
    if (p2 * p6 * p8 != 0) return 0;
  }
  return 1;
}

// [[Rcpp::export(.zhang_suen_cpp)]]
IntegerMatrix zhang_suen_cpp(IntegerMatrix img, int max_iter) {
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
          if (zs_can_delete(p2, p3, p4, p5, p6, p7, p8, p9, sub)) {
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
