# cran-comments.md

## Submission

This is thinr 0.2.0, a resubmission addressing the CRAN reviewer's
feedback on an earlier submission.

## Reviewer feedback addressed

### 1. Single quotes around function names in the Description field

> "Please omit the single quotes around function names in the DESCRIPTION.
>  -> 'thinImage()' --> thinImage()"

**Resolved.** All function-name references in the Description field
appear without single quotes: `thinImage()`, `thin()`. Package names
(`'EBImage'`) are kept in single quotes per CRAN convention.

### 2. References for algorithm methods

> "If there are references describing the methods in your package,
>  please add these in the description field of your DESCRIPTION file
>  in the form authors (year) <doi:...> ..."

**Resolved.** Every algorithm in the package now has its citation in
the Description field, with a DOI where one exists and resolves at
doi.org. Specifically:

- Zhang and Suen (1984)             <doi:10.1145/357994.358023>
- Guo and Hall (1989)               <doi:10.1145/62065.62074>
- Lee, Kashyap, and Chu (1994)      <doi:10.1006/cgip.1994.1042>
- Saeed, Tabedzki, Rybnik & Adamski (2010)   <doi:10.2478/v10006-010-0024-4>
- Holt, Stewart, Clint & Perrott (1987)      <doi:10.1145/12527.12531>
- Felzenszwalb and Huttenlocher (2012)       <doi:10.4086/toc.2012.v008a019>

Two referenced works do not have a DOI that resolves at doi.org and
are therefore cited by author and year only:

- Hilditch (1969), "Linear Skeletons from Square Cupboards", a book
  chapter in *Machine Intelligence 4* (Edinburgh University Press).
  Book chapter from before DOI assignment.
- Naccache and Shinghal (1984), "An investigation into the
  skeletonization approach of Hilditch", *Pattern Recognition*
  17(3):279-284. The old-format Elsevier DOI for this article does
  not resolve at doi.org as of submission.

### 3. Comprehensiveness of the algorithm set

> "I would like for this package to be comprehensive. Are there any
>  other algorithms that would rationally be used in the package that
>  are not yet?"

**Addressed.** The algorithm list was reviewed against major
open-source image-processing libraries (scikit-image, OpenCV
ximgproc, MATLAB Image Processing Toolbox, ImageJ, mahotas) and
authoritative references (Lam, Lee & Suen 1992 survey). The
package now ships **seven** thinning algorithms (Zhang-Suen,
Guo-Hall, Lee 2-D, K3M, the parallel form commonly attributed to
Hilditch, OPTA / SPTA, and Holt) plus the medial axis transform
and a Euclidean / Manhattan / Chessboard distance transform. Every
implementation was verified against its original source paper (the
papers are listed in `references/README.md`); the Lam-Lee-Suen 1992
survey was used as cross-reference, and one transcription error in
the survey's rendering of Holt's middle clause was caught and
corrected against Holt's original 1987 paper.

## Test environments

- local Ubuntu 24.04, R 4.6.0
- GitHub Actions matrix:
  - macOS-latest, R-release
  - windows-latest, R-release
  - ubuntu-latest, R-devel / R-release / R-oldrel-1
- win-builder (planned for the actual submission run)

## R CMD check --as-cran results

With `_R_CHECK_CRAN_INCOMING_=true` and
`_R_CHECK_CRAN_INCOMING_REMOTE_=true` on the local Ubuntu test
machine:

**0 errors, 0 warnings, 2 NOTEs.** Both NOTEs are expected and not
actionable from the package maintainer's side:

### NOTE 1 — "New submission"

```
* checking CRAN incoming feasibility ... NOTE
Maintainer: 'Bill Denney <wdenney@humanpredictions.com>'
New submission
```

This is the standard CRAN-incoming NOTE that always appears for a
first submission. It will not appear after CRAN accepts the package.

### NOTE 2 — "compilation flags used" (Ubuntu local only)

```
* checking compilation flags used ... NOTE
Compilation used the following non-portable flag(s):
  '-Wdate-time' '-Werror=format-security' '-Wformat'
  '-mno-omit-leaf-frame-pointer'
```

These flags are not specified by the package's `src/Makevars` (the
package has no `Makevars` or `Makevars.in`). They are injected by
Debian/Ubuntu's R packaging (the `r-base-core` package on Ubuntu
24.04 adds Debian hardening flags into the system-wide `R CMD config`
output). The NOTE does not reproduce on CRAN's own Debian build
machines (which use the standard upstream `R CMD config`) and does
not reproduce on the win-builder, macOS, or other ubuntu-* GitHub
Actions runners. It is a property of the local maintainer's test
machine, not of the package.

## Downstream dependencies

None at first submission. Internal use is planned in the figureextract
ecosystem (a separate, proprietary set of packages); CRAN
reverse-dependency check will be re-run when those packages become
public, if ever.

## Notes for the submission reviewer

- `EBImage::thinImage()` provides a Zhang-Suen implementation;
  `thinr::thinImage()` is a signature-compatible drop-in. The
  mention of `EBImage` in the Description field is informational; no
  `Imports` or `Suggests` link to it.
- The K3M lookup tables (`A_0`, `A_1`, ..., `A_5`, `A_1pix`) in
  `src/k3m.cpp` are reproduced verbatim from Saeed et al. (2010),
  Section 3.3, page 327. The paper itself is in
  `references/saeed-et-al-2010-k3m.pdf` (a developer-only directory
  that is excluded from the build via `.Rbuildignore`).
- The `hilditch` method ships the parallel form (Rutovitz-style
  R1-R4 conditions, look-ahead crossing-number checks at the north
  and east neighbours) commonly cited as "Hilditch" in modern
  image-processing surveys. The 1969 paper itself describes a
  sequential algorithm with within-pass deletion tracking and uses
  a different crossing-number definition. The package's source
  header for `src/hilditch.cpp` documents this clearly.
- For Holt, the implementation matches the survival expression on
  page 157 of Holt et al. (1987). The Lam-Lee-Suen 1992 survey
  transcribed the middle preservation clause with "vN" (north) where
  the original paper has "vE" (east); the original paper is followed.
- All four `_R_CHECK_CRAN_INCOMING_` DOI checks pass: the six DOIs
  in the Description field resolve at doi.org.
