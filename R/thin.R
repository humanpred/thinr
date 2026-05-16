#' Thin (skeletonize) a binary image
#'
#' Reduce a binary image to its one-pixel-wide skeleton using one of the
#' supported thinning algorithms.
#'
#' @param image A binary image: a matrix or array where non-zero values
#'   are foreground and zero values are background. Logical, integer, and
#'   numeric inputs are all accepted. The image is treated as a 2-D
#'   matrix; arrays with more than two dimensions are not yet supported.
#' @param method Algorithm to use. One of `"zhang_suen"` (default,
#'   matches `EBImage::thinImage`), `"guo_hall"`, `"lee"` (2-D
#'   adaptation of Lee, Kashyap & Chu 1994), or `"k3m"` (Saeed et al.
#'   2010). See `vignette("choosing-a-method")` for guidance on which to
#'   pick.
#' @param max_iter Maximum number of passes. Default 1000. Real binary
#'   images of typical sizes converge well under 50 passes; the limit is
#'   a safety bound against pathological inputs.
#'
#' @return A matrix of the same shape and storage mode as `image`, with
#'   foreground pixels marking the thinned skeleton and the rest set to
#'   background.
#'
#' @examples
#' # A 3x3 solid square thins to a single foreground pixel.
#' m <- matrix(c(0, 0, 0, 0, 0,
#'               0, 1, 1, 1, 0,
#'               0, 1, 1, 1, 0,
#'               0, 1, 1, 1, 0,
#'               0, 0, 0, 0, 0),
#'             nrow = 5, byrow = TRUE)
#' thin(m, method = "zhang_suen")
#' thin(m, method = "guo_hall")
#'
#' @export
thin <- function(image, method = c("zhang_suen", "guo_hall", "lee", "k3m"),
                 max_iter = 1000L) {
  method <- match.arg(method)
  mat <- as_binary_matrix(image)
  out <- switch(method,
    zhang_suen = .zhang_suen_cpp(mat, as.integer(max_iter)),
    guo_hall   = .guo_hall_cpp(mat,   as.integer(max_iter)),
    lee        = .lee_cpp(mat,        as.integer(max_iter)),
    k3m        = .k3m_cpp(mat,        as.integer(max_iter))
  )
  restore_storage(out, image)
}

# Convert any binary-image-shaped input to an IntegerMatrix where
# foreground is 1 and background is 0.
as_binary_matrix <- function(image) {
  if (is.matrix(image)) {
    if (is.logical(image)) {
      return(matrix(as.integer(image), nrow = nrow(image), ncol = ncol(image)))
    }
    if (is.integer(image)) {
      out <- image
      storage.mode(out) <- "integer"
      out[out != 0L] <- 1L
      return(out)
    }
    if (is.numeric(image)) {
      out <- matrix(as.integer(image != 0), nrow = nrow(image), ncol = ncol(image))
      return(out)
    }
    stop("thinr::thin() does not know how to interpret a matrix of mode '",
         storage.mode(image), "'.")
  }
  if (is.array(image) && length(dim(image)) == 2L) {
    return(as_binary_matrix(matrix(image, nrow = dim(image)[1], ncol = dim(image)[2])))
  }
  stop("thinr::thin() expects a 2-D matrix. ",
       "Higher-dimensional arrays are not yet supported.")
}

# Coerce the integer skeleton back into the storage mode of the user's
# original input so the return type is unsurprising.
restore_storage <- function(skeleton, original) {
  if (is.logical(original)) {
    return(matrix(as.logical(skeleton), nrow = nrow(skeleton), ncol = ncol(skeleton)))
  }
  if (is.numeric(original) && !is.integer(original)) {
    return(matrix(as.numeric(skeleton), nrow = nrow(skeleton), ncol = ncol(skeleton)))
  }
  skeleton
}
