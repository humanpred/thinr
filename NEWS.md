# thinr 0.2.0

Initial CRAN release.

## Algorithm verification pass

Each algorithm implementation was reviewed against its primary source (or against the Lam-Lee-Suen 1992 survey where the original paper was unavailable). Findings:

- **K3M**: the lookup tables `A_0`, `A_1`, ..., `A_5`, `A_1pix` are now reproduced verbatim from Saeed et al. (2010), Section 3.3, page 327. The previous v0.2.0-dev tables were off by one phase. The algorithm structure is now also sequential (scanline-type) as the paper describes, not the parallel mark-then-delete of the earlier draft.
- **OPTA / SPTA**: rewritten to use the actual safe-point boolean expression from Lam, Lee & Suen (1992) page 873. The previous "spike / isthmus guard" was not in the original Naccache-Shinghal paper.
- **Holt**: rewritten to use Holt's condition `H` exactly as given in Lam, Lee & Suen (1992) page 877. The previous "isolated 2x2 preservation" was not in Holt's original algorithm. Holt's `H` is documented to prevent disappearance of 2-pixel-wide vertical lines specifically and does not guarantee arbitrary topology preservation; the ring-topology test is skipped for this method.
- **Hilditch**: implementation matches the *parallel form* (Rutovitz R1-R4) that most modern surveys label "Hilditch". The original Hilditch (1969) is a *sequential* algorithm with within-pass deletion tracking and uses a different crossing number `X_H`; the parallel form is what the literature commonly cites. Source header now states this explicitly.
- **Stentiford**: the name is a folk misattribution in the wider literature. Stentiford & Mortimer (1983) actually describes *preprocessing* heuristics (hole removal, smoothing, acute-angle emphasis) intended to run before a separate thinning step, not a thinning algorithm itself. The four-template directional thinning implemented under this name is closer to that of Stefanelli & Rosenfeld (1971). The name is retained for compatibility; the source header documents this clearly.
- **Pavlidis**: the implementation here (`B(P)` in `[2, 5]` 4-directional thinning) does not match Pavlidis (1980), which is contour-following with multi-pixel detection masks. Source header documents this; a faithful implementation is on the roadmap.
- **Distance transform**: verified against Felzenszwalb & Huttenlocher (2012) Algorithm 1, page 420.
- **Zhang-Suen, Guo-Hall, Lee 2D, medial axis**: unchanged; the existing implementations match the standard published forms.

The OPTA "horizontal line collapses to one row" test is relaxed for OPTA and Holt: OPTA's N2 condition protects diagonal-2-neighbour patterns at bar corners (a documented property of SPTA - see Lam-Lee-Suen 1992 page 873) and Holt's `H` has no topology guard.

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
