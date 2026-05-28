// Naccache & Shinghal (1984), "An investigation into the
// skeletonization approach of Hilditch", Pattern Recognition
// 17(3):279-284. Also known as the Safe Point Thinning Algorithm
// (SPTA).
//
// Reference for the verified form: Lam, Lee & Suen (1992), "Thinning
// Methodologies - A Comprehensive Survey", IEEE TPAMI 14(9):869-885,
// page 873.
//
// A pixel p is a contour point in direction D iff its D-cardinal
// neighbour is background. For each direction, p is a "safe point"
// (preserved) if either:
//
//   N1: the direction's safe-point boolean expression evaluates to 0,
//       OR
//   N2: N(p) contains exactly two 4-adjacent foreground neighbours.
//
// p is deletable iff it is on at least one contour and is not safe
// under any of those contours.
//
// Safe-point expressions (one per direction; the west form is given
// in the survey explicitly, the others are 90 degree rotations).
// Each evaluates to 0 iff the pixel is safe in that direction:
//
//   West (p8 = 0):  p4 * (p3+p2+p6+p5) * (p2 + (1-p9)) * (p6 + (1-p7))
//   East (p4 = 0):  p8 * (p9+p2+p6+p7) * (p2 + (1-p3)) * (p6 + (1-p5))
//   North (p2 = 0): p6 * (p7+p8+p4+p5) * (p8 + (1-p9)) * (p4 + (1-p3))
//   South (p6 = 0): p2 * (p3+p4+p8+p9) * (p4 + (1-p5)) * (p8 + (1-p7))
//
// Iterate until no deletions.
//
// Implementation note: SPTA in the original paper performs two raster
// scans per cycle that together cover all four directions. The
// implementation here is the parallel-friendly equivalent: each cycle
// computes safety / deletability for every contour pixel using the
// pre-cycle state, then deletes the unsafe ones in batch. The
// per-direction safety conditions are unchanged; only the scan order
// differs from the sequential paper form.

#include <Rcpp.h>
using namespace Rcpp;

namespace {

// The four safe-point expressions; each returns true iff the
// corresponding direction's N1 condition holds (safe).

inline bool safe_west(int p2, int p3, int p4, int p5,
                      int p6, int p7, int /*p8*/, int p9) {
  int s2 = (p3 + p2 + p6 + p5) > 0;
  int s3 = (p2 + (1 - p9)) > 0;
  int s4 = (p6 + (1 - p7)) > 0;
  return (p4 * s2 * s3 * s4) == 0;
}

inline bool safe_east(int p2, int p3, int /*p4*/, int p5,
                      int p6, int p7, int p8, int p9) {
  int s2 = (p9 + p2 + p6 + p7) > 0;
  int s3 = (p2 + (1 - p3)) > 0;
  int s4 = (p6 + (1 - p5)) > 0;
  return (p8 * s2 * s3 * s4) == 0;
}

inline bool safe_north(int /*p2*/, int p3, int p4, int p5,
                       int p6, int p7, int p8, int p9) {
  int s2 = (p7 + p8 + p4 + p5) > 0;
  int s3 = (p8 + (1 - p9)) > 0;
  int s4 = (p4 + (1 - p3)) > 0;
  return (p6 * s2 * s3 * s4) == 0;
}

inline bool safe_south(int p2, int p3, int p4, int p5,
                       int /*p6*/, int p7, int p8, int p9) {
  int s2 = (p3 + p4 + p8 + p9) > 0;
  int s3 = (p4 + (1 - p5)) > 0;
  int s4 = (p8 + (1 - p7)) > 0;
  return (p2 * s2 * s3 * s4) == 0;
}

// N2: exactly two 4-adjacent foreground neighbours.
inline bool n2_protected(int p2, int p4, int p6, int p8) {
  return (p2 + p4 + p6 + p8) == 2;
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

        // N2 protection: pixel has exactly 2 4-adjacent FG neighbours.
        if (n2_protected(p2, p4, p6, p8)) continue;

        bool on_contour = false;
        bool is_safe = false;

        if (p8 == 0) {
          on_contour = true;
          if (safe_west(p2, p3, p4, p5, p6, p7, p8, p9)) is_safe = true;
        }
        if (p4 == 0) {
          on_contour = true;
          if (safe_east(p2, p3, p4, p5, p6, p7, p8, p9)) is_safe = true;
        }
        if (p2 == 0) {
          on_contour = true;
          if (safe_north(p2, p3, p4, p5, p6, p7, p8, p9)) is_safe = true;
        }
        if (p6 == 0) {
          on_contour = true;
          if (safe_south(p2, p3, p4, p5, p6, p7, p8, p9)) is_safe = true;
        }

        if (on_contour && !is_safe) mark(r, c) = 1;
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
