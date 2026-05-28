// Distance transforms on binary images.
//
// Three metrics are supported:
//
//   * "euclidean"  — exact squared Euclidean distance computed via
//                    Felzenszwalb & Huttenlocher (2012), then sqrt'd
//                    to produce L2 distance. <doi:10.4086/toc.2012.v008a019>
//   * "manhattan"  — L1 distance via the two-pass forward + backward
//                    sweep (Rosenfeld & Pfaltz 1968).
//   * "chessboard" — L_infinity (Chebyshev) distance via the same
//                    two-pass sweep with 8-connected propagation.
//
// All three compute distance from each foreground pixel to the
// nearest background pixel. Background pixels have distance 0.

#include <Rcpp.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>
#include "distance_transform.h"

using namespace Rcpp;

namespace {

// 1D squared-distance transform of a function sampled on an integer grid.
// Felzenszwalb & Huttenlocher (2012), Theory of Computing 8(19):415-428.
//
// f: input values at indices 0..n-1.
// d: output squared distances, same length.
//
// At index q the algorithm computes
//   d(q) = min_p { (q - p)^2 + f(p) }.
//
// Total time O(n) via the lower envelope of parabolas y = (x-p)^2 + f(p).
//
// Parabolas with f(p) = +infinity are skipped entirely - they never
// contribute to the lower envelope. If every parabola is at infinity,
// the output is all infinity (no finite source). This skip-on-infinity
// handling matters because the 2-D DT uses f = 0 for source pixels and
// f = infinity for non-source pixels, and naive arithmetic of
// inf - inf produces NaN which corrupts the intersection comparisons.
void dt_1d(const std::vector<double>& f, std::vector<double>& d) {
  const double inf = std::numeric_limits<double>::infinity();
  int n = static_cast<int>(f.size());
  d.assign(n, inf);

  // Locate the first finite parabola; if none, output is all infinity.
  int start = 0;
  while (start < n && !std::isfinite(f[start])) start++;
  if (start == n) return;

  std::vector<int> v(n);
  std::vector<double> z(n + 1);
  int k = 0;
  v[0] = start;
  z[0] = -inf;
  z[1] = inf;

  for (int q = start + 1; q < n; q++) {
    if (!std::isfinite(f[q])) continue;
    double s;
    while (true) {
      double dv = static_cast<double>(q - v[k]);
      s = ((f[q] + static_cast<double>(q) * q)
           - (f[v[k]] + static_cast<double>(v[k]) * v[k]))
          / (2.0 * dv);
      if (s <= z[k] && k > 0) {
        k--;
      } else {
        break;
      }
    }
    k++;
    v[k] = q;
    z[k] = s;
    z[k + 1] = inf;
  }

  k = 0;
  for (int q = 0; q < n; q++) {
    while (z[k + 1] < q) k++;
    double dv = static_cast<double>(q - v[k]);
    d[q] = dv * dv + f[v[k]];
  }
}

}  // namespace

namespace thinr {

NumericMatrix squared_euclidean_dt(const NumericMatrix& f) {
  int nrow = f.nrow();
  int ncol = f.ncol();
  NumericMatrix dt(nrow, ncol);
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) dt(r, c) = f(r, c);
  }

  // Row pass.
  std::vector<double> rowf(ncol), rowd(ncol);
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) rowf[c] = dt(r, c);
    dt_1d(rowf, rowd);
    for (int c = 0; c < ncol; c++) dt(r, c) = rowd[c];
  }

  // Column pass.
  std::vector<double> colf(nrow), cold(nrow);
  for (int c = 0; c < ncol; c++) {
    for (int r = 0; r < nrow; r++) colf[r] = dt(r, c);
    dt_1d(colf, cold);
    for (int r = 0; r < nrow; r++) dt(r, c) = cold[r];
  }

  return dt;
}

}  // namespace thinr

namespace {

// L1 (Manhattan) two-pass forward + backward sweep.
NumericMatrix dt_manhattan(IntegerMatrix img) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  const double big = static_cast<double>(nrow) + static_cast<double>(ncol) + 1.0;
  NumericMatrix d(nrow, ncol);

  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      d(r, c) = (img(r, c) == 0) ? 0.0 : big;
    }
  }

  // Forward sweep: top-left to bottom-right; neighbours above and left.
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      double here = d(r, c);
      if (here == 0.0) continue;
      if (r > 0) here = std::min(here, d(r - 1, c) + 1.0);
      if (c > 0) here = std::min(here, d(r, c - 1) + 1.0);
      d(r, c) = here;
    }
  }

  // Backward sweep: bottom-right to top-left; neighbours below and right.
  for (int r = nrow - 1; r >= 0; r--) {
    for (int c = ncol - 1; c >= 0; c--) {
      double here = d(r, c);
      if (here == 0.0) continue;
      if (r < nrow - 1) here = std::min(here, d(r + 1, c) + 1.0);
      if (c < ncol - 1) here = std::min(here, d(r, c + 1) + 1.0);
      d(r, c) = here;
    }
  }

  return d;
}

// L_infinity (Chebyshev / chessboard) two-pass sweep with 8-connected
// propagation.
NumericMatrix dt_chessboard(IntegerMatrix img) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  const double big = static_cast<double>(std::max(nrow, ncol)) + 1.0;
  NumericMatrix d(nrow, ncol);

  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      d(r, c) = (img(r, c) == 0) ? 0.0 : big;
    }
  }

  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      double here = d(r, c);
      if (here == 0.0) continue;
      if (r > 0) {
        here = std::min(here, d(r - 1, c) + 1.0);
        if (c > 0)        here = std::min(here, d(r - 1, c - 1) + 1.0);
        if (c < ncol - 1) here = std::min(here, d(r - 1, c + 1) + 1.0);
      }
      if (c > 0)          here = std::min(here, d(r, c - 1) + 1.0);
      d(r, c) = here;
    }
  }

  for (int r = nrow - 1; r >= 0; r--) {
    for (int c = ncol - 1; c >= 0; c--) {
      double here = d(r, c);
      if (here == 0.0) continue;
      if (r < nrow - 1) {
        here = std::min(here, d(r + 1, c) + 1.0);
        if (c > 0)        here = std::min(here, d(r + 1, c - 1) + 1.0);
        if (c < ncol - 1) here = std::min(here, d(r + 1, c + 1) + 1.0);
      }
      if (c < ncol - 1)   here = std::min(here, d(r, c + 1) + 1.0);
      d(r, c) = here;
    }
  }

  return d;
}

NumericMatrix dt_euclidean(IntegerMatrix img) {
  int nrow = img.nrow();
  int ncol = img.ncol();
  const double inf = std::numeric_limits<double>::infinity();
  NumericMatrix f(nrow, ncol);
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      f(r, c) = (img(r, c) == 0) ? 0.0 : inf;
    }
  }
  NumericMatrix sq = thinr::squared_euclidean_dt(f);
  NumericMatrix out(nrow, ncol);
  for (int r = 0; r < nrow; r++) {
    for (int c = 0; c < ncol; c++) {
      out(r, c) = std::sqrt(sq(r, c));
    }
  }
  return out;
}

}  // namespace

// [[Rcpp::export(.distance_transform_cpp)]]
NumericMatrix distance_transform_cpp(IntegerMatrix img, int metric) {
  switch (metric) {
    case 0:  return dt_euclidean(img);
    case 1:  return dt_manhattan(img);
    case 2:  return dt_chessboard(img);
    default: Rcpp::stop("Unknown metric code passed to distance_transform_cpp.");
  }
  return NumericMatrix(0, 0);  // unreachable
}
