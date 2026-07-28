// Holt, Stewart, Clint & Perrott (1987), "An improved parallel
// thinning algorithm", Communications of the ACM 30(2):156-160.
// doi:10.1145/12527.12531
//
// Single-subiteration parallel algorithm using edge information about
// neighbours. The survival expression (Section 2, p. 157) is:
//
//   survive(C) = vC AND (~edgeC OR
//                        (edgeE AND vN AND vS) OR
//                        (edgeS AND vW AND vE) OR
//                        (edgeE AND edgeSE AND edgeS))
//
// where vX is the foreground value at position X and edgeX is the
// edge() function evaluated at X. An element is REMOVED iff survive
// is false; for a foreground element this becomes:
//
//   remove(C) = edgeC AND (~edgeE OR ~vN OR ~vS)
//                     AND (~edgeS OR ~vW OR ~vE)
//                     AND (~edgeE OR ~edgeSE OR ~edgeS)
//
// edge() (Appendix A) is the full simple-point check: a pixel is on
// the edge iff its 8-neighbourhood has B(p) in [2, 6] foreground
// neighbours AND A(p) = 1 (exactly one 0->1 transition in the cyclic
// neighbour sequence). This is the same condition Zhang-Suen uses for
// candidate selection.
//
// Implementation note: the Lam-Lee-Suen (1992) survey on page 877
// transcribes Holt's middle clause as "edgeS AND vW AND vN" (with N
// for the third term), but the original paper (CACM 30(2) p. 157) has
// "edgeS AND vW AND vE" (with E). The original paper is followed
// here. Computing edge() at the E, S, and SE neighbours requires
// reading a 5x5 window around C; out-of-bounds reads are treated as
// background.

#include <Rcpp.h>
#include "thinr_common.h"
using namespace Rcpp;

namespace {

// Holt's edge() function (Appendix A): foreground simple point with
// B(p) in [2, 6] and A(p) = 1.
inline bool holt_edge(const IntegerMatrix& m, int r, int c,
                      int nrow, int ncol) {
  if (r < 0 || r >= nrow || c < 0 || c >= ncol) return false;
  if (m(r, c) != 1) return false;

  int p2 = (r > 0)                          ? m(r - 1, c    ) : 0;  // N
  int p3 = (r > 0 && c < ncol - 1)          ? m(r - 1, c + 1) : 0;  // NE
  int p4 = (c < ncol - 1)                   ? m(r,     c + 1) : 0;  // E
  int p5 = (r < nrow - 1 && c < ncol - 1)   ? m(r + 1, c + 1) : 0;  // SE
  int p6 = (r < nrow - 1)                   ? m(r + 1, c    ) : 0;  // S
  int p7 = (r < nrow - 1 && c > 0)          ? m(r + 1, c - 1) : 0;  // SW
  int p8 = (c > 0)                          ? m(r,     c - 1) : 0;  // W
  int p9 = (r > 0 && c > 0)                 ? m(r - 1, c - 1) : 0;  // NW

  int B = thinr::neighbour_count(p2, p3, p4, p5, p6, p7, p8, p9);
  if (B < 2 || B > 6) return false;

  int A = thinr::crossing_number(p2, p3, p4, p5, p6, p7, p8, p9);
  return A == 1;
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
    std::fill(mark.begin(), mark.end(), 0);

    for (int r = 1; r < nrow - 1; r++) {
      for (int c = 1; c < ncol - 1; c++) {
        if (m(r, c) != 1) continue;
        if (!holt_edge(m, r, c, nrow, ncol)) continue;

        int p2 = m(r - 1, c);  // vN
        int p4 = m(r,     c + 1);  // vE
        int p6 = m(r + 1, c);  // vS
        int p8 = m(r,     c - 1);  // vW

        bool edgeE  = holt_edge(m, r,     c + 1, nrow, ncol);
        bool edgeS  = holt_edge(m, r + 1, c,     nrow, ncol);
        bool edgeSE = holt_edge(m, r + 1, c + 1, nrow, ncol);

        // remove(C) = edgeC AND
        //             (~edgeE OR ~vN OR ~vS) AND        [first clause]
        //             (~edgeS OR ~vW OR ~vE) AND        [second clause - vE per paper]
        //             (~edgeE OR ~edgeSE OR ~edgeS)     [third clause]
        bool a = !edgeE || (p2 == 0) || (p6 == 0);
        bool b = !edgeS || (p8 == 0) || (p4 == 0);
        bool c_ = !edgeE || !edgeSE || !edgeS;

        if (a && b && c_) mark(r, c) = 1;
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
