# Distance transform of a binary image

Compute the distance from each foreground pixel to the nearest
background pixel, under one of three standard metrics.

## Usage

``` r
distance_transform(image, metric = c("euclidean", "manhattan", "chessboard"))
```

## Arguments

- image:

  A binary image: a matrix where non-zero values are foreground and zero
  values are background. Logical, integer, and numeric inputs are
  accepted. `NA` values are rejected with an error rather than silently
  coerced.

- metric:

  Distance metric. One of:

  - `"euclidean"` (default) — exact L2 distance, via Felzenszwalb &
    Huttenlocher (2012) linear-time separable algorithm.

  - `"manhattan"` — L1 distance via two-pass forward + backward sweep
    (Rosenfeld & Pfaltz 1968).

  - `"chessboard"` — L_infinity (Chebyshev) distance via the same
    two-pass sweep with 8-connected propagation.

## Value

A numeric matrix of the same shape as `image`. Background pixels are 0;
foreground pixels carry their distance to the nearest background pixel.
When the image is entirely foreground (no background pixel exists) every
pixel is `Inf`, consistently across all three metrics — the distance to
a non-existent nearest background pixel is unbounded, matching
`EBImage::distmap()`. Only pixels count as background; the region
outside the matrix is not treated as an implicit background border.

## References

Felzenszwalb, P. F., & Huttenlocher, D. P. (2012). Distance transforms
of sampled functions. *Theory of Computing*, 8(19), 415-428.
[doi:10.4086/toc.2012.v008a019](https://doi.org/10.4086/toc.2012.v008a019)

Rosenfeld, A., & Pfaltz, J. L. (1968). Distance functions on digital
pictures. *Pattern Recognition*, 1(1), 33-61.
[doi:10.1016/0031-3203(68)90013-7](https://doi.org/10.1016/0031-3203%2868%2990013-7)

## Examples

``` r
# A 5x5 image with a single background pixel in the corner.
m <- matrix(1L, nrow = 5, ncol = 5)
m[1, 1] <- 0L
distance_transform(m, metric = "manhattan")
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    1    2    3    4
#> [2,]    1    2    3    4    5
#> [3,]    2    3    4    5    6
#> [4,]    3    4    5    6    7
#> [5,]    4    5    6    7    8
distance_transform(m, metric = "chessboard")
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    1    2    3    4
#> [2,]    1    1    2    3    4
#> [3,]    2    2    2    3    4
#> [4,]    3    3    3    3    4
#> [5,]    4    4    4    4    4
round(distance_transform(m, metric = "euclidean"), 3)
#>      [,1]  [,2]  [,3]  [,4]  [,5]
#> [1,]    0 1.000 2.000 3.000 4.000
#> [2,]    1 1.414 2.236 3.162 4.123
#> [3,]    2 2.236 2.828 3.606 4.472
#> [4,]    3 3.162 3.606 4.243 5.000
#> [5,]    4 4.123 4.472 5.000 5.657
```
