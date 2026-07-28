// Lee, Kashyap & Chu (1994) - "Building Skeleton Models via 3-D Medial
// Surface Axis Thinning Algorithms" - 2D adaptation.
//
// Lee's algorithm preserves topology via an Euler-invariance check on the
// 8-neighborhood. In 2D this check reduces to "crossing number equals 1",
// which is the same simple-point criterion used by Zhang-Suen. The shape
// difference from Zhang-Suen is the **sub-iteration order**: Lee processes
// each cardinal-direction boundary in its own sub-iteration (4 per outer
// pass), where Zhang-Suen uses 2 sub-iterations with mixed direction
// products. The directional ordering tends to give cleaner skeletons on
// asymmetric inputs.
//
// Sub-iterations:
//   0: northern-boundary pixels   (p2 == 0)
//   1: eastern-boundary pixels    (p4 == 0)
//   2: southern-boundary pixels   (p6 == 0)
//   3: western-boundary pixels    (p8 == 0)
//
// Each marks all deletable pixels for the current direction in parallel,
// then removes them. Repeats outer loop until convergence or max_iter.
//
// End-point preservation: a pixel with neighbour count < 2 is an isolated
// pixel or a curve endpoint and is preserved (B(p) >= 2 condition below).

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

// Neighbour layout:
//   p9 p2 p3
//   p8 P1 p4
//   p7 p6 p5
static inline int lee_can_delete(int p2, int p3, int p4, int p5,
                                 int p6, int p7, int p8, int p9, int sub) {
  // Directional boundary requirement: pixel must touch the relevant
  // background side for this sub-iteration.
  int on_boundary;
  switch (sub) {
    case 0:  on_boundary = (p2 == 0); break;  // north
    case 1:  on_boundary = (p4 == 0); break;  // east
    case 2:  on_boundary = (p6 == 0); break;  // south
    case 3:  on_boundary = (p8 == 0); break;  // west
    // Fail-fast assertion (unreachable): `sub` is driven only by the
    // internal `for (int sub = 0; sub < 4; sub++)` loop below, so values
    // outside 0-3 cannot reach here. If one ever does, the caller's loop
    // bound has been corrupted; abort loudly rather than silently treating
    // the pixel as interior so the broken invariant is caught, not masked.
    default: Rcpp::stop("thinr internal invariant: lee sub-iteration index out of range (expected 0-3) in lee_can_delete().");  // # nocov
  }
  if (!on_boundary) return 0;

  // Endpoint and interior-pixel guards: B(p) in [2, 6].
  // Below 2 -> isolated point or curve endpoint (preserve).
  // Above 6 -> interior pixel (removing it would punch a hole).
  int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
  if (B < 2 || B > 6) return 0;

  // Euler-invariance via crossing number: A(p) is the count of 0->1
  // transitions in the cyclic neighbour sequence p2,p3,...,p9,p2.
  // A(p) == 1 is the simple-point condition.
  int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
  if (A != 1) return 0;

  return 1;
}

// [[Rcpp::export(.lee_cpp)]]
IntegerMatrix lee_cpp(IntegerMatrix img, int max_iter) {
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
          if (lee_can_delete(p2, p3, p4, p5, p6, p7, p8, p9, sub)) {
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
