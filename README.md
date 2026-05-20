# thinr

<!-- badges: start -->
[![R-CMD-check](https://github.com/humanpred/thinr/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/humanpred/thinr/actions/workflows/R-CMD-check.yaml)
[![Codecov test coverage](https://codecov.io/gh/humanpred/thinr/graph/badge.svg)](https://app.codecov.io/gh/humanpred/thinr)
[![pkgdown](https://github.com/humanpred/thinr/actions/workflows/pkgdown.yaml/badge.svg)](https://github.com/humanpred/thinr/actions/workflows/pkgdown.yaml)
[![lint](https://github.com/humanpred/thinr/actions/workflows/lint.yaml/badge.svg)](https://github.com/humanpred/thinr/actions/workflows/lint.yaml)
<!-- badges: end -->

Binary image thinning (skeletonization) algorithms for R, plus the medial axis transform and a fast distance transform. Designed as a drop-in replacement for `EBImage::thinImage()` with eight additional algorithms behind a single dispatching function.

## Installation

```r
# Once on CRAN:
# install.packages("thinr")

# From GitHub:
# install.packages("remotes")
remotes::install_github("humanpred/thinr")
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
thin(m, method = "hilditch")
thin(m, method = "holt")

# Drop-in for EBImage::thinImage()
thinImage(m)

# Medial axis transform (returns binary skeleton + per-pixel width)
medial_axis(m)
medial_axis(m, return_distance = TRUE)

# Distance transform as a standalone utility
distance_transform(m, metric = "euclidean")
distance_transform(m, metric = "manhattan")
distance_transform(m, metric = "chessboard")
```

## Algorithms

| Method        | Reference                                            |
|---------------|------------------------------------------------------|
| `zhang_suen`  | Zhang and Suen (1984). Default; matches `EBImage::thinImage()`. |
| `guo_hall`    | Guo and Hall (1989). |
| `lee`         | Lee, Kashyap, and Chu (1994), 2-D adaptation. |
| `k3m`         | Saeed et al. (2010). |
| `hilditch`    | Hilditch (1969). Look-ahead crossing-number check. |
| `stentiford`  | Stentiford and Mortimer (1983). |
| `pavlidis`    | Pavlidis (1980). |
| `opta`        | Naccache and Shinghal (1984). One-pass. |
| `holt`        | Holt et al. (1987). Preserves isolated 2x2 blocks. |

Plus:

| Function             | Purpose                                               |
|----------------------|-------------------------------------------------------|
| `medial_axis()`      | Medial axis (Blum 1967): skeleton + width information. |
| `distance_transform()` | Euclidean (Felzenszwalb-Huttenlocher 2012), Manhattan, or Chessboard distance transform. |

See `vignette("choosing-a-method")` for guidance.

## License

LGPL-3. Chosen for drop-in compatibility with `EBImage` (which is LGPL) so that `EBImage` can optionally depend on `thinr` and retire its in-tree thinning code.
