// Lee (1994), "Building Skeleton Models via 3-D Medial Surface Axis
// Thinning Algorithms". The full Lee algorithm uses Euler-invariance
// look-up tables and treats 2D as a special case of 3D thinning.
//
// v0.1: stub that errors with a clear message. v0.2 ships the
// implementation. See NEWS.md and the choosing-a-method vignette.

#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export(.lee_cpp)]]
IntegerMatrix lee_cpp(IntegerMatrix img, int max_iter) {
  (void) img;
  (void) max_iter;
  stop("Lee's algorithm is not yet implemented in thinr v0.1; planned "
       "for v0.2. Use method = 'zhang_suen' or 'guo_hall' for now.");
  return IntegerMatrix(0, 0);  // unreachable; silences compiler warning
}
