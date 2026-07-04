# Changelog

## thinr (development version)

Correctness fixes from the figureextract ecosystem review (2026-07-03).
All are boundary conditions the published algorithms assume away; see
the new
[`vignette("correctness-properties")`](https://humanpred.github.io/thinr/articles/correctness-properties.md)
for the guarantees they restore.

- [`thin()`](https://humanpred.github.io/thinr/reference/thin.md) now
  skeletonises shapes that touch the matrix border correctly. The
  kernels inspect an 8-neighbourhood and so never deleted pixels in the
  outermost row or column, leaving edge-touching shapes two to three
  pixels thick;
  [`thin()`](https://humanpred.github.io/thinr/reference/thin.md) now
  pads the image with a one-pixel background margin and crops it back,
  so a shape thins identically whether or not it touches the frame.
  Applies to all seven methods (#F012).

- `thin(method = "opta")` now keeps two-pixel-wide strokes connected.
  The SPTA kernel evaluated all four contour directions against the
  pre-cycle snapshot and deleted them in one batch, so both sides of a
  2px stroke were removed together and the skeleton fragmented (a 2×9
  bar collapsed to its four corner pixels). Deletion is now sequential
  per direction, as Naccache & Shinghal’s two-scan formulation requires
  (#F011).

- `distance_transform(metric = "manhattan" | "chessboard")` now returns
  `Inf` for a fully-foreground image (no background pixel exists),
  matching the `"euclidean"` metric and `EBImage::distmap()`. The
  two-pass metrics previously seeded foreground with a finite sentinel
  that leaked out as a plausible-looking but meaningless finite distance
  (#F015).

- [`thin()`](https://humanpred.github.io/thinr/reference/thin.md),
  [`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md),
  and
  [`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md)
  now reject `NA` input with a clear error at the coercion boundary
  instead of silently turning it into `NA_integer_` (`INT_MIN`) in the
  C++ kernels (#F016).

- Added a connectivity-preservation property test across all seven
  methods and tightened the Holt straight-line test (#F013).

- Removed `thinImage()`. Use
  [`thin()`](https://humanpred.github.io/thinr/reference/thin.md)
  (Zhang-Suen is the default method).

## thinr 0.2.0

CRAN release: 2026-05-27

Initial CRAN release.
