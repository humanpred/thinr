// Saeed et al. (2010), "K3M: A universal algorithm for image
// skeletonization and a review of thinning techniques". Six-phase
// lookup-table-driven thinning with strong corner preservation.
//
// v0.1: stub that errors with a clear message. v0.2 ships the
// implementation. See NEWS.md and the choosing-a-method vignette.

#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export(.k3m_cpp)]]
IntegerMatrix k3m_cpp(IntegerMatrix img, int max_iter) {
  (void) img;
  (void) max_iter;
  stop("K3M is not yet implemented in thinr v0.1; planned for v0.2. "
       "Use method = 'zhang_suen' or 'guo_hall' for now.");
  return IntegerMatrix(0, 0);  // unreachable
}
