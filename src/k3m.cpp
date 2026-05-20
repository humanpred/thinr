// Saeed, Tabedzki, Rybnik & Adamski (2010) - "K3M: A universal
// algorithm for image skeletonization and a review of thinning
// techniques", Int. J. Appl. Math. Comput. Sci. 20(2):317-335.
// doi:10.2478/v10006-010-0024-4
//
// Sequential (scanline-type) iterative thinning. Each iteration:
//
//   Phase 0  Scan all foreground pixels; mark as "border" any pixel
//            whose 8-neighbour weight w(x,y) is in lookup table A_0.
//   Phase 1  Scan border pixels in raster order; delete any whose
//            current w(x,y) is in A_1.
//   Phase 2  Same, with A_2.
//   Phase 3  Same, with A_3.
//   Phase 4  Same, with A_4.
//   Phase 5  Same, with A_5.
//   Phase 6  Unmark remaining borders (implicit - the next iteration
//            re-runs Phase 0 from scratch).
//
// Iterate until phases 1..5 make no modifications. Then run a final
// "one-pixel-width" pass that scans all foreground pixels and deletes
// any whose w(x,y) is in lookup table A_1pix.
//
// Neighbour weight w(x,y) follows the paper's matrix N (Eq. 3, p. 326):
//
//     128   1   2
//      64       4
//      32  16   8
//
// In thinr's 8-neighbour labelling (p2 = N, going clockwise):
//
//     p9=NW (128)   p2=N (1)    p3=NE (2)
//     p8=W   (64)               p4=E   (4)
//     p7=SW  (32)   p6=S (16)   p5=SE  (8)
//
// The lookup arrays A_0..A_5 and A_1pix are reproduced verbatim from
// Saeed et al. (2010), Section 3.3 ("Components of neighbourhood
// lookup arrays", p. 327).

#include <Rcpp.h>
using namespace Rcpp;

namespace {

// A_0: border-marking lookup. 48 patterns. Section 3.3, p. 327.
const int A_0_DATA[] = {
    3,   6,   7,  12,  14,  15,  24,  28,  30,  31,
   48,  56,  60,  62,  63,  96, 112, 120, 124, 126,
  127, 129, 131, 135, 143, 159, 191, 192, 193, 195,
  199, 207, 223, 224, 225, 227, 231, 239, 240, 241,
  243, 247, 248, 249, 251, 252, 253, 254
};

// A_1: 8 patterns. Phase 1 deletion lookup.
const int A_1_DATA[] = {7, 14, 28, 56, 112, 131, 193, 224};

// A_2: 16 patterns. Phase 2 deletion lookup (cumulative; A_1 in A_2).
const int A_2_DATA[] = {
    7,  14,  15,  28,  30,  56,  60, 112, 120, 131,
  135, 193, 195, 224, 225, 240
};

// A_3: 24 patterns. Phase 3 deletion lookup (A_2 in A_3).
const int A_3_DATA[] = {
    7,  14,  15,  28,  30,  31,  56,  60,  62, 112,
  120, 124, 131, 135, 143, 193, 195, 199, 224, 225,
  227, 240, 241, 248
};

// A_4: 32 patterns. Phase 4 deletion lookup (A_3 in A_4).
const int A_4_DATA[] = {
    7,  14,  15,  28,  30,  31,  56,  60,  62,  63,
  112, 120, 124, 126, 131, 135, 143, 159, 193, 195,
  199, 207, 224, 225, 227, 231, 240, 241, 243, 248,
  249, 252
};

// A_5: 38 patterns. Phase 5 deletion lookup (A_4 in A_5).
const int A_5_DATA[] = {
    7,  14,  15,  28,  30,  31,  56,  60,  62,  63,
  112, 120, 124, 126, 131, 135, 143, 159, 191, 193,
  195, 199, 207, 224, 225, 227, 231, 239, 240, 241,
  243, 247, 248, 249, 251, 252, 253, 254
};

// A_1pix: one-pixel-width thinning lookup. Identical to A_0 in the
// published table. 48 patterns.
const int A_1PIX_DATA[] = {
    3,   6,   7,  12,  14,  15,  24,  28,  30,  31,
   48,  56,  60,  62,  63,  96, 112, 120, 124, 126,
  127, 129, 131, 135, 143, 159, 191, 192, 193, 195,
  199, 207, 223, 224, 225, 227, 231, 239, 240, 241,
  243, 247, 248, 249, 251, 252, 253, 254
};

struct K3MTables {
  bool a0[256];
  bool a[6][256];      // indexed 1..5; a[0] unused
  bool a1pix[256];
  K3MTables() {
    for (int v = 0; v < 256; v++) {
      a0[v] = false;
      a1pix[v] = false;
      for (int p = 0; p < 6; p++) a[p][v] = false;
    }
    for (size_t i = 0; i < sizeof(A_0_DATA)    / sizeof(int); i++) a0[A_0_DATA[i]] = true;
    for (size_t i = 0; i < sizeof(A_1_DATA)    / sizeof(int); i++) a[1][A_1_DATA[i]] = true;
    for (size_t i = 0; i < sizeof(A_2_DATA)    / sizeof(int); i++) a[2][A_2_DATA[i]] = true;
    for (size_t i = 0; i < sizeof(A_3_DATA)    / sizeof(int); i++) a[3][A_3_DATA[i]] = true;
    for (size_t i = 0; i < sizeof(A_4_DATA)    / sizeof(int); i++) a[4][A_4_DATA[i]] = true;
    for (size_t i = 0; i < sizeof(A_5_DATA)    / sizeof(int); i++) a[5][A_5_DATA[i]] = true;
    for (size_t i = 0; i < sizeof(A_1PIX_DATA) / sizeof(int); i++) a1pix[A_1PIX_DATA[i]] = true;
  }
};

const K3MTables TBL = K3MTables();

}  // namespace

