#' thinr: Binary Image Thinning Algorithms
#'
#' Thinning (also called skeletonization) reduces a binary image of a
#' shape to a one-pixel-wide centerline that preserves the shape's
#' topology. `thinr` provides seven thinning algorithms behind a
#' single dispatching function, plus the medial axis transform and a
#' fast distance transform.
#'
#' @section Thinning algorithms (`thin(method = ...)`):
#'
#' - `zhang_suen` — Zhang & Suen (1984)
#'   \doi{10.1145/357994.358023}. Default; matches
#'   `EBImage::thinImage`.
#' - `guo_hall` — Guo & Hall (1989) \doi{10.1145/62065.62074}. Often
#'   better corner preservation on diagonal features.
#' - `lee` — Lee, Kashyap & Chu (1994) \doi{10.1006/cgip.1994.1042},
#'   2-D adaptation. Four directional sub-iterations.
#' - `k3m` — Saeed, Tabędzki, Rybnik & Adamski (2010)
#'   \doi{10.2478/v10006-010-0024-4}. Six-phase lookup-table thinning.
#' - `hilditch` — parallel form commonly attributed to Hilditch
#'   (1969, in *Machine Intelligence 4*). Single-pass thinning with
#'   look-ahead crossing-number check.
#' - `opta` — Naccache & Shinghal (1984), "An investigation into the
#'   skeletonization approach of Hilditch", *Pattern Recognition*
#'   17(3):279-284. One-pass safe-point thinning (SPTA).
#' - `holt` — Holt, Stewart, Clint & Perrott (1987)
#'   \doi{10.1145/12527.12531}. One-subcycle parallel thinning with
#'   edge information about neighbours.
#'
#' See [thin()] and `vignette("choosing-a-method")` for guidance.
#'
#' @section Medial axis and distance transform:
#'
#' - [medial_axis()] — Medial axis transform (Blum 1967): the locus
#'   of foreground pixels that are ridge points of the distance
#'   transform. Optionally returns the per-pixel distance.
#' - [distance_transform()] — Euclidean (Felzenszwalb-Huttenlocher
#'   2012, linear-time separable), Manhattan, or Chessboard distance
#'   from each foreground pixel to the nearest background pixel.
#'
#' @section Drop-in compatibility:
#'
#' [thinImage()] matches the signature of `EBImage::thinImage()`. Code
#' that uses `EBImage::thinImage` can switch to `thinr::thinImage` with
#' no other changes.
#'
#' @keywords internal
#' @useDynLib thinr, .registration = TRUE
#' @importFrom Rcpp sourceCpp
"_PACKAGE"
