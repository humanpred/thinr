// Restrictive 4-directional thinning, retained under the name
// "pavlidis" for compatibility with how the algorithm is commonly
// cited.
//
// Important: this implementation **does not match** Pavlidis's
// original (1980) "A thinning algorithm for discrete binary images",
// CGIP 13(2):142-157. The 1980 paper presents a *contour-following*
// thinning algorithm with multi-pixel detection masks (see Lam, Lee
// & Suen 1992 survey, pages 873-875, reference [86]). What is
// implemented here is a different beast: a Lee-2D-style parallel
// thinning with a tighter `B(P) in [2, 5]` neighbour-count bound,
// not the contour-traced multi-pixel approach of the actual paper.
//
// The faithful Pavlidis contour-following implementation is on the
// thinr roadmap for a later release. For now, this algorithm is
// shipped under the same name because the package's existing users
// reach for `thin(method = "pavlidis")` to get the restrictive
// behaviour described below.
//
// Algorithm:
//   Four directional sub-iterations like Lee 2D, but a pixel needs
//   B(P) in [2, 5] (Lee uses [2, 6]). The lower upper-bound preserves
//   pixels with 6 or more foreground neighbours, which Lee would
//   erode. Crossing-number topology check (A(P) = 1) is the same as
//   Lee.

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

namespace {

inline int on_boundary(int p2, int p4, int p6, int p8, int sub) {
  switch (sub) {
    case 0:  return p2 == 0;  // north boundary
    case 1:  return p4 == 0;  // east  boundary
    case 2:  return p6 == 0;  // south boundary
    case 3:  return p8 == 0;  // west  boundary
    default: return 0;
  }
}

}  // namespace

// [[Rcpp::export(.pavlidis_cpp)]]
IntegerMatrix pavlidis_cpp(IntegerMatrix img, int max_iter) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  IntegerMatrix m = clone(img);
  IntegerMatrix mark(nrow, ncol);

  for (int it = 0; it < max_iter; it++) {
    bool changed = false;

    for (int sub = 0; sub < 4; sub++) {
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

          if (!on_boundary(p2, p4, p6, p8, sub)) continue;

          int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
          if (B < 2 || B > 5) continue;

          int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
          if (A != 1) continue;

          mark(r, c) = 1;
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
