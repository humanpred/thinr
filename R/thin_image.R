#' Drop-in replacement for `EBImage::thinImage`
#'
#' Applies Zhang-Suen thinning to a binary image. Provided as a
#' signature-compatible alternative to `EBImage::thinImage()` so callers
#' can switch from `EBImage` to `thinr` by changing the namespace prefix
#' only.
#'
#' For access to the other algorithms (Guo-Hall, and eventually Lee /
#' K3M), use [thin()].
#'
#' @param x A binary image. Same constraints as [thin()]'s `image`
#'   argument.
#'
#' @return The thinned skeleton in the same storage mode as `x`.
#'
#' @examples
#' m <- matrix(c(0, 1, 1, 1, 0,
#'               0, 1, 1, 1, 0,
#'               0, 1, 1, 1, 0),
#'             nrow = 3, byrow = TRUE)
#' thinImage(m)
#'
#' @export
thinImage <- function(x) {
  thin(x, method = "zhang_suen")
}
