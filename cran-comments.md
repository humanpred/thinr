## Submission

This is a new release: thinr 0.1.0.

## Test environments

- local Ubuntu 24.04, R 4.6.0
- GitHub Actions (planned):
  - macOS-latest, R-release
  - windows-latest, R-release
  - ubuntu-latest, R-devel / R-release / R-oldrel-1
- win-builder (planned for the actual submission run)

## R CMD check --as-cran results

0 errors, 0 warnings, 1-2 NOTEs:

- "New submission" — expected.
- "Maintainer was changed" / "Days since last update" — not applicable for a first submission.

## Downstream dependencies

None at first submission. Internal use is planned in the figureextract ecosystem (a separate proprietary set of packages); CRAN reverse-dependency check will be re-run when those packages are public, if ever.

## Notes for the submission reviewer

- `EBImage::thinImage()` provides a Zhang-Suen implementation; `thinr::thinImage()` is a signature-compatible drop-in. Mentioning `EBImage` in the description is informational; no `Imports` or `Suggests` link to it.
- Two of the four algorithms named in the description (Lee, K3M) ship as stubs in v0.1.0 that error with an informative message; they will be replaced with full implementations in v0.2.0. This is documented in `NEWS.md` and the vignette.
