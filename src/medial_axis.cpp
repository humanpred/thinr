// Medial axis transform.
//
// Returns the locus of "ridge points" of the squared Euclidean
// distance transform: foreground pixels that are local maxima of the
// distance transform along at least one of the four principal
// directions (E-W, N-S, NE-SW, NW-SE). The result is a binary
// skeleton; the per-pixel distance to the nearest background is
// available alongside for callers that need width information.
//
// Distinction from `thin()`: thinning algorithms reduce a shape to a
// 1-pixel-wide topology-preserving skeleton. `medial_axis` returns
// the **medial axis** (Blum 1967) — the set of points equidistant
// from at least two boundary points, with the value at each point
// equal to the distance to the boundary. The two produce different
// outputs in general; medial axis carries thickness information that
// classical thinning discards.

#include <Rcpp.h>
#include <cmath>
#include <limits>
#include "distance_transform.h"

using namespace Rcpp;

// [[Rcpp::export(.medial_axis_cpp)]]
List medial_axis_cpp(IntegerMatrix img) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  const double inf = std::numeric_limits<double>::infinity();

  // Build the FH-2012 source function: 0 at background, infinity at
  // foreground. The squared-Euclidean DT then carries distance from
  // each foreground pixel to the nearest background.
  NumericMatrix f(nrow, ncol);
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      f(r, c) = (img(r, c) == 0) ? 0.0 : inf;
    }
  }
  NumericMatrix sq = thinr::squared_euclidean_dt(f);

  // Identify ridge pixels: foreground pixels that are a strict local
  // maximum of the squared distance transform along at least one of
  // the four principal directions (horizontal, vertical, NW-SE,
  // NE-SW). "Strict" along a direction means the pixel's value is
  // strictly greater than at least one neighbour in that direction
  // AND no less than the other neighbour — this excludes flat
  // interior pixels (where both opposite neighbours equal the pixel)
  // while keeping the medial line of a uniform-width region.
  IntegerMatrix skel(nrow, ncol);
  auto val = [&](int rr, int cc) -> double {
    if (rr < 0 || rr >= nrow || cc < 0 || cc >= ncol) return -1.0;
    return sq(rr, cc);
  };
  auto is_strict_ridge = [&](double v, double a, double b) -> bool {
    return v >= a && v >= b && (v > a || v > b);
  };
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      if (img(r, c) == 0) continue;
      double v = sq(r, c);
      bool ridge =
           is_strict_ridge(v, val(r,     c - 1), val(r,     c + 1))
        || is_strict_ridge(v, val(r - 1, c    ), val(r + 1, c    ))
        || is_strict_ridge(v, val(r - 1, c - 1), val(r + 1, c + 1))
        || is_strict_ridge(v, val(r - 1, c + 1), val(r + 1, c - 1));
      if (ridge) skel(r, c) = 1;
    }
  }

  // Euclidean distance (sqrt of squared) for the caller's convenience.
  NumericMatrix dist(nrow, ncol);
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      dist(r, c) = std::sqrt(sq(r, c));
    }
  }

  return List::create(Named("skeleton") = skel,
                      Named("distance") = dist);
}
