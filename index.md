# thinr

Binary image thinning (skeletonization) algorithms for R, plus the
medial axis transform and a fast Euclidean / Manhattan / Chessboard
distance transform. Designed as a drop-in replacement for
`EBImage::thinImage()` with six additional thinning algorithms behind a
single dispatching function.

## Installation

``` r

# Once on CRAN:
# install.packages("thinr")

# From GitHub:
# install.packages("remotes")
remotes::install_github("humanpred/thinr")
```

## Usage

``` r

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

| Method | Reference |
|----|----|
| `zhang_suen` | Zhang, T. Y. & Suen, C. Y. (1984). A fast parallel algorithm for thinning digital patterns. *Communications of the ACM*, 27(3), 236–239. [doi:10.1145/357994.358023](https://doi.org/10.1145/357994.358023). *Default; matches `EBImage::thinImage()`.* |
| `guo_hall` | Guo, Z. & Hall, R. W. (1989). Parallel thinning with two-subiteration algorithms. *Communications of the ACM*, 32(3), 359–373. [doi:10.1145/62065.62074](https://doi.org/10.1145/62065.62074). |
| `lee` | Lee, T.-C., Kashyap, R. L. & Chu, C.-N. (1994). Building skeleton models via 3-D medial surface axis thinning algorithms. *CVGIP: Graphical Models and Image Processing*, 56(6), 462–478. [doi:10.1006/cgip.1994.1042](https://doi.org/10.1006/cgip.1994.1042). 2-D adaptation; the 3-D form is not implemented. |
| `k3m` | Saeed, K., Tabędzki, M., Rybnik, M. & Adamski, M. (2010). K3M: A universal algorithm for image skeletonization and a review of thinning techniques. *International Journal of Applied Mathematics and Computer Science*, 20(2), 317–335. [doi:10.2478/v10006-010-0024-4](https://doi.org/10.2478/v10006-010-0024-4). Lookup tables `A_0…A_5` reproduced from the paper. |
| `hilditch` | Parallel form commonly attributed to Hilditch. The original algorithm is Hilditch, C. J. (1969). Linear skeletons from square cupboards. In B. Meltzer & D. Michie (Eds.), *Machine Intelligence 4* (pp. 403–420). Edinburgh University Press. The parallel form here uses Rutovitz-style `R1`–`R4` conditions; see Lam, Lee & Suen (1992) for the precise differences. |
| `opta` | Naccache, N. J. & Shinghal, R. (1984). An investigation into the skeletonization approach of Hilditch. *Pattern Recognition*, 17(3), 279–284. Also called the Safe Point Thinning Algorithm (SPTA). |
| `holt` | Holt, C. M., Stewart, A., Clint, M. & Perrott, R. H. (1987). An improved parallel thinning algorithm. *Communications of the ACM*, 30(2), 156–160. [doi:10.1145/12527.12531](https://doi.org/10.1145/12527.12531). One-subcycle, uses edge information about neighbours. |

Plus:

| Function | Reference |
|----|----|
| [`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md) | Blum, H. (1967). A transformation for extracting new descriptors of shape. In *Models for the Perception of Speech and Visual Form* (pp. 362–380). MIT Press. Implementation finds ridge points of the squared Euclidean distance transform; returns the binary skeleton, optionally with per-pixel distance. |
| [`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md) | Felzenszwalb, P. F. & Huttenlocher, D. P. (2012). Distance transforms of sampled functions. *Theory of Computing*, 8(19), 415–428. [doi:10.4086/toc.2012.v008a019](https://doi.org/10.4086/toc.2012.v008a019). Linear-time separable squared Euclidean transform; plus the Rosenfeld & Pfaltz (1968) two-pass forward + backward sweep for `metric = "manhattan"` and `"chessboard"`. |

The survey by Lam, L., Lee, S.-W. & Suen, C. Y. (1992), “Thinning
methodologies — a comprehensive survey”, *IEEE Transactions on Pattern
Analysis and Machine Intelligence*, 14(9), 869–885,
[doi:10.1109/34.161346](https://doi.org/10.1109/34.161346), was used as
the cross-reference for the parallel-form Hilditch and for verifying
OPTA’s safe-point boolean expression.

See
[`vignette("choosing-a-method")`](https://humanpred.github.io/thinr/articles/choosing-a-method.md)
for guidance on choosing among the methods.

## License

LGPL-3. Chosen for drop-in compatibility with `EBImage` (which is LGPL)
so that `EBImage` can optionally depend on `thinr` and retire its
in-tree thinning code.
