# CLAUDE.md — thinr

> Working rules for Claude Code (and any human collaborator) when
> editing this package. The figureextract meta-repo’s `ARCHITECTURE.md`
> and `DECISIONS.md` describe the wider ecosystem; this file is the
> per-package complement.

## Package responsibility

`thinr` provides binary image thinning (skeletonization) algorithms —
Zhang-Suen, Guo-Hall, Lee (2-D), K3M, the parallel form commonly
attributed to Hilditch, OPTA / SPTA, and Holt — behind a single
dispatching API. Also provides the medial axis transform (Blum 1967) and
a fast distance transform (Felzenszwalb-Huttenlocher 2012; classic
two-pass sweep). `EBImage` provides binary morphology but no thinning
operator, so `thinr` complements it rather than replacing any of its
functions. Per ADR-007 this is the **one public CRAN package** in the
figureextract ecosystem; LGPL-3 is chosen for EBImage compatibility.

## Current state

- **Slice:** 0 — Infrastructure
- **Version:** 0.3.0.9000 (as of 2026-07-28, from `DESCRIPTION`) — a
  development version on top of the published CRAN release 0.3.0. The
  `.9000` suffix was added *because* 0.3.0 is live on CRAN: before the
  bump, a local `rcmdcheck(args = "--as-cran")` reported “Insufficient
  package version (submitted: 0.3.0, existing: 0.3.0)” from the live
  CRAN incoming-feasibility check — correct behaviour for a repeat
  submission of a published version, but a confusing WARNING for anyone
  checking locally. Bump the version again (to `0.4.0` or `0.3.1`) at
  the next CRAN submission.
- **Status:** Published CRAN release (0.3.0) plus unreleased development
  work. Seven thinning algorithms fully implemented in Rcpp (as of
  2026-07-28: `zhang_suen`, `guo_hall`, `lee`, `k3m`, `hilditch`,
  `opta`, `holt` — the `tests/testthat/test-thin.R` `methods` vector),
  all verified against original papers (or the Lam-Lee-Suen 1992 survey
  for Hilditch’s parallel form). Medial axis and distance transform
  shipped as standalone exported utilities. Tests pass (510 assertions,
  `devtools::test()`); lintr clean. GitHub Actions: R-CMD-check
  (matrix), pkgdown, test-coverage, lint, pr-commands.
