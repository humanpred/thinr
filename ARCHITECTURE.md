# Architecture — thinr

> Per-package architecture. The figureextract meta-repo’s
> `ARCHITECTURE.md` describes the wider ecosystem context (ADR-007:
> `thinr` is the only public CRAN package).

## Purpose

Implement multiple binary image thinning algorithms behind a single
dispatching API. Fills the thinning / skeletonization gap that `EBImage`
(binary morphology, but no thinning operator) leaves in the R
image-processing stack. Designed to be small, focused, CRAN-quality, and
`EBImage`-license-compatible (LGPL-3).

## Pipeline shape

    matrix (logical / integer / numeric)
            │
            v
    as_binary_matrix() ──> IntegerMatrix with {0, 1} values
            │
            v
       .<method>_cpp(mat, max_iter) ──> IntegerMatrix skeleton
            │
            v
    restore_storage(skeleton, original) ──> matrix in original storage mode

The dispatcher
([`thin()`](https://humanpred.github.io/thinr/reference/thin.md))
selects the C++ implementation by name and handles the coercion
sandwich. The C++ implementations are isolated by file, one per
algorithm.

## Algorithm contents

| File | Algorithm | Status | Notes |
|----|----|----|----|
| `src/zhang_suen.cpp` | Zhang & Suen (1984) | Full | Two sub-iterations per pass; standard parallel thinning. |
| `src/guo_hall.cpp` | Guo & Hall (1989) | Full | Two sub-iterations; uses the OpenCV-canonical C(p) and N(p) formulations. |
| `src/lee.cpp` | Lee (1994) | Stub | 3-D capable; Euler-invariance check. v0.2. |
| `src/k3m.cpp` | Saeed et al. (2010) | Stub | Six-phase look-up-table thinning with strong corner preservation. v0.2. |
| `src/RcppExports.cpp` | (generated) | n/a | Regenerate via [`Rcpp::compileAttributes()`](https://rdrr.io/pkg/Rcpp/man/compileAttributes.html). |

## Design constraints

1.  **Minimal dependencies.** Rcpp is the only Imports entry. The
    package is CRAN-targeted and should install cleanly on Linux, macOS,
    and Windows without system libraries beyond the C++ toolchain.

2.  **API stability.**
    [`thin()`](https://humanpred.github.io/thinr/reference/thin.md) is
    the public thinning surface. Any signature change is a major version
    bump. The default algorithm is locked to Zhang-Suen.

3.  **Stubs error, don’t silently fall back.** Lee and K3M throw with a
    message that names the planned version. Silent fall-backs to
    Zhang-Suen would hide the limitation.

4.  **2-D for now.** Higher-dimensional arrays are explicitly rejected.
    The Lee implementation in v0.2 introduces 3-D support; the
    dispatcher will route based on `length(dim(image))`.

## Versioning

- Independent semver, on a CRAN-friendly cadence (major releases ≤ once
  per year).
- LGPL-3 is the chosen license per ADR-007 for EBImage compatibility.
- Slice tags in the figureextract meta-repo reference a specific `thinr`
  version (e.g. `thinr 0.1.0` for slice-0).

## CRAN submission expectations

- 0 errors / 0 warnings / NOTEs limited to “new submission” /
  “version-large-components”.
- Vignette builds in under 30 seconds.
- Total package size (source) under 5 MB.
- Reverse-dependency check via `revdepcheck::revdep_check()` once
  `figureextractraster` (the first internal consumer) lands.

## Open questions

- Whether `Lee` should default to the 2-D mode when given a 2-D matrix,
  or whether the 3-D / 2-D distinction should be an explicit argument.
- Whether
  [`thin()`](https://humanpred.github.io/thinr/reference/thin.md) should
  accept `EBImage::Image` objects directly (currently it accepts the
  underlying matrix; adding `Image` support would create a soft
  `Suggests` link to `EBImage`).
- Performance: the current implementation uses `IntegerMatrix` element
  access. A bit-packed representation could be 2-4x faster on large
  images, at the cost of more complex code. Defer until benchmarks show
  it matters.
