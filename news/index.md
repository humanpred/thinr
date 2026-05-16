# Changelog

## thinr 0.2.0

All four algorithms are now implemented.

### New

- `lee` — 2-D adaptation of Lee, Kashyap & Chu (1994). Four directional
  sub-iterations (N / E / S / W boundary) with crossing-number
  Euler-invariance check and endpoint preservation.
- `k3m` — Saeed et al. (2010). Six-phase iterative thinning: phases 1–5
  remove border pixels under progressively permissive lookup tables,
  plus a final phase 0 cleanup sweep with the strictest table. Includes
  the crossing-number topology guard and endpoint preservation.

### Tests

- Test suite expanded from 17 to 38 assertions. Each of the six core
  properties (skeleton size, horizontal-line collapse, idempotence,
  all-background invariance, isolated-pixel preservation, ring-topology
  preservation) is exercised across all four methods.

### Notes

- The K3M lookup tables (`A1` through `A5`) in `src/k3m.cpp` are
  reconstructed from the algorithm’s published description. The
  algorithm produces topology-preserving, one-pixel-wide skeletons on
  the test corpus; reviewers familiar with the paper are invited to
  verify the table contents against the original publication and submit
  corrections.
- 3-D support (the original motivation for Lee’s algorithm) is still not
  implemented; arrays with more than two dimensions are explicitly
  rejected. The 2-D Lee adaptation is what ships here.

## thinr 0.1.0

Initial release.

### Algorithms

- `zhang_suen` — Zhang & Suen (1984). Full implementation.
- `guo_hall` — Guo & Hall (1989). Full implementation.
- `lee` — Lee (1994). Stub; planned for v0.2.
- `k3m` — Saeed et al. (2010). Stub; planned for v0.2.

### API

- `thin(image, method)` — main dispatching function.
- `thinImage(x)` — drop-in replacement for `EBImage::thinImage()`. Uses
  Zhang-Suen.
- Accepts logical, integer, and numeric input matrices; preserves
  storage mode in the return.

### Known limitations

- 2-D matrix inputs only; higher-dimensional arrays are not yet
  supported.
- Lee and K3M are stubs that error with a clear message. Two algorithms
  are enough to validate the package API and to provide an immediate
  drop-in replacement for `EBImage::thinImage()`; the other two follow
  in v0.2 once the underlying implementations are written.