// [[Rcpp::export(.k3m_cpp)]]
IntegerMatrix k3m_cpp(IntegerMatrix img, int max_iter) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  IntegerMatrix m = clone(img);
  IntegerMatrix border_mask(nrow, ncol);

  // Compute the paper's neighbour weight w(x,y) at (r, c) from the
  // current state of m.
  auto get_weight = [&](int r, int c) -> int {
    int p2 = m(r - 1, c);
    int p3 = m(r - 1, c + 1);
    int p4 = m(r,     c + 1);
    int p5 = m(r + 1, c + 1);
    int p6 = m(r + 1, c);
    int p7 = m(r + 1, c - 1);
    int p8 = m(r,     c - 1);
    int p9 = m(r - 1, c - 1);
    return p2 * 1 + p3 * 2 + p4 * 4 + p5 * 8
         + p6 * 16 + p7 * 32 + p8 * 64 + p9 * 128;
  };

  // Phases 0..6, iterated.
  for (int it = 0; it < max_iter; it++) {
    bool modified = false;

    // Phase 0: mark borders.
    std::fill(border_mask.begin(), border_mask.end(), 0);
    for (int r = 1; r < nrow - 1; r++) {
      for (int c = 1; c < ncol - 1; c++) {
        if (m(r, c) != 1) continue;
        if (TBL.a0[get_weight(r, c)]) border_mask(r, c) = 1;
      }
    }

    // Phases 1..5: sequential deletion of border pixels matching A_i.
    // The weight is recomputed against the current state at each
    // visit, so deletions earlier in the scan influence later
    // neighbour weights (this is the sequential character of the
    // algorithm, per Fig. 19 and Section 3 of the paper).
    for (int phase = 1; phase <= 5; phase++) {
      for (int r = 1; r < nrow - 1; r++) {
        for (int c = 1; c < ncol - 1; c++) {
          if (!border_mask(r, c)) continue;
          if (m(r, c) != 1) continue;
          if (TBL.a[phase][get_weight(r, c)]) {
            m(r, c) = 0;
            modified = true;
          }
        }
      }
    }

    // Phase 6 (unmark borders): implicit; next iteration's Phase 0
    // recomputes border_mask from scratch.
    if (!modified) break;
  }

  // Thinning to a one-pixel width skeleton (Section 3, p. 326).
  // Uses lookup table A_1pix; applied iteratively until no further
  // pixels can be deleted.
  for (int it = 0; it < max_iter; it++) {
    bool modified = false;
    for (int r = 1; r < nrow - 1; r++) {
      for (int c = 1; c < ncol - 1; c++) {
        if (m(r, c) != 1) continue;
        if (TBL.a1pix[get_weight(r, c)]) {
          m(r, c) = 0;
          modified = true;
        }
      }
    }
    if (!modified) break;
  }

  return m;
}
