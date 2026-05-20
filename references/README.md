# Reference papers (not committed)

This directory holds PDF copies of the algorithm papers that `thinr` implements. **The PDFs themselves are intentionally not committed to git** because we do not hold redistribution licenses for them. They are also excluded from `R CMD build` via `.Rbuildignore`, so they will never ship with the package.

Drop relevant paper PDFs into this directory when working on `thinr`. They will not be staged, pushed, or installed.

## What's blocked from git

`.gitignore` ignores everything in this directory except this README:

```
/references/*
!/references/README.md
```

Anything else you place here — `*.pdf`, `*.djvu`, transcripts, notes — is invisible to git. If you ever need to verify, run `git status` after dropping a file; it should still report a clean working tree.

## Suggested filename convention

```
<lastname>-<year>-<short-slug>.<ext>
```

For example:

- `holt-1987-improved-parallel-thinning.pdf`
- `pavlidis-1980-discrete-binary-thinning.pdf`
- `lam-lee-suen-1992-thinning-survey.pdf`
- `naccache-shinghal-1984-opta.pdf`
- `stentiford-mortimer-1983-ocr-thinning.pdf`
- `hilditch-1969-linear-skeletons.pdf` (book chapter, if scanned)
- `saeed-et-al-2010-k3m.pdf`

If the paper has multiple useful artefacts (the paper itself, an accompanying tech report, slides, errata), put them in a sub-folder named for the paper:

```
references/
├── holt-1987-improved-parallel-thinning/
│   ├── holt-1987-improved-parallel-thinning.pdf
│   └── notes.md
└── pavlidis-1980-discrete-binary-thinning.pdf
```

## How this gets used

When updating an algorithm's `.cpp` source against its published form, Claude (or a human contributor) can reference the local PDF for the exact conditions / lookup tables / pseudocode, then update the implementation, tests, and source-header citation. Once the implementation is verified against the paper, the source-header "reviewers familiar with the original publication are invited to verify" caveat should be replaced with a verified-against-source acknowledgement.

## Papers most useful to have

In priority order (highest leverage first):

1. **Lam, Lee & Suen (1992)** — "Thinning methodologies — a comprehensive survey", IEEE TPAMI 14(9):869–885. Covers Holt, Stentiford, Pavlidis, OPTA, Hilditch in one survey.
2. **Holt et al. (1987)** — "An improved parallel thinning algorithm", CACM 30(2):156–160.
3. **Pavlidis (1980)** — "A thinning algorithm for discrete binary images", CGIP 13(2):142–157.
4. **Naccache & Shinghal (1984)** — "An investigation into the skeletonization approach of Hilditch", Pattern Recognition 17(3):279–284.
5. **Saeed et al. (2010)** — open access at Sciendo; useful for verifying the K3M lookup tables.
6. **Stentiford & Mortimer (1983)** — IEEE TSMC 13(1):81–84.
7. **Hilditch (1969)** — book chapter in *Machine Intelligence 4*.
8. **Lee, Kashyap & Chu (1994)** — confirms the 2-D specialization of the 3-D algorithm.