- **Recent shipments:**
  - **thinr 0.3.0.9000 — F018 kernel de-duplication** (2026-07-28) —
    `src/zhang_suen.cpp`, `src/lee.cpp`, and `src/holt.cpp` now
    `#include "thinr_common.h"` and call `thinr::crossing_number()` /
    `thinr::neighbour_count()` instead of re-implementing them inline,
    joining `src/hilditch.cpp`. **The de-duplication is the gate**:
    before it, mutating the shared definition in `thinr_common.h`
    changed only `hilditch`’s output; after it, the same mutation
    reaches `zhang_suen`, `lee`, `holt`, and `hilditch` — so a future
    correctness fix to the shared helpers can no longer silently miss
    three of the four kernels that need it. Behaviour-identical, proven
    rather than argued: the pre- and post-change builds were installed
    into separate libraries and compared over 257 binary images × 7
    methods × 5 `max_iter` values ×
    {[`thin()`](https://humanpred.github.io/thinr/reference/thin.md),
    raw kernel} plus
    [`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md)
    and all 3
    [`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md)
    metrics — 19,172 result slots, all
    [`identical()`](https://rdrr.io/r/base/identical.html), same
    serialisation MD5. `guo_hall.cpp` and `k3m.cpp` are deliberately
    **not** included: they implement different connectivity criteria
    (Guo-Hall’s `C(p)`/`N1`/`N2`, K3M’s lookup-table weights), not the
    Zhang-Suen crossing number.
  - **thinr 0.3.0 — 2026-07-03 ecosystem review fix wave** (as of
    2026-07-28; see `NEWS.md` and `REVIEW-2026-07-28.md` for detail) —
    F012 (edge/border pixels now thinned via zero-pad-then-crop, all 7
    methods), F011 (OPTA no longer disconnects a 2px-thick stroke via
    sequential per-direction sub-iterations), F015
    ([`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md)
    returns `Inf`, not a finite sentinel, on an all-foreground image,
    for `manhattan`/`chessboard` matching `euclidean`), F016 (`NA`
    rejected with a clear error at the `as_binary_matrix()` coercion
    boundary instead of silently becoming `INT_MIN`), F014 (Hilditch
    look-ahead junction condition corrected to the published parallel
    form), F013 (added a connectivity-preservation property test across
    all 7 methods), F017 (documented + pinned the isolated-2x2-block
    method-dependent survival), and **removed `thinImage()` entirely**
    (it had described itself as an `EBImage::thinImage()`-equivalent, a
    function that has never existed in `EBImage`;
    [`thin()`](https://humanpred.github.io/thinr/reference/thin.md) is
    now the sole entry point) . F018
    (`zhang_suen.cpp`/`lee.cpp`/`holt.cpp` duplicating the shared
    `thinr::crossing_number()`/`neighbour_count()`) was confirmed still
    present by the 2026-07-28 review and deliberately held back from
    that documentation branch; it shipped separately as 0.3.0.9000 — see
    the entry above and `REVIEW-2026-07-28.md`.
  - **R-side coverage to its practical maximum** (2026-05-21, PR \#2,
    `1870e25`) — added tests covering the remaining branches in the
    [`thin()`](https://humanpred.github.io/thinr/reference/thin.md)
    dispatcher and the storage-restoration helpers.
  - **release-prep merged** (2026-05-20, PR \#1, `98f4a80`) — the
    CRAN-prep branch (with all reviewer-feedback fixes, the K3M / OPTA /
    Holt verifications, the Stentiford / Pavlidis drop, and pkgdown
    reference-index fix) landed on `main`.
  - Dropped `stentiford` (folk misattribution; the 1983 paper is
    preprocessing not thinning) and `pavlidis` (the implementation
    didn’t match the contour-following algorithm of the 1980 paper). The
    dropped algorithms are not implemented in any major image-processing
    library (scikit-image, OpenCV, MATLAB, ImageJ, mahotas); per the
    package’s “widely used elsewhere” inclusion criterion, removing them
    keeps the package focused. (2026-05-20)
  - Algorithm verification pass against papers in `references/`: K3M
    lookup tables corrected against Saeed et al. 2010; OPTA rewritten
    per survey’s safe-point expression; Holt rewritten per the original
    CACM paper (correcting a survey transcription error in the middle
    clause). (2026-05-20)
  - Tier-1 + Tier-2 algorithm expansion: Hilditch, OPTA, Holt; plus
    [`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md)
    and
    [`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md).
    (2026-05-16)
  - CRAN reviewer feedback addressed (function-name quotes + DOIs).
    (2026-05-16)
  - v0.2.0 stub-replacement: Lee 2-D and K3M fully implemented.
    (2026-05-16)
  - v0.1.0 skeleton with Rcpp setup, 2 algorithms, vignette, README,
    NEWS, GitHub Actions CI, pkgdown. (2026-05-16)

## Coding rules

- **Style:** tidyverse style guide.
  [`styler::style_pkg()`](https://styler.r-lib.org/reference/style_pkg.html)
  clean.
  [`lintr::lint_package()`](https://lintr.r-lib.org/reference/lint.html)
  clean per `.lintr` (line length 100, snake_case + CamelCase, no
  commented-code linter).
- **C++:** ISO C++17 baseline (Rcpp default for current R). Two-space
  indent. No `using namespace std`. Avoid Rcpp sugar in tight loops
  where it costs measurable performance.
- **R version:** ≥ 4.2.
- **Pipe:** native `|>` preferred.
- **Imports:** Rcpp only. Don’t add dependencies without a strong reason
  — this is a small, focused, CRAN-friendly package.

## Testing conventions

- **Framework:** `testthat` 3rd edition with the `describe`/`it` BDD
  style (legible test reports for a public package).
- **Coverage:** every algorithm in `methods <- c(...)` (currently seven,
  as of 2026-07-28, from `tests/testthat/test-thin.R` —
  `stentiford`/`pavlidis` were dropped 2026-05-20, see below) is
  exercised against the same property suite (solid square thins, line
  collapse, idempotence, empty input, isolated-pixel preservation, ring
  topology, connectivity-preservation, edge-handling). New methods are
  added to the vector and inherit all tests automatically.
- **`distance_transform` and `medial_axis`** each have their own test
  files (`tests/testthat/test-distance-transform.R`,
  `test-medial-axis.R`).
- **Acceptance:**
  `rcmdcheck::rcmdcheck(args = c("--no-manual", "--as-cran"), error_on = "warning")`
  locally reports 0 errors and 0 warnings (verified 2026-07-28 on
  `0.3.0.9000`). Two NOTEs are expected and acceptable: the Ubuntu
  system compilation-flags note (`-mno-omit-leaf-frame-pointer`, from
  the R installation, not from this package) and CRAN incoming
  feasibility’s “Version contains large components” for the `.9000`
  development suffix. A `Non-standard file/directory found at top level`
  NOTE means a new root-level doc needs a `.Rbuildignore` entry — add it
  rather than accepting the NOTE. The GitHub Actions `R-CMD-check.yaml`
  gate is the authoritative signal and is green on `main`.
- **Checking from a `git worktree`:** `R CMD build` excludes a `.git`
  *directory* but not the `.git` *file* a worktree uses, so `--as-cran`
  used to emit a spurious “hidden files and directories: .git” NOTE from
  a worktree that a normal clone never showed. `.Rbuildignore` now
  carries `^\.git$` so worktree checks match clone checks.

## Module boundaries

C++ sources in `src/`:

- `thinr_common.h` — shared inline helpers (`crossing_number`,
  `neighbour_count`).
- `zhang_suen.cpp` — Zhang & Suen (1984).
- `guo_hall.cpp` — Guo & Hall (1989).
- `lee.cpp` — Lee, Kashyap & Chu (1994), 2-D adaptation.
- `k3m.cpp` — Saeed et al. (2010); paper lookup tables reproduced
  verbatim.
- `hilditch.cpp` — parallel form commonly attributed to Hilditch (1969);
  the actual implementation follows the Rutovitz-style R1–R4 conditions
  as documented in Lam, Lee & Suen (1992).
- `opta.cpp` — Naccache & Shinghal (1984), Safe Point Thinning
  Algorithm; boolean safe-point expression follows the survey.
- `holt.cpp` — Holt, Stewart, Clint & Perrott (1987); condition H
  follows the original CACM paper (a survey transcription error was
  caught and corrected against the original).
- `distance_transform.h` + `distance_transform.cpp` —
  Felzenszwalb-Huttenlocher 2012 squared Euclidean + Rosenfeld-Pfaltz
  two-pass sweep for L1 and L∞.
- `medial_axis.cpp` — ridge detection on the squared Euclidean DT.
- `RcppExports.cpp` — auto-generated; do not edit (regenerated by
  [`Rcpp::compileAttributes()`](https://rdrr.io/pkg/Rcpp/man/compileAttributes.html)).

R sources in `R/`:

- `thin.R` —
  [`thin()`](https://humanpred.github.io/thinr/reference/thin.md)
  dispatching function and the `as_binary_matrix()` /
  `restore_storage()` coercion helpers.
- `distance_transform.R` — exported wrapper for
  `.distance_transform_cpp`.
- `medial_axis.R` — exported wrapper for `.medial_axis_cpp`.
- `thinr-package.R` — package-level Roxygen doc.
- `RcppExports.R` — auto-generated; do not edit.

## Public API surface

- Exported:
  [`thin()`](https://humanpred.github.io/thinr/reference/thin.md),
  [`medial_axis()`](https://humanpred.github.io/thinr/reference/medial_axis.md),
  [`distance_transform()`](https://humanpred.github.io/thinr/reference/distance_transform.md).
- Internal: `as_binary_matrix()`, `restore_storage()` (helpers; not
  exported).

## Extension points

To add a new thinning algorithm:

1.  Add `src/<algorithm>.cpp` exporting
    `.<algorithm>_cpp(IntegerMatrix, int)`. Use the helpers in
    `thinr_common.h`.
2.  Run `Rcpp::compileAttributes(".")` so `R/RcppExports.R` is
    regenerated.
3.  Add the method name to the `match.arg` list and the
    [`switch()`](https://rdrr.io/r/base/switch.html) in `R/thin.R`.
4.  Add the method to the `methods` vector at the top of
    `tests/testthat/test-thin.R` — all property tests apply
    automatically.
5.  Update `NEWS.md`, the algorithms table in `README.md`, the
    algorithms section in `R/thinr-package.R`, and the algorithms table
    in `vignettes/choosing-a-method.Rmd`.
6.  Add the published reference to `DESCRIPTION` Description field if it
    has a DOI.

## Documentation requirements

CRAN package documentation conventions:

- Every exported function has Roxygen `@param`, `@return`, `@examples`.
- Examples are runnable (`R CMD check` runs them with `--run-donttest`).
- The vignette is non-trivial; it shows a real comparison of algorithms.
- The README has install instructions and a usage snippet.
- NEWS.md follows the standard “version - bullet list” format.
- `cran-comments.md` is updated on every CRAN submission.

## Workflow rules

### Safe to auto-approve

- `R CMD check`, `devtools::check()`, `devtools::test()`,
  `devtools::document()`.
- [`Rcpp::compileAttributes()`](https://rdrr.io/pkg/Rcpp/man/compileAttributes.html),
  `roxygen2::roxygenize()`.
- [`lintr::lint_package()`](https://lintr.r-lib.org/reference/lint.html),
  [`styler::style_pkg()`](https://styler.r-lib.org/reference/style_pkg.html),
  [`covr::package_coverage()`](http://covr.r-lib.org/reference/package_coverage.md).
- [`pkgdown::build_site()`](https://pkgdown.r-lib.org/reference/build_site.html),
  `usethis::use_*()` helpers that only edit local files.
- Reading any file; `ls`, `git status`, `git log`, `git diff`.

### Requires explicit confirmation

- Adding / removing dependencies (this package wants to stay small).
- Renaming / removing exported functions (breaking change).
- Bumping the major version.
- Modifying CI configuration.
- Any `git` commit, tag, branch, or push.
- Submitting to CRAN (`devtools::submit_cran()`, `devtools::release()`).
- Modifying the LGPL-3 license declaration.

### Never

- Force-push.
- Commit secrets.
- Modify git config.
- Skip git hooks.
- Destructive resets without per-instance approval.

## Process rules

- Multi-step work: propose a plan, wait for explicit approval.
- Surface suggestions; don’t implement without approval.
- Convert relative dates to absolute when committing or writing docs.

## License

**LGPL-3**, per ADR-007 and ADR-017. Public CRAN release.

- The LGPL-3 license is the same as EBImage’s, so EBImage can depend on
  `thinr` without relicensing concerns.
- Source files do not carry per-file copyright headers (CRAN
  convention); the LGPL-3 text is included by R’s standard license
  machinery via `License: LGPL-3` in `DESCRIPTION`.
- Contributors retain copyright in their contributions, licensed under
  LGPL-3.
