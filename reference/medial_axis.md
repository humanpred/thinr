# Medial axis transform

Return the medial axis of a binary image: the locus of foreground pixels
that are local maxima of the (squared) Euclidean distance transform in
at least one of the four principal directions (horizontal, vertical,
NW-SE diagonal, NE-SW diagonal). Each skeleton pixel carries width
information via the distance value at that point.

## Usage

``` r
medial_axis(image, return_distance = FALSE)
```

## Arguments

- image:

  A binary image: a matrix where non-zero values are foreground and zero
  values are background. Logical, integer, and numeric inputs are
  accepted; `NA` values are not.

- return_distance:

  Logical. If `FALSE` (default), return only the binary skeleton in the
  same storage mode as `image`. If `TRUE`, return a list with elements
  `skeleton` (the binary skeleton) and `distance` (a numeric matrix of
  Euclidean distances from each foreground pixel to the nearest
  background pixel, with 0 on background pixels).

## Value

Either a matrix (when `return_distance = FALSE`) or a
`list(skeleton, distance)` (when `return_distance = TRUE`).

## Details

This is different from
[`thin()`](https://humanpred.github.io/thinr/reference/thin.md):
classical thinning algorithms produce a connected, 1-pixel-wide skeleton
without width information. The medial axis transform (Blum 1967)
produces a skeleton **with** width information, useful for shape
analysis where local thickness matters.

## References

Blum, H. (1967). A transformation for extracting new descriptors of
shape. In *Models for the Perception of Speech and Visual Form* (pp.
362-380). MIT Press.

Felzenszwalb, P. F., & Huttenlocher, D. P. (2012). Distance transforms
of sampled functions. *Theory of Computing*, 8(19), 415-428.
[doi:10.4086/toc.2012.v008a019](https://doi.org/10.4086/toc.2012.v008a019)

## Examples

``` r
# A 7x9 solid rectangle: the medial axis is the middle row.
m <- matrix(0L, nrow = 7, ncol = 9)
m[3:5, 3:7] <- 1L
medial_axis(m)
#>      [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9]
#> [1,]    0    0    0    0    0    0    0    0    0
#> [2,]    0    0    0    0    0    0    0    0    0
#> [3,]    0    0    1    1    0    1    1    0    0
#> [4,]    0    0    1    1    1    1    1    0    0
#> [5,]    0    0    1    1    0    1    1    0    0
#> [6,]    0    0    0    0    0    0    0    0    0
#> [7,]    0    0    0    0    0    0    0    0    0

# Returning width information alongside the skeleton.
result <- medial_axis(m, return_distance = TRUE)
result$skeleton
#>      [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9]
#> [1,]    0    0    0    0    0    0    0    0    0
#> [2,]    0    0    0    0    0    0    0    0    0
#> [3,]    0    0    1    1    0    1    1    0    0
#> [4,]    0    0    1    1    1    1    1    0    0
#> [5,]    0    0    1    1    0    1    1    0    0
#> [6,]    0    0    0    0    0    0    0    0    0
#> [7,]    0    0    0    0    0    0    0    0    0
round(result$distance, 3)
#>      [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9]
#> [1,]    0    0    0    0    0    0    0    0    0
#> [2,]    0    0    0    0    0    0    0    0    0
#> [3,]    0    0    1    1    1    1    1    0    0
#> [4,]    0    0    1    2    2    2    1    0    0
#> [5,]    0    0    1    1    1    1    1    0    0
#> [6,]    0    0    0    0    0    0    0    0    0
#> [7,]    0    0    0    0    0    0    0    0    0
```
