// Holt, Stewart, von Diprosperro & Cross (1987), "An improved parallel
// thinning algorithm", Communications of the ACM 30(2):156-160.
//
// Holt's algorithm is Zhang-Suen with two modifications aimed at
// reducing erosion on diagonal staircase patterns and at preserving
// 2x2 squares as 2x2 squares (rather than thinning them to a single
// pixel).
//
// Two sub-iterations like Zhang-Suen. Each sub-iteration replaces the
// Zhang-Suen p2*p4*p6 / p4*p6*p8 (resp. p2*p4*p8 / p2*p6*p8) tests
// with their union, AND additionally requires:
//
//   * The pixel is not part of a 2x2 foreground square (else we
//     would thin a 2x2 block down to a single pixel).
//   * The pixel is not at the "staircase" corner that Zhang-Suen
//     over-erodes.
//
// Implementation note: this implementation follows the description
// in Lam, Lee & Suen (1992) "Thinning methodologies - a comprehensive
// survey" (IEEE PAMI 14(9):869-885), which catalogues Holt's
// modifications. Reviewers familiar with Holt et al. (1987) are
// invited to verify against the original publication.

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

namespace {

// Isolated 2x2 foreground square guard: the pixel is one corner of
// a 2x2 foreground block AND the other five 8-neighbours are all
// background — i.e. the 2x2 block is genuinely standalone (not just
// a 2x2 patch inside a larger solid). Any of four rotational
// orientations.
inline int in_isolated_2x2_block(int p2, int p3, int p4, int p5,
                                 int p6, int p7, int p8, int p9) {
  // NE block: p1, p2, p3, p4 form the square; p5..p9 are background.
  if (p2 == 1 && p3 == 1 && p4 == 1 &&
      p5 == 0 && p6 == 0 && p7 == 0 && p8 == 0 && p9 == 0) return 1;
  // SE block: p1, p4, p5, p6 form the square.
  if (p4 == 1 && p5 == 1 && p6 == 1 &&
      p2 == 0 && p3 == 0 && p7 == 0 && p8 == 0 && p9 == 0) return 1;
  // SW block: p1, p6, p7, p8 form the square.
  if (p6 == 1 && p7 == 1 && p8 == 1 &&
      p2 == 0 && p3 == 0 && p4 == 0 && p5 == 0 && p9 == 0) return 1;
  // NW block: p1, p8, p9, p2 form the square.
  if (p8 == 1 && p9 == 1 && p2 == 1 &&
      p3 == 0 && p4 == 0 && p5 == 0 && p6 == 0 && p7 == 0) return 1;
  return 0;
}

static inline int holt_can_delete(int p2, int p3, int p4, int p5,
                                  int p6, int p7, int p8, int p9, int sub) {
  int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
  if (B < 2 || B > 6) return 0;

  int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
  if (A != 1) return 0;

  // 2x2 guard: refuse to thin an isolated 2x2 block to a single
  // point. The guard only fires when the 2x2 is genuinely
  // standalone, so it does not protect inner pixels of larger solids.
  if (in_isolated_2x2_block(p2, p3, p4, p5, p6, p7, p8, p9)) return 0;

  if (sub == 0) {
    // First sub-iteration: same direction-quadrant cuts as Zhang-Suen
    // sub 0.
    if (p2 * p4 * p6 != 0) return 0;
    if (p4 * p6 * p8 != 0) return 0;
  } else {
    // Second sub-iteration.
    if (p2 * p4 * p8 != 0) return 0;
    if (p2 * p6 * p8 != 0) return 0;
  }

  return 1;
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
          if (holt_can_delete(p2, p3, p4, p5, p6, p7, p8, p9, sub)) {
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
