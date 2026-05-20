# thinr 0.2.0

Initial CRAN release.

## Thinning algorithms

Nine algorithms behind a single dispatching function `thin(image, method)`:

- `zhang_suen` — Zhang and Suen (1984) <doi:10.1145/357994.358023>. Default; matches `EBImage::thinImage()`.
- `guo_hall` — Guo and Hall (1989) <doi:10.1145/62065.62074>.
- `lee` — 2-D adaptation of Lee, Kashyap, and Chu (1994) <doi:10.1006/cgip.1994.1042>.
- `k3m` — Saeed, Tabedzki, Rybnik, and Adamski (2010) <doi:10.2478/v10006-010-0024-4>.
- `hilditch` — Hilditch (1969), single-pass parallel thinning with look-ahead crossing-number check.
- `stentiford` — Stentiford and Mortimer (1983) <doi:10.1109/TSMC.1983.6313095>, four directional 3-pixel templates per pass.
- `pavlidis` — Pavlidis (1980) <doi:10.1016/0146-664X(80)90041-5>, restrictive `B(P) <= 5` interior preservation.
- `opta` — Naccache and Shinghal (1984) <doi:10.1016/0031-3203(84)90067-1>, one-pass thinning derived from Hilditch.
- `holt` — Holt, Stewart, von Diprosperro and Cross (1987), Zhang-Suen variant with isolated-2x2-block preservation.

## Medial axis and distance transform

- `medial_axis(image, return_distance = FALSE)` returns the medial axis (Blum 1967), the locus of foreground pixels that are ridge points of the squared Euclidean distance transform along at least one of the four principal directions. With `return_distance = TRUE`, returns the binary skeleton alongside the per-pixel Euclidean distance to the nearest background pixel.
- `distance_transform(image, metric)` exposes the distance transform as a first-class utility. Metric is one of `"euclidean"` (Felzenszwalb and Huttenlocher 2012 <doi:10.4086/toc.2012.v008a019>, linear-time separable algorithm), `"manhattan"` (L1, two-pass forward + backward sweep; Rosenfeld and Pfaltz 1968 <doi:10.1016/0031-3203(68)90013-7>), or `"chessboard"` (L_infinity / Chebyshev, two-pass sweep with 8-connected propagation).

## Drop-in compatibility

`thinImage(x)` matches the signature of `EBImage::thinImage()` and runs the Zhang-Suen algorithm. Existing code can switch by changing only the namespace prefix.

## Storage modes

`thin()`, `medial_axis()`, and `thinImage()` accept logical, integer, or numeric input matrices and return a matrix of the same storage mode. `distance_transform()` always returns a numeric matrix.

## Implementation notes

- All thinning algorithms are implemented in Rcpp; the inner loops are pure integer arithmetic on the 8-neighbourhood.
- The Lee 2-D, K3M, OPTA, Pavlidis, and Holt implementations follow the published descriptions of the algorithms. The Hilditch and Stentiford implementations follow standard image-processing references. Reviewers familiar with any of the original publications are invited to verify the implementations and submit corrections.
- 2-D matrices only in this release. 3-D thinning (Lee's original 1994 form, which uses a 26-cell Euler-invariance lookup) is left for a future release. The medial axis and distance transform algorithms generalize trivially to 3-D and may pick up that support in a later release.
