# Choosing a thinning method

Thinning reduces a binary shape to a one-pixel-wide centerline that
preserves topology. Several algorithms exist; they differ in how
aggressively they erode pixels, how well they preserve corners, and how
fast they run. This vignette gives a short guide to choosing among the
algorithms `thinr` provides.

## What’s implemented

| Method | Reference | Approach |
|----|----|----|
| `zhang_suen` | Zhang & Suen (1984) | 2 sub-iterations, mixed-direction products |
| `guo_hall` | Guo & Hall (1989) | 2 sub-iterations, conditions tuned for diagonals |
| `lee` | Lee, Kashyap & Chu (1994), 2-D | 4 directional sub-iterations + Euler-invariance |
| `k3m` | Saeed et al. (2010) | 6 phases of progressively permissive removal |
| `hilditch` | parallel form (Rutovitz-style) | Single pass with look-ahead crossing-number check |
| `opta` | Naccache & Shinghal (1984) | Safe-point thinning (SPTA) |
| `holt` | Holt, Stewart, Clint & Perrott (1987) | One subcycle with edge information on neighbours |

`zhang_suen` is the default.

`lee` is a 2-D adaptation of Lee’s original 3-D algorithm. The 3-D case
(volumetric thinning) is not implemented in this release.

The `hilditch` method implements the *parallel form* commonly cited as
“Hilditch” in modern image-processing surveys; Hilditch’s 1969 paper
actually describes a sequential algorithm with within-pass deletion
tracking and a different crossing-number definition. See Lam, Lee & Suen
(1992) for the relationship between the two.

The K3M lookup tables `A_0 … A_5` are reproduced from Saeed et
al. (2010), Section 3.3, page 327. OPTA’s safe-point boolean expression
and Holt’s condition `H` are taken from the original papers; the
Lam-Lee-Suen 1992 survey was used as cross-reference and one
transcription error in Holt’s middle clause (north vs east) was
corrected against the original.

## A quick visual comparison

``` r

# A 'V' shape — exercises diagonal preservation
make_v <- function() {
  m <- matrix(0L, 11, 11)
  for (k in seq(0, 5)) {
    m[2 + k, 2 + k] <- 1L
    m[2 + k, 10 - k] <- 1L
    m[3 + k, 2 + k] <- 1L
    m[3 + k, 10 - k] <- 1L
  }
  m
}
v <- make_v()
v
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    1    0    0    0    0    0    0    0     1     0
#>  [3,]    0    1    1    0    0    0    0    0    1     1     0
#>  [4,]    0    0    1    1    0    0    0    1    1     0     0
#>  [5,]    0    0    0    1    1    0    1    1    0     0     0
#>  [6,]    0    0    0    0    1    1    1    0    0     0     0
#>  [7,]    0    0    0    0    1    1    1    0    0     0     0
#>  [8,]    0    0    0    0    1    0    1    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
#> [10,]    0    0    0    0    0    0    0    0    0     0     0
#> [11,]    0    0    0    0    0    0    0    0    0     0     0
```

``` r

thin(v, method = "zhang_suen")
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    0    0    0    0    0    0    0     0     0
#>  [4,]    0    0    0    0    0    0    0    0    0     0     0
#>  [5,]    0    0    0    0    0    0    0    0    0     0     0
#>  [6,]    0    0    0    0    1    1    1    0    0     0     0
#>  [7,]    0    0    0    0    0    0    0    0    0     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
#> [10,]    0    0    0    0    0    0    0    0    0     0     0
#> [11,]    0    0    0    0    0    0    0    0    0     0     0
```

``` r

thin(v, method = "guo_hall")
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    1    0    0    0    0    0    0    0     1     0
#>  [3,]    0    1    0    0    0    0    0    0    1     0     0
#>  [4,]    0    0    1    0    0    0    0    1    0     0     0
#>  [5,]    0    0    0    1    0    0    1    0    0     0     0
#>  [6,]    0    0    0    0    1    1    0    0    0     0     0
#>  [7,]    0    0    0    0    0    1    0    0    0     0     0
#>  [8,]    0    0    0    0    1    0    1    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
#> [10,]    0    0    0    0    0    0    0    0    0     0     0
#> [11,]    0    0    0    0    0    0    0    0    0     0     0
```

``` r

thin(v, method = "hilditch")
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    0    0    0    0    0    0    0     0     0
#>  [4,]    0    0    0    0    0    0    0    0    0     0     0
#>  [5,]    0    0    0    0    0    0    0    0    0     0     0
#>  [6,]    0    0    0    0    1    1    1    0    0     0     0
#>  [7,]    0    0    0    0    0    0    0    0    0     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
#> [10,]    0    0    0    0    0    0    0    0    0     0     0
#> [11,]    0    0    0    0    0    0    0    0    0     0     0
```

