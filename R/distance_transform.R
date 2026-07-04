#' Distance transform of a binary image
#'
#' Compute the distance from each foreground pixel to the nearest
#' background pixel, under one of three standard metrics.
#'
#' @param image A binary image: a matrix where non-zero values are
#'   foreground and zero values are background. Logical, integer, and
#'   numeric inputs are accepted. `NA` values are rejected with an error
#'   rather than silently coerced.
#' @param metric Distance metric. One of:
#'   * `"euclidean"` (default) — exact L2 distance, via
#'     Felzenszwalb & Huttenlocher (2012) linear-time separable
#'     algorithm.
#'   * `"manhattan"` — L1 distance via two-pass forward + backward
#'     sweep (Rosenfeld & Pfaltz 1968).
#'   * `"chessboard"` — L_infinity (Chebyshev) distance via the same
#'     two-pass sweep with 8-connected propagation.
#'
#' @return A numeric matrix of the same shape as `image`. Background
#'   pixels are 0; foreground pixels carry their distance to the
#'   nearest background pixel. When the image is entirely foreground
#'   (no background pixel exists) every pixel is `Inf`, consistently
#'   across all three metrics — the distance to a non-existent nearest
#'   background pixel is unbounded, matching `EBImage::distmap()`. Only
#'   pixels count as background; the region outside the matrix is not
#'   treated as an implicit background border.
#'
#' @examples
#' # A 5x5 image with a single background pixel in the corner.
#' m <- matrix(1L, nrow = 5, ncol = 5)
#' m[1, 1] <- 0L
#' distance_transform(m, metric = "manhattan")
#' distance_transform(m, metric = "chessboard")
#' round(distance_transform(m, metric = "euclidean"), 3)
#'
#' @references
#' Felzenszwalb, P. F., & Huttenlocher, D. P. (2012). Distance
#' transforms of sampled functions. *Theory of Computing*, 8(19),
#' 415-428. \doi{10.4086/toc.2012.v008a019}
#'
#' Rosenfeld, A., & Pfaltz, J. L. (1968). Distance functions on
#' digital pictures. *Pattern Recognition*, 1(1), 33-61.
#' \doi{10.1016/0031-3203(68)90013-7}
#'
#' @export
distance_transform <- function(image,
                               metric = c("euclidean", "manhattan",
                                          "chessboard")) {
  metric <- match.arg(metric)
  mat <- as_binary_matrix(image)
  code <- switch(metric, euclidean = 0L, manhattan = 1L, chessboard = 2L)
  .distance_transform_cpp(mat, code)
}
