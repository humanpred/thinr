# thinr

<!-- badges: start -->
[![R-CMD-check](https://github.com/billdenney/thinr/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/billdenney/thinr/actions/workflows/R-CMD-check.yaml)
<!-- badges: end -->

Binary image thinning (skeletonization) algorithms for R. Designed as a drop-in replacement for `EBImage::thinImage()` with additional algorithms behind a single dispatching function.

## Installation

```r
# Once on CRAN:
# install.packages("thinr")

# From GitHub:
# install.packages("remotes")
remotes::install_github("billdenney/thinr")
```

## Usage

```r
library(thinr)

m <- matrix(0L, 11, 11)
m[3:9, 3:9] <- 1L      # 7x7 solid square

# Default: Zhang-Suen (matches EBImage::thinImage)
thin(m)

# Or pick an algorithm explicitly
thin(m, method = "guo_hall")

# Drop-in for EBImage::thinImage()
thinImage(m)
```

## Algorithms

| Method        | Status (v0.1)   | Notes |
|---------------|-----------------|-------|
| `zhang_suen`  | Full            | Default; matches `EBImage::thinImage()`. |
| `guo_hall`    | Full            | Slightly better diagonal-corner preservation. |
| `lee`         | Stub; v0.2      | 3-D capable; Euler-invariance check. |
| `k3m`         | Stub; v0.2      | Strong corner preservation. |

See `vignette("choosing-a-method")` for guidance.

## License

LGPL-3. Chosen for drop-in compatibility with `EBImage` (which is LGPL) so that `EBImage` can optionally depend on `thinr` and retire its in-tree thinning code.