``` r

thin(v, method = "k3m")
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    0    0    0    0    0    0    0     0     0
#>  [4,]    0    0    0    0    0    0    0    0    0     0     0
#>  [5,]    0    0    0    0    1    0    1    0    0     0     0
#>  [6,]    0    0    0    0    1    0    1    0    0     0     0
#>  [7,]    0    0    0    0    1    1    1    0    0     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
#> [10,]    0    0    0    0    0    0    0    0    0     0     0
#> [11,]    0    0    0    0    0    0    0    0    0     0     0
```

``` r

thin(v, method = "holt")
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    0    0    0    0    0    0    0     0     0
#>  [4,]    0    0    0    0    0    0    0    0    0     0     0
#>  [5,]    0    0    0    0    0    0    0    0    0     0     0
#>  [6,]    0    0    0    0    1    1    1    0    0     0     0
#>  [7,]    0    0    0    0    0    0    0    0    0     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
#> [10,]    0    0    0    0    0    0    0    0    0     0     0
#> [11,]    0    0    0    0    0    0    0    0    0     0     0
```

The thinning algorithms produce broadly similar skeletons on this V —
they all collapse the two diagonal strokes to single lines and meet at
the bottom. Differences show up on more complex shapes:

- `zhang_suen` is the most aggressive on simple shapes.
- `guo_hall` and `k3m` preserve corners marginally better.
- `hilditch` has the look-ahead crossing-number check, which gives
  different connectivity properties at junctions.
- `holt` uses edge information about neighbouring pixels rather than a
  crossing-number check on the central pixel; it is specifically
  designed to preserve 2-pixel-wide lines.

## When to use which

- **`zhang_suen`** — the default. Most predictable behavior. Use for
  general purpose thinning. One caveat: an *isolated* 2×2 block is
  erased entirely (see “Small isolated blobs” below).
- **`guo_hall`** — try this if your skeletons have lots of diagonal
  features and Zhang-Suen is breaking them at corners. Keeps one pixel
  of an isolated 2×2 block where Zhang-Suen erases it.
- **`lee`** — when you want directional processing (four sub-iterations
  per pass, one per cardinal direction). Sometimes produces cleaner
  skeletons on asymmetric inputs.
- **`k3m`** — strongest corner preservation in published comparative
  studies, at the cost of being slower (six phases per outer iteration
  vs. two for Zhang-Suen).
- **`hilditch`** — well-cited historical algorithm; the look-ahead
  crossing-number check makes its connectivity slightly different from
  the other parallel algorithms.
- **`opta`** — one-pass safe-point algorithm. Its `N2` condition
  protects two-4-adjacent-pixel diagonal patterns, which can leave stray
  pixels at bar corners (a documented property of SPTA).
- **`holt`** — when 2-pixel-wide lines should be preserved. The
  algorithm uses edge information from neighbouring pixels in a 5x5
  window, allowing a single subcycle.

### Small isolated blobs

The methods disagree on the smallest shapes. An isolated 2×2 block is
the notable case: all four of its pixels pass the Zhang-Suen deletion
gate in the same sub-iteration, so the default method erases the block
completely, while Guo-Hall keeps one pixel.

``` r

block <- matrix(0L, 6, 6)
block[3:4, 3:4] <- 1L
sum(thin(block, method = "zhang_suen"))  # 0 — the block is erased
#> [1] 0
sum(thin(block, method = "guo_hall"))    # 1 — one pixel survives
#> [1] 1
```

This matters when a mask can contain tiny blobs — a marker remnant, a
dotted-line dash, a speck left after segmentation. If such blobs must
not vanish, use `guo_hall` (or filter blobs by size before thinning)
rather than the default. Larger isolated shapes (3×3 and up) survive
under every method; only the 2×2 block is fully erased.

## Medial axis transform

The thinning algorithms above all produce binary 1-pixel-wide skeletons
without width information. For tasks where local thickness matters, use
[`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md):

``` r

m <- matrix(0L, 9, 11)
m[3:7, 3:9] <- 1L      # 5x7 solid rectangle
medial_axis(m)
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    1    1    0    0    0    1    1     0     0
#>  [4,]    0    0    1    1    1    0    1    1    1     0     0
#>  [5,]    0    0    0    1    1    1    1    1    0     0     0
#>  [6,]    0    0    1    1    1    0    1    1    1     0     0
#>  [7,]    0    0    1    1    0    0    0    1    1     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
```

``` r

result <- medial_axis(m, return_distance = TRUE)
result$skeleton
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    1    1    0    0    0    1    1     0     0
#>  [4,]    0    0    1    1    1    0    1    1    1     0     0
#>  [5,]    0    0    0    1    1    1    1    1    0     0     0
#>  [6,]    0    0    1    1    1    0    1    1    1     0     0
#>  [7,]    0    0    1    1    0    0    0    1    1     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
round(result$distance, 3)
#>       [,1] [,2] [,3] [,4] [,5] [,6] [,7] [,8] [,9] [,10] [,11]
#>  [1,]    0    0    0    0    0    0    0    0    0     0     0
#>  [2,]    0    0    0    0    0    0    0    0    0     0     0
#>  [3,]    0    0    1    1    1    1    1    1    1     0     0
#>  [4,]    0    0    1    2    2    2    2    2    1     0     0
#>  [5,]    0    0    1    2    3    3    3    2    1     0     0
#>  [6,]    0    0    1    2    2    2    2    2    1     0     0
#>  [7,]    0    0    1    1    1    1    1    1    1     0     0
#>  [8,]    0    0    0    0    0    0    0    0    0     0     0
#>  [9,]    0    0    0    0    0    0    0    0    0     0     0
```

The distance is the Euclidean distance from each foreground pixel to the
nearest background pixel.

## Distance transform

[`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md)
exposes the distance transform itself as a stand-alone utility, with a
choice of metric:

