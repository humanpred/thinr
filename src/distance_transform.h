// Squared Euclidean distance transform — shared declaration so
// medial_axis can reuse the implementation without exporting an
// internal-only helper through Rcpp.

#ifndef THINR_DISTANCE_TRANSFORM_H
#define THINR_DISTANCE_TRANSFORM_H

#include <Rcpp.h>

namespace thinr {

// Squared Euclidean distance transform via Felzenszwalb & Huttenlocher
// (2012). `f` carries the input source values per pixel: 0 for source
// pixels, R_PosInf for non-source pixels. The output is the squared
// distance from each pixel to the nearest source pixel.
Rcpp::NumericMatrix squared_euclidean_dt(const Rcpp::NumericMatrix& f);

}  // namespace thinr

#endif  // THINR_DISTANCE_TRANSFORM_H
