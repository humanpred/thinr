// Four-template directional thinning, commonly cited as the
// "Stentiford" thinning algorithm.
//
// Important: this method's common name in the image-processing
// literature is a **folk misattribution**. The Stentiford &
// Mortimer (1983) paper ("Some new heuristics for thinning binary
// handprinted characters for OCR", IEEE Trans Sys Man Cyb 13(1):
// 81-84) actually describes *preprocessing heuristics* (hole
// removal, smoothing, acute-angle emphasis via H / I / D / U
// templates) intended to run *before* a separate thinning step. It
// is not a complete thinning algorithm. The 4-directional template
// approach implemented here is closer to that of Stefanelli &
// Rosenfeld (1971), survey reference [115]; the algorithm has been
// retained under the name "stentiford" for compatibility with the
// common literature attribution.
//
// Algorithm:
//   Four directional templates (T1 = top, T2 = right, T3 = bottom,
//   T4 = left), one per sub-iteration. Each template requires the
//   entire 3-pixel side of the 3x3 neighbourhood to be background:
//   T1 requires p9 == p2 == p3 == 0, etc. Marked pixels are removed
//   iff connectivity is preserved (A(P) = 1) and the pixel is not
//   an endpoint (B(P) >= 2).

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

namespace {

inline int matches_template(int p2, int p3, int p4, int p5,
                            int p6, int p7, int p8, int p9, int sub) {
  switch (sub) {
    case 0:  return (p9 == 0 && p2 == 0 && p3 == 0);  // T1: top
    case 1:  return (p3 == 0 && p4 == 0 && p5 == 0);  // T2: right
    case 2:  return (p5 == 0 && p6 == 0 && p7 == 0);  // T3: bottom
    case 3:  return (p7 == 0 && p8 == 0 && p9 == 0);  // T4: left
    default: return 0;
  }
}

}  // namespace

// [[Rcpp::export(.stentiford_cpp)]]
IntegerMatrix stentiford_cpp(IntegerMatrix img, int max_iter) {
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

          if (!matches_template(p2, p3, p4, p5, p6, p7, p8, p9, sub)) continue;

          int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
          if (B < 2) continue;

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
