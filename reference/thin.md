# Thin (skeletonize) a binary image

Reduce a binary image to its one-pixel-wide skeleton using one of the
supported thinning algorithms.

## Usage

``` r
thin(
  image,
  method = c("zhang_suen", "guo_hall", "lee", "k3m"),
  max_iter = 1000L
)
```

## Arguments

- image:

  A binary image: a matrix or array where non-zero values are foreground
  and zero values are background. Logical, integer, and numeric inputs
  are all accepted. The image is treated as a 2-D matrix; arrays with
  more than two dimensions are not yet supported.

- method:

  Algorithm to use. One of `"zhang_suen"` (default, matches
  `EBImage::thinImage`), `"guo_hall"`, `"lee"` (2-D adaptation of Lee,
  Kashyap & Chu 1994), or `"k3m"` (Saeed et al. 2010). See
  [`vignette("choosing-a-method")`](https://humanpred.github.io/thinr/articles/choosing-a-method.md)
  for guidance on which to pick.

- max_iter:

  Maximum number of passes. Default 1000. Real binary images of typical
  sizes converge well under 50 passes; the limit is a safety bound
  against pathological inputs.

## Value

A matrix of the same shape and storage mode as `image`, with foreground
pixels marking the thinned skeleton and the rest set to background.

## Examples

``` r
# A 3x3 solid square thins to a single foreground pixel.
m <- matrix(c(0, 0, 0, 0, 0,
              0, 1, 1, 1, 0,
              0, 1, 1, 1, 0,
              0, 1, 1, 1, 0,
              0, 0, 0, 0, 0),
            nrow = 5, byrow = TRUE)
thin(m, method = "zhang_suen")
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    0    0    0    0
#> [2,]    0    0    0    0    0
#> [3,]    0    0    1    0    0
#> [4,]    0    0    0    0    0
#> [5,]    0    0    0    0    0
thin(m, method = "guo_hall")
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    0    0    0    0
#> [2,]    0    0    0    0    0
#> [3,]    0    0    1    0    0
#> [4,]    0    0    0    0    0
#> [5,]    0    0    0    0    0
```
