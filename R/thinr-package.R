#' thinr: Binary Image Thinning Algorithms
#'
#' Thinning (also called skeletonization) reduces a binary image of a
#' shape to a one-pixel-wide centerline that preserves the shape's
#' topology. `thinr` provides multiple thinning algorithms behind a
#' single dispatching function.
#'
#' @section Algorithms:
#'
#' - [`zhang_suen`][thin] — Zhang & Suen (1984). Fast, well-known,
#'   matches the algorithm in `EBImage::thinImage`. Default.
#' - [`guo_hall`][thin] — Guo & Hall (1989). Often better corner
#'   preservation than Zhang-Suen on diagonal features.
#' - `lee` — Lee (1994), 3D-capable. Planned for v0.2.
#' - `k3m` — Saeed et al. (2010). Planned for v0.2.
#'
#' @section Drop-in compatibility:
#'
#' [thinImage()] matches the signature of `EBImage::thinImage()`. Code
#' that uses `EBImage::thinImage` can switch to `thinr::thinImage` with
#' no other changes.
#'
#' @section Choosing an algorithm:
#'
#' See `vignette("choosing-a-method", package = "thinr")` for guidance
#' on which algorithm to pick for which kind of image.
#'
#' @keywords internal
#' @useDynLib thinr, .registration = TRUE
#' @importFrom Rcpp sourceCpp
"_PACKAGE"
