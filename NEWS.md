# thinr 0.1.0

Initial release.

## Algorithms

- `zhang_suen` — Zhang & Suen (1984). Full implementation.
- `guo_hall` — Guo & Hall (1989). Full implementation.
- `lee` — Lee (1994). Stub; planned for v0.2.
- `k3m` — Saeed et al. (2010). Stub; planned for v0.2.

## API

- `thin(image, method)` — main dispatching function.
- `thinImage(x)` — drop-in replacement for `EBImage::thinImage()`. Uses Zhang-Suen.
- Accepts logical, integer, and numeric input matrices; preserves storage mode in the return.

## Known limitations

- 2-D matrix inputs only; higher-dimensional arrays are not yet supported. 3-D support arrives with the Lee algorithm in v0.2.
- Lee and K3M are stubs that error with a clear message. Two algorithms are enough to validate the package API and to provide an immediate drop-in replacement for `EBImage::thinImage()`; the other two follow in v0.2 once the underlying implementations are written.
