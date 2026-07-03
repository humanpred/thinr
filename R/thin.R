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
#'   adaptation of Lee, Kashyap & Chu 1994), `"k3m"` (Saeed et al.
#'   2010), `"hilditch"` (Hilditch 1969), `"opta"` (Naccache &
#'   Shinghal 1984), or `"holt"` (Holt et al. 1987).
#'   See `vignette("choosing-a-method")` for guidance on which to pick.
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
#' thin(m, method = "hilditch")
#'
#' @export
thin <- function(image,
                 method = c("zhang_suen", "guo_hall", "lee", "k3m",
                            "hilditch", "opta", "holt"),
                 max_iter = 1000L) {
  method <- match.arg(method)
  mat <- as_binary_matrix(image)
  # The C++ kernels examine an 8-neighbourhood and therefore never
  # delete pixels in the outermost row/column. Pad with a one-pixel
  # background border so shapes touching the matrix edge are thinned
  # like interior shapes, then crop back to the original extent.
  rows <- seq_len(nrow(mat)) + 1L
  cols <- seq_len(ncol(mat)) + 1L
  padded <- matrix(0L, nrow = nrow(mat) + 2L, ncol = ncol(mat) + 2L)
  padded[rows, cols] <- mat
  iter <- as.integer(max_iter)
  out <- switch(method,
    zhang_suen = .zhang_suen_cpp(padded, iter),
    guo_hall   = .guo_hall_cpp(padded,   iter),
    lee        = .lee_cpp(padded,        iter),
    k3m        = .k3m_cpp(padded,        iter),
    hilditch   = .hilditch_cpp(padded,   iter),
    opta       = .opta_cpp(padded,       iter),
    holt       = .holt_cpp(padded,       iter)
  )
  out <- out[rows, cols, drop = FALSE]
  dimnames(out) <- dimnames(mat)
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
    # nocov start
    # Fail-fast assertion (unreachable): the package requires R (>= 4.2),
    # where every object carrying a length-2 `dim` attribute also satisfies
    # is.matrix() -- so any 2-D array is already handled by the is.matrix()
    # branch above and control cannot arrive here. If it ever does, base R's
    # array/matrix invariant has changed underneath us; raise loudly rather
    # than silently recursing so the broken assumption is caught, not masked.
    stop("thinr internal invariant violated: a length-2 `dim` object ",
         "reached the array branch without satisfying is.matrix(). ",
         "This contradicts base R (>= 4.2) array/matrix semantics.")
    # nocov end
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
