// Hilditch-family parallel thinning.
//
// References:
//   - Hilditch (1969), "Linear skeletons from square cupboards",
//     Machine Intelligence 4. Origin of the look-ahead crossing-number
//     idea.
//   - Rutovitz (1966), parallel form. Survey reference [103].
//   - Lam, Lee & Suen (1992), "Thinning Methodologies - A Comprehensive
//     Survey", IEEE TPAMI 14(9):869-885. The parallel form R1-R4 is
//     described on page 876; this implementation matches that form.
//
// Important: the implementation here is the **parallel form**
// commonly labelled "Hilditch" in modern image-processing references
// (Rutovitz R1-R4 with look-ahead crossing-number checks at p2 and
// p4). Hilditch's original 1969 algorithm is *sequential* with raster
// scan and within-pass deletion tracking via an R set, and uses the
// Hilditch crossing number X_H rather than the Rutovitz X_R / Zhang-
// Suen A(p) crossing number used here. The two produce similar but
// not identical skeletons. See the survey, pages 871-876.
//
// Distinctive feature of this form vs. Zhang-Suen: the look-ahead
// crossing-number check on cardinal neighbours - when conditions 3
// and 4 trigger, the algorithm computes A(p2) (or A(p4)) under the
// assumption that the centre pixel has been removed, and refuses the
// removal if that would change the topological character of the
// neighbour.
//
// Implementation note: the look-ahead requires reading rows r-2 /
// r+2 and columns c-2 / c+2. Out-of-bounds reads are treated as
// background.

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

namespace {

// A(p2) | p1 = 0. p2 sits at (r-1, c). p2's 8-neighbours clockwise
// from p2's north are: qn (r-2,c), qne (r-2,c+1), p3, p4,
// p1_set_to_zero, p8, p9, qnw (r-2,c-1).
inline int crossing_at_north(int qn,  int qne, int p3, int p4,
                             int p1,  int p8,  int p9, int qnw) {
  return (qn  == 0 && qne == 1) + (qne == 0 && p3  == 1)
       + (p3  == 0 && p4  == 1) + (p4  == 0 && p1  == 1)
       + (p1  == 0 && p8  == 1) + (p8  == 0 && p9  == 1)
       + (p9  == 0 && qnw == 1) + (qnw == 0 && qn  == 1);
}

// A(p4) | p1 = 0. p4 sits at (r, c+1). p4's 8-neighbours clockwise
// from p4's north are: p3, qen (r-1,c+2), qee (r,c+2),
// qes (r+1,c+2), p5, p6, p1_set_to_zero, p2.
inline int crossing_at_east(int p3, int qen, int qee, int qes,
                            int p5, int p6,  int p1,  int p2) {
  return (p3  == 0 && qen == 1) + (qen == 0 && qee == 1)
       + (qee == 0 && qes == 1) + (qes == 0 && p5  == 1)
       + (p5  == 0 && p6  == 1) + (p6  == 0 && p1  == 1)
       + (p1  == 0 && p2  == 1) + (p2  == 0 && p3  == 1);
}

}  // namespace

// [[Rcpp::export(.hilditch_cpp)]]
IntegerMatrix hilditch_cpp(IntegerMatrix img, int max_iter) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  IntegerMatrix m = clone(img);
  IntegerMatrix mark(nrow, ncol);

  auto get = [&](int r, int c) -> int {
    if (r < 0 || r >= nrow || c < 0 || c >= ncol) return 0;
    return m(r, c);
  };

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

        int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
        if (B < 2 || B > 6) continue;

        int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
        if (A != 1) continue;

        // Hilditch condition 3: p2 * p4 * p8 == 0 OR A(p2)|_{p1=0} == 1.
        if (p2 == 1 && p4 == 1 && p8 == 1) {
          int qn  = get(r - 2, c);
          int qne = get(r - 2, c + 1);
          int qnw = get(r - 2, c - 1);
          int A_p2 = crossing_at_north(qn, qne, p3, p4, 0, p8, p9, qnw);
          if (A_p2 != 1) continue;
        }

        // Hilditch condition 4: p2 * p4 * p6 == 0 OR A(p4)|_{p1=0} == 1.
        if (p2 == 1 && p4 == 1 && p6 == 1) {
          int qen = get(r - 1, c + 2);
          int qee = get(r,     c + 2);
          int qes = get(r + 1, c + 2);
          int A_p4 = crossing_at_east(p3, qen, qee, qes, p5, p6, 0, p2);
          if (A_p4 != 1) continue;
        }

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