``` r

m <- matrix(1L, 5, 5)
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

The Euclidean implementation uses the linear-time separable algorithm of
Felzenszwalb and Huttenlocher (2012); the others use the classical
Rosenfeld and Pfaltz (1968) two-pass sweep.

## Inputs and outputs

[`thin()`](https://humanpred.github.io/thinr/reference/thin.md) and
[`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md)
accept logical, integer, and numeric matrices. Non-zero values are
foreground. The return matrix matches the storage mode of the input —
logical in, logical out; integer in, integer out; numeric in, numeric
out.

[`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md)
always returns a numeric matrix.

Higher-dimensional arrays (e.g. 3-D volumes) are not yet supported.

## Performance

A simple [`bench::mark()`](https://bench.r-lib.org/reference/mark.html)
comparison on a moderate image (illustrative):

``` r

library(bench)
m <- matrix(0L, 200, 200)
m[50:150, 50:150] <- 1L  # solid square

bench::mark(
  zs       = thin(m, method = "zhang_suen"),
  gh       = thin(m, method = "guo_hall"),
  hild     = thin(m, method = "hilditch"),
  k3m      = thin(m, method = "k3m"),
  ma       = medial_axis(m),
  dt_eucl  = distance_transform(m, metric = "euclidean"),
  iterations = 5,
  check = FALSE
)
```

All thinning algorithms are `O(iterations × pixels)`. Constant factors
are smallest for `zhang_suen` and `guo_hall`; `holt` and `k3m` are the
most expensive because of their look-aheads. The Euclidean distance
transform is `O(pixels)` via Felzenszwalb-Huttenlocher; medial axis is
`O(pixels)` since it just adds a single linear pass over the DT.

For 200×200 images all algorithms finish in a few milliseconds on modern
hardware.

## References

### Thinning

- Zhang, T. Y. & Suen, C. Y. (1984). A fast parallel algorithm for
  thinning digital patterns. *Communications of the ACM*, 27(3),
  236–239.
- Guo, Z. & Hall, R. W. (1989). Parallel thinning with two-subiteration
  algorithms. *Communications of the ACM*, 32(3), 359–373.
- Lee, T.-C., Kashyap, R. L., & Chu, C.-N. (1994). Building skeleton
  models via 3-D medial surface axis thinning algorithms. *CVGIP:
  Graphical Models and Image Processing*, 56(6), 462–478.
- Saeed, K., Tabędzki, M., Rybnik, M., & Adamski, M. (2010). K3M: A
  universal algorithm for image skeletonization and a review of thinning
  techniques. *International Journal of Applied Mathematics and Computer
  Science*, 20(2), 317–335.
- Hilditch, C. J. (1969). Linear skeletons from square cupboards. In B.
  Meltzer & D. Michie (Eds.), *Machine Intelligence 4* (pp. 403–420).
  Edinburgh University Press.
- Naccache, N. J. & Shinghal, R. (1984). An investigation into the
  skeletonization approach of Hilditch. *Pattern Recognition*, 17(3),
  279–284.
- Holt, C. M., Stewart, A., Clint, M., & Perrott, R. H. (1987). An
  improved parallel thinning algorithm. *Communications of the ACM*,
  30(2), 156–160.

### Survey

- Lam, L., Lee, S.-W., & Suen, C. Y. (1992). Thinning methodologies — a
  comprehensive survey. *IEEE Transactions on Pattern Analysis and
  Machine Intelligence*, 14(9), 869–885.

### Medial axis and distance transform

- Blum, H. (1967). A transformation for extracting new descriptors of
  shape. In *Models for the Perception of Speech and Visual Form*
  (pp. 362–380). MIT Press.
- Felzenszwalb, P. F. & Huttenlocher, D. P. (2012). Distance transforms
  of sampled functions. *Theory of Computing*, 8(19), 415–428.
- Rosenfeld, A. & Pfaltz, J. L. (1968). Distance functions on digital
  pictures. *Pattern Recognition*, 1(1), 33–61.
