#' Thin a binary image with the default Zhang-Suen algorithm
#'
#' A convenience wrapper for `thin(x, method = "zhang_suen")`, provided
#' under a short single-argument name for the common case. Use [thin()]
#' directly to select any of the other algorithms (Guo-Hall, Lee, K3M,
#' Hilditch, OPTA, Holt).
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
