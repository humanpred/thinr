# Drop-in replacement for `EBImage::thinImage`

Applies Zhang-Suen thinning to a binary image. Provided as a
signature-compatible alternative to `EBImage::thinImage()` so callers
can switch from `EBImage` to `thinr` by changing the namespace prefix
only.

## Usage

``` r
thinImage(x)
```

## Arguments

- x:

  A binary image. Same constraints as
  [`thin()`](https://humanpred.github.io/thinr/reference/thin.md)'s
  `image` argument.

## Value

The thinned skeleton in the same storage mode as `x`.

## Details

For access to the other algorithms (Guo-Hall, and eventually Lee / K3M),
use [`thin()`](https://humanpred.github.io/thinr/reference/thin.md).

## Examples

``` r
m <- matrix(c(0, 1, 1, 1, 0,
              0, 1, 1, 1, 0,
              0, 1, 1, 1, 0),
            nrow = 3, byrow = TRUE)
thinImage(m)
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    1    1    1    0
#> [2,]    0    0    1    0    0
#> [3,]    0    1    1    1    0
```
