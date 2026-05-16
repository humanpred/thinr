# thinr: Binary Image Thinning Algorithms

Thinning (also called skeletonization) reduces a binary image of a shape
to a one-pixel-wide centerline that preserves the shape's topology.
`thinr` provides multiple thinning algorithms behind a single
dispatching function.

## Algorithms

- [`zhang_suen`](https://humanpred.github.io/thinr/reference/thin.md) —
  Zhang & Suen (1984). Fast, well-known, matches the algorithm in
  `EBImage::thinImage`. Default.

- [`guo_hall`](https://humanpred.github.io/thinr/reference/thin.md) —
  Guo & Hall (1989). Often better corner preservation than Zhang-Suen on
  diagonal features.

- [`lee`](https://humanpred.github.io/thinr/reference/thin.md) — Lee,
  Kashyap & Chu (1994), 2-D adaptation. Four directional sub-iterations
  with crossing-number Euler-invariance.

- [`k3m`](https://humanpred.github.io/thinr/reference/thin.md) — Saeed
  et al. (2010). Six-phase lookup-table thinning; strong corner
  preservation.

## Drop-in compatibility

[`thinImage()`](https://humanpred.github.io/thinr/reference/thinImage.md)
matches the signature of `EBImage::thinImage()`. Code that uses
`EBImage::thinImage` can switch to
[`thinr::thinImage`](https://humanpred.github.io/thinr/reference/thinImage.md)
with no other changes.

## Choosing an algorithm

See
[`vignette("choosing-a-method", package = "thinr")`](https://humanpred.github.io/thinr/articles/choosing-a-method.md)
for guidance on which algorithm to pick for which kind of image.

## See also

Useful links:

- <https://github.com/billdenney/thinr>

- Report bugs at <https://github.com/billdenney/thinr/issues>

## Author

**Maintainer**: Bill Denney <wdenney@humanpredictions.com> (affiliation:
Human Predictions, LLC)

Authors:

- Bill Denney <wdenney@humanpredictions.com> (affiliation: Human
  Predictions, LLC)
