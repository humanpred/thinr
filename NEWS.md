# thinr 0.3.0.9000 (development version)

* Development version bump following the 0.3.0 CRAN release.

* Internal, with no change to any output: the unused `thinr::is_border_4()`
  helper was removed from `src/thinr_common.h`. Its last callers were the
  stentiford and pavlidis kernels, dropped 2026-05-20; nothing has referenced
  it since. The header now declares only the two helpers that are actually
  used.

* Internal, with no change to any output: the Zhang-Suen crossing number
  `A(p)` and neighbour count `B(p)` were re-implemented inline in
  `src/zhang_suen.cpp`, `src/lee.cpp`, and `src/holt.cpp` instead of calling
  the shared `thinr::crossing_number()` / `thinr::neighbour_count()` in
  `src/thinr_common.h`, which only `src/hilditch.cpp` used. A future
  correctness fix to the shared definition would have reached one of the four
  kernels that need it and silently missed the other three. All four now call
  the shared helpers. The replacement expressions are token-identical to the
  inline copies they replace; identical output was verified empirically across
  257 binary images (degenerate, boundary, synthetic, and real chart masks)
  for all seven methods (#F018).

# thinr 0.3.0

Correctness fixes from the figureextract ecosystem review (2026-07-03).
All are boundary conditions the published algorithms assume away; see the
new `vignette("correctness-properties")` for the guarantees they restore.

* `thin()` now skeletonises shapes that touch the matrix border correctly.
  The kernels inspect an 8-neighbourhood and so never deleted pixels in the
  outermost row or column, leaving edge-touching shapes two to three pixels
  thick; `thin()` now pads the image with a one-pixel background margin and
  crops it back, so a shape thins identically whether or not it touches the
  frame. Applies to all seven methods (#F012).

* `thin(method = "opta")` now keeps two-pixel-wide strokes connected. The
  SPTA kernel evaluated all four contour directions against the pre-cycle
  snapshot and deleted them in one batch, so both sides of a 2px stroke were
  removed together and the skeleton fragmented (a 2×9 bar collapsed to its
  four corner pixels). Deletion is now sequential per direction, as
  Naccache & Shinghal's two-scan formulation requires (#F011).

* `distance_transform(metric = "manhattan" | "chessboard")` now returns
  `Inf` for a fully-foreground image (no background pixel exists), matching
  the `"euclidean"` metric and `EBImage::distmap()`. The two-pass metrics
  previously seeded foreground with a finite sentinel that leaked out as a
  plausible-looking but meaningless finite distance (#F015).

* `thin()`, `distance_transform()`, and `medial_axis()` now reject `NA`
  input with a clear error at the coercion boundary instead of silently
  turning it into `NA_integer_` (`INT_MIN`) in the C++ kernels (#F016).

* `thin(method = "hilditch")` now thins junctions to the published parallel
  form. The look-ahead conditions 3 and 4 skip deleting a pixel only when a
  cardinal neighbour has crossing number `A == 1` on the current image; the
  kernel had compared the look-ahead crossing number (computed with the
  centre already removed) against 1, which is strictly stronger and also
  spared junction neighbours where `A >= 2`, leaving a redundant pixel beside
  the junction. Skeletons are now equal to or thinner than before, never
  thicker (verified against a reference implementation of the published form
  over random images) (#F014).

* Added a connectivity-preservation property test across all seven methods
  and tightened the Holt straight-line test (#F013).

* Documented and pinned the isolated-2×2-block behaviour: the default
  `zhang_suen` erases an isolated 2×2 block entirely while `guo_hall` keeps
  one pixel. See `vignette("choosing-a-method")` for guidance on small-blob
  masks (#F017).

* Removed `thinImage()`. Use `thin()` (Zhang-Suen is the default method).

# thinr 0.2.0

Initial CRAN release.
