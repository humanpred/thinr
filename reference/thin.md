# Thin (skeletonize) a binary image

Reduce a binary image to its one-pixel-wide skeleton using one of the
supported thinning algorithms.

## Usage

``` r
thin(
  image,
  method = c("zhang_suen", "guo_hall", "lee", "k3m", "hilditch", "opta", "holt"),
  max_iter = 1000L
)
```

## Arguments

- image:

  A binary image: a matrix or array where non-zero values are foreground
  and zero values are background. Logical, integer, and numeric inputs
  are all accepted. `NA` values are rejected with an error rather than
  silently coerced (a background pixel would change the skeleton). The
  image is treated as a 2-D matrix; arrays with more than two dimensions
  are not yet supported.

- method:

  Algorithm to use. One of `"zhang_suen"` (default), `"guo_hall"`,
  `"lee"` (2-D adaptation of Lee, Kashyap & Chu 1994), `"k3m"` (Saeed et
  al. 2010), `"hilditch"` (Hilditch 1969), `"opta"` (Naccache & Shinghal
  1984), or `"holt"` (Holt et al. 1987). See
  [`vignette("choosing-a-method")`](https://humanpred.github.io/thinr/articles/choosing-a-method.md)
  for guidance on which to pick, and
  [`vignette("correctness-properties")`](https://humanpred.github.io/thinr/articles/correctness-properties.md)
  for the edge-handling and connectivity guarantees shared by every
  method.

- max_iter:

  Maximum number of passes. Default 1000. Real binary images of typical
  sizes converge well under 50 passes; the limit is a safety bound
  against pathological inputs.

## Value

A matrix of the same shape and storage mode as `image`, with foreground
pixels marking the thinned skeleton and the rest set to background.

## Edge handling

Every kernel inspects an 8-neighbourhood, so a naive implementation can
never delete a pixel in the outermost row or column and leaves shapes
that touch the matrix border two or three pixels thick. `thin()`
therefore surrounds the image with a one-pixel background margin before
thinning and crops it back afterwards, so a shape is skeletonised
identically whether or not it touches the frame. This applies uniformly
to all seven methods.

## Connectivity

Thinning only ever deletes foreground pixels, and each method deletes in
an order that keeps 8-connected components connected: an object that
starts as one connected component thins to one connected component. In
particular a two-pixel-wide stroke thins to a connected one-pixel line
rather than fragmenting. See
[`vignette("correctness-properties")`](https://humanpred.github.io/thinr/articles/correctness-properties.md).

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
thin(m, method = "hilditch")
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    0    0    0    0
#> [2,]    0    0    0    0    0
#> [3,]    0    0    1    0    0
#> [4,]    0    0    0    0    0
#> [5,]    0    0    0    0    0
```
