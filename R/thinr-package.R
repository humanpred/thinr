#' thinr: Binary Image Thinning Algorithms
#'
#' Thinning (also called skeletonization) reduces a binary image of a
#' shape to a one-pixel-wide centerline that preserves the shape's
#' topology. `thinr` provides nine thinning algorithms behind a
#' single dispatching function, plus the medial axis transform and a
#' fast distance transform.
#'
#' @section Thinning algorithms (`thin(method = ...)`):
#'
#' - `zhang_suen` — Zhang & Suen (1984). Default; matches
#'   `EBImage::thinImage`.
#' - `guo_hall` — Guo & Hall (1989). Often better corner preservation
#'   on diagonal features.
#' - `lee` — Lee, Kashyap & Chu (1994), 2-D adaptation. Four
#'   directional sub-iterations.
#' - `k3m` — Saeed et al. (2010). Six-phase lookup-table thinning.
#' - `hilditch` — Hilditch (1969). Single-pass parallel thinning with
#'   look-ahead crossing-number check.
#' - `stentiford` — Stentiford & Mortimer (1983). Four directional
#'   3-pixel templates per pass.
#' - `pavlidis` — Pavlidis (1980). Directional thinning with
#'   restrictive interior preservation (`B(P) <= 5`).
#' - `opta` — Naccache & Shinghal (1984). One-pass thinning derived
#'   from Hilditch.
#' - `holt` — Holt et al. (1987). Zhang-Suen variant that preserves
#'   isolated 2x2 blocks.
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
