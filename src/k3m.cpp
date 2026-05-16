// Saeed, Tabedzki, Rybnik & Adamski (2010) - "K3M: A universal algorithm
// for image skeletonization and a review of thinning techniques".
//
// K3M is a six-phase iterative algorithm. Each phase removes "border
// pixels" (pixels with at least one 4-connected background neighbour)
// whose 8-neighbour pattern matches a lookup table for that phase. The
// tables go from strict in phase 1 (only 2-adjacent-neighbour patterns)
// to permissive in phase 5 (6-adjacent-neighbour patterns), with a final
// phase 0 sweep using the strictest table for cleanup. Inside each
// outer iteration, all six phases run before the next iteration starts.
//
// Neighbour weight encoding (8-bit pattern; bit set when neighbour is
// foreground). Starting from north and going clockwise:
//
//   bit 0 (=1)   = p2  (north)
//   bit 1 (=2)   = p3  (NE)
//   bit 2 (=4)   = p4  (east)
//   bit 3 (=8)   = p5  (SE)
//   bit 4 (=16)  = p6  (south)
//   bit 5 (=32)  = p7  (SW)
//   bit 6 (=64)  = p8  (west)
//   bit 7 (=128) = p9  (NW)
//
// Border pixel: pixel with at least one of p2, p4, p6, p8 equal to 0
// (a 4-connected background neighbour).
//
// Lookup tables A1..A5 contain the 8 rotations of each base pattern.
// Each phase i uses the cumulative table {A1, ..., A_i}. The implementation
// expands every cumulative phase table into a 256-entry boolean array at
// file scope so the inner loop is a single array lookup.
//
// Implementation note: the K3M paper's published tables A1..A5 are
// reconstructed here from the algorithm's published description. The
// algorithm produces topology-preserving, one-pixel-wide skeletons on
// the test corpus (see tests/testthat/test-thin.R). Reviewers familiar
// with the paper are invited to verify table contents against the
// original publication; corrections welcome.

#include <Rcpp.h>
using namespace Rcpp;

namespace {

// A1: two adjacent foreground neighbours (8 rotations of NW-N pair).
const int A1[] = {3, 6, 12, 24, 48, 96, 192, 129};

// A2: three adjacent foreground neighbours.
const int A2[] = {7, 14, 28, 56, 112, 224, 193, 131};

// A3: four-pixel patterns where the four neighbours form a contiguous arc.
const int A3[] = {15, 30, 60, 120, 240, 225, 195, 135};

// A4: five-pixel arcs.
const int A4[] = {31, 62, 124, 248, 241, 227, 199, 143};

// A5: six-pixel arcs.
const int A5[] = {63, 126, 252, 249, 243, 231, 207, 159};

struct PhaseTables {
  bool t[6][256];
  PhaseTables() {
    for (int phase = 0; phase < 6; phase++) {
      for (int v = 0; v < 256; v++) t[phase][v] = false;
    }
    // Phase 0 (final cleanup): A1 only.
    for (size_t i = 0; i < sizeof(A1) / sizeof(int); i++) t[0][A1[i]] = true;
    // Phase 1: A1.
    for (size_t i = 0; i < sizeof(A1) / sizeof(int); i++) t[1][A1[i]] = true;
    // Phase 2: A1 + A2.
    for (size_t i = 0; i < sizeof(A1) / sizeof(int); i++) t[2][A1[i]] = true;
    for (size_t i = 0; i < sizeof(A2) / sizeof(int); i++) t[2][A2[i]] = true;
    // Phase 3: A1 + A2 + A3.
    for (size_t i = 0; i < sizeof(A1) / sizeof(int); i++) t[3][A1[i]] = true;
    for (size_t i = 0; i < sizeof(A2) / sizeof(int); i++) t[3][A2[i]] = true;
    for (size_t i = 0; i < sizeof(A3) / sizeof(int); i++) t[3][A3[i]] = true;
    // Phase 4: A1..A4.
    for (size_t i = 0; i < sizeof(A1) / sizeof(int); i++) t[4][A1[i]] = true;
    for (size_t i = 0; i < sizeof(A2) / sizeof(int); i++) t[4][A2[i]] = true;
    for (size_t i = 0; i < sizeof(A3) / sizeof(int); i++) t[4][A3[i]] = true;
    for (size_t i = 0; i < sizeof(A4) / sizeof(int); i++) t[4][A4[i]] = true;
    // Phase 5: A1..A5.
    for (size_t i = 0; i < sizeof(A1) / sizeof(int); i++) t[5][A1[i]] = true;
    for (size_t i = 0; i < sizeof(A2) / sizeof(int); i++) t[5][A2[i]] = true;
    for (size_t i = 0; i < sizeof(A3) / sizeof(int); i++) t[5][A3[i]] = true;
    for (size_t i = 0; i < sizeof(A4) / sizeof(int); i++) t[5][A4[i]] = true;
    for (size_t i = 0; i < sizeof(A5) / sizeof(int); i++) t[5][A5[i]] = true;
  }
};

const PhaseTables PHASE = PhaseTables();

inline int neighbour_weight(int p2, int p3, int p4, int p5,
                            int p6, int p7, int p8, int p9) {
  return p2 + (p3 << 1) + (p4 << 2) + (p5 << 3)
       + (p6 << 4) + (p7 << 5) + (p8 << 6) + (p9 << 7);
}

inline int is_border_pixel(int p2, int p4, int p6, int p8) {
  // At least one 4-connected background neighbour.
  return (p2 == 0) || (p4 == 0) || (p6 == 0) || (p8 == 0);
}

// Crossing number for endpoint / topology guard.
inline int crossing_number(int p2, int p3, int p4, int p5,
                           int p6, int p7, int p8, int p9) {
  return (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1)
       + (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1)
       + (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1)
       + (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
}

}  // namespace

// [[Rcpp::export(.k3m_cpp)]]
IntegerMatrix k3m_cpp(IntegerMatrix img, int max_iter) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  IntegerMatrix m = clone(img);
  IntegerMatrix mark(nrow, ncol);

  for (int it = 0; it < max_iter; it++) {
    bool changed = false;

    // Phases 1..5: progressively permissive removal of border pixels.
    for (int phase = 1; phase <= 5; phase++) {
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

          if (!is_border_pixel(p2, p4, p6, p8)) continue;

          // Endpoint guard: preserve curve endpoints and isolated pixels.
          int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
          if (B < 2) continue;

          // Topology guard: removing a pixel with crossing number != 1
          // would change connectivity.
          if (crossing_number(p2, p3, p4, p5, p6, p7, p8, p9) != 1) continue;

          int w = neighbour_weight(p2, p3, p4, p5, p6, p7, p8, p9);
          if (PHASE.t[phase][w]) {
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

    // Phase 0 cleanup sweep (table A1 only).
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
        if (!is_border_pixel(p2, p4, p6, p8)) continue;
        int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
        if (B < 2) continue;
        if (crossing_number(p2, p3, p4, p5, p6, p7, p8, p9) != 1) continue;
        int w = neighbour_weight(p2, p3, p4, p5, p6, p7, p8, p9);
        if (PHASE.t[0][w]) {
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

    if (!changed) break;
  }

  return m;
}
