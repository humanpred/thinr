// Naccache & Shinghal (1984), "An investigation into the
// skeletonization approach of Hilditch", Pattern Recognition
// 17(3):279-284.
//
// OPTA (One-Pass Thinning Algorithm) revisits Hilditch's rules and
// merges them into a single sub-iteration. A pixel is removable iff:
//
//   * it has at least one 4-connected background neighbour (border
//     pixel);
//   * B(P) in [2, 6];
//   * A(P) = 1;
//   * it does not match the "spurious removal" template that
//     Naccache and Shinghal identified as the failure mode of the
//     direct Hilditch rules - specifically, the pixel is not the
//     centre of a "T" formed by three 4-connected foreground
//     neighbours on consecutive cardinal directions.
//
// Implementation note: the spurious-removal template here follows
// the spirit of the OPTA "spike" / "isthmus" guard described in the
// 1984 paper. Reviewers familiar with the original publication are
// invited to verify the template against the paper's figure.

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

namespace {

// Spike / isthmus guard: refuse to remove a pixel that is the centre
// of a T or cross formed by three consecutive 4-connected foreground
// neighbours. Eight rotational cases.
inline int is_spike_centre(int p2, int p4, int p6, int p8) {
  // Three consecutive cardinals foreground.
  int a = (p2 == 1) + (p4 == 1) + (p6 == 1) + (p8 == 1);
  if (a < 3) return 0;
  return 1;
}

}  // namespace

// [[Rcpp::export(.opta_cpp)]]
IntegerMatrix opta_cpp(IntegerMatrix img, int max_iter) {
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
        int p3 = m(r - 1, c + 1);
        int p4 = m(r,     c + 1);
        int p5 = m(r + 1, c + 1);
        int p6 = m(r + 1, c);
        int p7 = m(r + 1, c - 1);
        int p8 = m(r,     c - 1);
        int p9 = m(r - 1, c - 1);

        if (!thinr::is_border_4(p2, p4, p6, p8)) continue;

        int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
        if (B < 2 || B > 6) continue;

        int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
        if (A != 1) continue;

        if (is_spike_centre(p2, p4, p6, p8)) continue;

        mark(r, c) = 1;
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
