# Properties every thinning algorithm should satisfy on simple inputs.
# Run for all implemented methods.

methods <- c("zhang_suen", "guo_hall", "lee", "k3m",
             "hilditch", "opta", "holt")

# Count 8-connected foreground components via flood fill; used by the
# connectivity-preservation tests (thinning may only delete pixels, so
# the component count must never change).
count_components <- function(img) {
  visited <- matrix(FALSE, nrow = nrow(img), ncol = ncol(img))
  n <- 0L
  for (r0 in seq_len(nrow(img))) {
    for (c0 in seq_len(ncol(img))) {
      if (img[r0, c0] == 0 || visited[r0, c0]) next
      n <- n + 1L
      queue <- list(c(r0, c0))
      while (length(queue) > 0) {
        pt <- queue[[1]]
        queue <- queue[-1]
        r <- pt[1]
        c <- pt[2]
        if (r < 1 || r > nrow(img) || c < 1 || c > ncol(img)) next
        if (visited[r, c]) next
        if (img[r, c] == 0) next
        visited[r, c] <- TRUE
        for (dr in -1L:1L) {
          for (dc in -1L:1L) {
            if (dr != 0L || dc != 0L) {
              queue <- c(queue, list(c(r + dr, c + dc)))
            }
          }
        }
      }
    }
  }
  n
}

# Rasterize a circular ring of the given radius / stroke thickness;
# used by the connectivity and complex-shape tests.
build_ring <- function(nrow, ncol, radius, cr, cc, thickness = 1) {
  img <- matrix(0L, nrow = nrow, ncol = ncol)
  for (angle in seq(0, 2 * pi, length.out = 200)) {
    r <- round(cr + radius * cos(angle))
    c <- round(cc + radius * sin(angle))
    for (dr in -thickness:thickness) {
      for (dc in -thickness:thickness) {
        rr <- r + dr
        cc2 <- c + dc
        if (rr >= 1 && rr <= nrow && cc2 >= 1 && cc2 <= ncol) {
          img[rr, cc2] <- 1L
        }
      }
    }
  }
  img
}

describe("solid square thins to a much smaller skeleton", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 9, ncol = 9)
        img[3:7, 3:7] <- 1L
        sk <- thin(img, method = m)
        expect_lte(sum(sk), sum(img))
        expect_lt(sum(sk), sum(img) / 2)
      })
    })
  }
})

describe("horizontal line collapses to (nearly) a single row", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 5, ncol = 11)
        img[2:4, 2:10] <- 1L
        sk <- thin(img, method = m)
        rows_with_fg <- which(rowSums(sk) > 0)
        # OPTA's N2 condition protects diagonal-2-neighbour patterns,
        # which preserves bar corner pixels on the top and bottom row -
        # this is the published behaviour, not an implementation choice.
        # Every other method collapses the bar to a single row.
        max_rows <- if (m == "opta") 3L else 1L
        expect_lte(length(rows_with_fg), max_rows,
                   label = paste("method =", m,
                                 "; rows with foreground =",
                                 paste(rows_with_fg, collapse = ",")))
      })
    })
  }
})

describe("a 2px-thick bar keeps a single connected skeleton", {
  # A two-pixel-thick stroke is the canonical parallel-deletion trap: a
  # batch pass that tests both sides against the pre-pass state can
  # delete both sides at once and disconnect the shape. Sequential /
  # sub-iteration algorithms must keep the bar in one piece.
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 6, ncol = 13)
        img[3:4, 3:11] <- 1L
        sk <- thin(img, method = m)
        expect_identical(count_components(img), 1L)
        expect_identical(count_components(sk), 1L)
      })
    })
  }
})

describe("thinning preserves the 8-connected component count", {
  # Thinning only deletes pixels, so it can split a component but never
  # merge two; preserving the count exactly is the defining topological
  # invariant of every method in the package.
  shape_2px_hbar <- matrix(0L, nrow = 8, ncol = 15)
  shape_2px_hbar[4:5, 3:13] <- 1L
  shape_3px_hbar <- matrix(0L, nrow = 9, ncol = 15)
  shape_3px_hbar[4:6, 3:13] <- 1L
  shape_2px_vbar <- matrix(0L, nrow = 15, ncol = 8)
  shape_2px_vbar[3:13, 4:5] <- 1L
  shape_3px_vbar <- matrix(0L, nrow = 15, ncol = 9)
  shape_3px_vbar[3:13, 4:6] <- 1L
  shape_l <- matrix(0L, nrow = 13, ncol = 13)
  shape_l[3:11, 3:4] <- 1L
  shape_l[10:11, 3:11] <- 1L
  shape_t <- matrix(0L, nrow = 11, ncol = 11)
  shape_t[3:5, 3:9] <- 1L
  shape_t[5:9, 5:7] <- 1L
  shape_plus <- matrix(0L, nrow = 11, ncol = 11)
  shape_plus[5:7, 3:9] <- 1L
  shape_plus[3:9, 5:7] <- 1L
  shape_blobs <- matrix(0L, nrow = 9, ncol = 15)
  shape_blobs[2:4, 2:4] <- 1L
  shape_blobs[6:8, 11:13] <- 1L
  shapes <- list(
    "2px horizontal bar" = list(img = shape_2px_hbar, n = 1L),
    "3px horizontal bar" = list(img = shape_3px_hbar, n = 1L),
    "2px vertical bar"   = list(img = shape_2px_vbar, n = 1L),
    "3px vertical bar"   = list(img = shape_3px_vbar, n = 1L),
    "L-shape"            = list(img = shape_l, n = 1L),
    "T-shape"            = list(img = shape_t, n = 1L),
    "plus-shape"         = list(img = shape_plus, n = 1L),
    "two disjoint blobs" = list(img = shape_blobs, n = 2L),
    "thick ring"         = list(
      img = build_ring(21, 21, radius = 7, cr = 11, cc = 11, thickness = 2),
      n = 1L
    )
  )
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        for (shape_name in names(shapes)) {
          img <- shapes[[shape_name]]$img
          n <- shapes[[shape_name]]$n
          expect_identical(count_components(img), n,
                           label = paste0("count_components(", shape_name, ")"))
          sk <- thin(img, method = m)
          expect_identical(
            count_components(sk), n,
            label = paste0("count_components(thin(", shape_name,
                           ", method = \"", m, "\"))")
          )
        }
      })
    })
  }
})

describe("thinning is idempotent", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 7, ncol = 11)
        img[3:5, 3:9] <- 1L
        once  <- thin(img,  method = m)
        twice <- thin(once, method = m)
        expect_equal(twice, once)
      })
    })
  }
})

describe("all-background image is unchanged", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 5, ncol = 5)
        expect_equal(thin(img, method = m), img)
      })
    })
  }
})

describe("a single isolated foreground pixel is preserved (endpoint)", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 5, ncol = 5)
        img[3, 3] <- 1L
        expect_equal(thin(img, method = m), img)
      })
    })
  }
})

describe("topology is preserved on a small ring (hole stays a hole)", {
  # Holt's H condition does not include a crossing-number topology
  # guard; the survey notes it is specifically designed to prevent
  # 2-pixel-wide line disappearance, not arbitrary topology. Ring
  # preservation is therefore not guaranteed for Holt; we skip it.
  topology_methods <- setdiff(methods, "holt")
  count_holes_present <- function(img) {
    visited <- matrix(FALSE, nrow = nrow(img), ncol = ncol(img))
    queue <- list(c(1L, 1L))
    while (length(queue) > 0) {
      pt <- queue[[1]]
      queue <- queue[-1]
      r <- pt[1]
      c <- pt[2]
      if (r < 1 || r > nrow(img) || c < 1 || c > ncol(img)) next
      if (visited[r, c]) next
      if (img[r, c] != 0) next
      visited[r, c] <- TRUE
      queue <- c(queue,
                 list(c(r - 1L, c), c(r + 1L, c), c(r, c - 1L), c(r, c + 1L)))
    }
    any(img == 0 & !visited)
  }
  for (mth in topology_methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 11, ncol = 11)
        img[2:10, 2:10] <- 1L
        img[5:7,  5:7]  <- 0L
        expect_true(count_holes_present(img), info = "starting image has a hole")
        sk <- thin(img, method = m)
        expect_true(count_holes_present(sk),
                    info = paste("thinned image (method =", m,
                                 ") should still have a hole"))
      })
    })
  }
})

describe("input coercion", {
  it("accepts a logical matrix and returns a logical matrix", {
    img <- matrix(FALSE, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- TRUE
    sk <- thin(img, method = "zhang_suen")
    expect_type(sk, "logical")
    expect_equal(dim(sk), dim(img))
  })

  it("accepts a numeric matrix and returns a numeric matrix", {
    img <- matrix(0, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1
    sk <- thin(img, method = "zhang_suen")
    expect_type(sk, "double")
    expect_equal(dim(sk), dim(img))
  })

  it("treats non-zero numeric values as foreground", {
    img <- matrix(0, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 0.7
    sk <- thin(img, method = "zhang_suen")
    expect_gt(sum(sk), 0)
  })

  it("accepts an integer matrix and returns an integer matrix", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1L
    sk <- thin(img, method = "zhang_suen")
    expect_type(sk, "integer")
    expect_equal(dim(sk), dim(img))
  })

  it("treats any non-zero integer (not just 1) as foreground", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 255L
    sk <- thin(img, method = "zhang_suen")
    expect_gt(sum(sk), 0)
    # The output collapses to {0, 1} regardless of the input foreground value.
    expect_true(all(sk %in% c(0L, 1L)))
  })
})

describe("input validation", {
  it("errors on an unknown method", {
    img <- matrix(0L, nrow = 3, ncol = 3)
    expect_error(thin(img, method = "nope"))
  })

  it("errors on a matrix of unsupported storage mode (character)", {
    img <- matrix("a", nrow = 3, ncol = 3)
    expect_error(thin(img), "does not know how to interpret")
  })

  it("errors on a complex-mode matrix", {
    img <- matrix(complex(real = 0, imaginary = 0), nrow = 3, ncol = 3)
    expect_error(thin(img), "does not know how to interpret")
  })

  it("errors on a 1-D array", {
    img <- array(c(1L, 1L, 0L), dim = 3)
    expect_error(thin(img), "expects a 2-D matrix")
  })

  it("errors on NA in a numeric matrix", {
    img <- matrix(c(NA, 1, 0, 1), nrow = 2)
    expect_error(thin(img), "does not accept NA values", fixed = TRUE)
  })

  it("errors on NA in a logical matrix", {
    img <- matrix(c(NA, TRUE, FALSE, TRUE), nrow = 2)
    expect_error(thin(img), "does not accept NA values", fixed = TRUE)
  })

  it("errors on NA in an integer matrix", {
    img <- matrix(c(NA_integer_, 1L, 0L, 1L), nrow = 2)
    expect_error(thin(img), "does not accept NA values", fixed = TRUE)
  })

  it("errors on a 3-D array", {
    img <- array(0L, dim = c(3, 3, 2))
    expect_error(thin(img), "expects a 2-D matrix")
  })
})

describe("max_iter", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("max_iter = 0 returns input unchanged [", m, "]"), {
        img <- matrix(0L, nrow = 9, ncol = 9)
        img[3:7, 3:7] <- 1L
        sk <- thin(img, method = m, max_iter = 0L)
        expect_equal(sk, img)
      })
      it(paste0("non-integer max_iter is coerced to integer [", m, "]"), {
        # The dispatcher does `as.integer(max_iter)`, so a double is accepted.
        img <- matrix(0L, nrow = 9, ncol = 9)
        img[3:7, 3:7] <- 1L
        sk_int <- thin(img, method = m, max_iter = 50L)
        sk_dbl <- thin(img, method = m, max_iter = 50)
        expect_equal(sk_dbl, sk_int)
      })
    })
  }
})

describe("already-thin skeleton is a fixed point of thin()", {
  # Idempotence on a one-pixel-wide skeleton is a stronger property than
  # plain idempotence: any further pass must leave the skeleton unchanged.
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "] horizontal single-row line"), {
        img <- matrix(0L, nrow = 5, ncol = 9)
        img[3, 2:8] <- 1L
        expect_equal(thin(img, method = m), img)
      })
      it(paste0("[", m, "] vertical single-column line"), {
        img <- matrix(0L, nrow = 9, ncol = 5)
        img[2:8, 3] <- 1L
        expect_equal(thin(img, method = m), img)
      })
      it(paste0("[", m, "] diagonal single-pixel-wide line"), {
        img <- matrix(0L, nrow = 7, ncol = 7)
        for (i in 2:6) img[i, i] <- 1L
        expect_equal(thin(img, method = m), img)
      })
    })
  }
})

describe("output has no foreground pixels on the matrix border", {
  # Thinning only deletes pixels, so an input with a one-pixel
  # background margin must keep that margin clear. We test a
  # representative shape against each algorithm.
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 9, ncol = 9)
        img[3:7, 3:7] <- 1L
        sk <- thin(img, method = m)
        expect_equal(sum(sk[1, ]), 0L)
        expect_equal(sum(sk[nrow(sk), ]), 0L)
        expect_equal(sum(sk[, 1]), 0L)
        expect_equal(sum(sk[, ncol(sk)]), 0L)
      })
    })
  }
})

describe("small / degenerate inputs do not crash", {
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "] 1x1 empty matrix"), {
        img <- matrix(0L, nrow = 1, ncol = 1)
        expect_equal(thin(img, method = m), img)
      })
      it(paste0("[", m, "] 1x1 single-foreground matrix"), {
        img <- matrix(1L, nrow = 1, ncol = 1)
        # An isolated pixel is an endpoint, so it must be preserved.
        expect_equal(thin(img, method = m), img)
      })
      it(paste0("[", m, "] 3x3 matrix with one centre pixel"), {
        img <- matrix(0L, nrow = 3, ncol = 3)
        img[2, 2] <- 1L
        expect_equal(thin(img, method = m), img)
      })
    })
  }
})

describe("shapes with multiple endpoints", {
  # T-, L-, plus-, cross-shapes exercise corner / branchpoint detection
  # which is distinct from the straight-line code paths used elsewhere.
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "] T-shape collapses to a connected skeleton"), {
        img <- matrix(0L, nrow = 11, ncol = 11)
        img[3:5, 3:9] <- 1L  # top bar
        img[5:9, 5:7] <- 1L  # stem
        sk <- thin(img, method = m)
        expect_gt(sum(sk), 0L)
        expect_lt(sum(sk), sum(img))
        expect_identical(count_components(sk), 1L)
      })
      it(paste0("[", m, "] plus-shape collapses to a connected skeleton"), {
        img <- matrix(0L, nrow = 11, ncol = 11)
        img[5:7, 3:9] <- 1L
        img[3:9, 5:7] <- 1L
        sk <- thin(img, method = m)
        expect_gt(sum(sk), 0L)
        expect_lt(sum(sk), sum(img))
        expect_identical(count_components(sk), 1L)
      })
      it(paste0("[", m, "] disconnected components stay disconnected"), {
        img <- matrix(0L, nrow = 9, ncol = 11)
        img[2:4, 2:4] <- 1L
        img[6:8, 8:10] <- 1L
        sk <- thin(img, method = m)
        # Each component has at least one surviving foreground pixel,
        # and the two components neither merge nor vanish.
        expect_gt(sum(sk[1:5, 1:6]), 0L)
        expect_gt(sum(sk[5:9, 6:11]), 0L)
        expect_identical(count_components(sk), 2L)
      })
    })
  }
})

describe("complex shapes do not crash and yield smaller skeletons", {
  # Larger / more varied shapes drive iterations through more branches
  # of the underlying algorithms.
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "] thick ring"), {
        img <- build_ring(21, 21, radius = 7, cr = 11, cc = 11, thickness = 2)
        sk <- thin(img, method = m)
        expect_gt(sum(sk), 0L)
        expect_lt(sum(sk), sum(img))
      })
      it(paste0("[", m, "] notched square with internal voids"), {
        img <- matrix(0L, nrow = 15, ncol = 15)
        img[3:13, 3:13] <- 1L
        img[5:7, 5:7]  <- 0L  # internal hole
        img[9:11, 9:11] <- 0L  # second hole
        img[3:5, 6:8]  <- 0L  # notch from top edge
        sk <- thin(img, method = m)
        expect_gt(sum(sk), 0L)
        expect_lt(sum(sk), sum(img))
      })
    })
  }
})

describe("exact skeletons on small known shapes", {
  # The property tests above bound the skeleton size; these pin the
  # exact pixel pattern so a change in any algorithm's output is caught.
  it("zhang_suen collapses a 3x3 solid block to its single centre pixel", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1L
    expected <- matrix(0L, nrow = 5, ncol = 5)
    expected[3, 3] <- 1L
    expect_identical(thin(img, method = "zhang_suen"), expected)
  })

  it("guo_hall collapses a 3x3 solid block to its single centre pixel", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1L
    expected <- matrix(0L, nrow = 5, ncol = 5)
    expected[3, 3] <- 1L
    expect_identical(thin(img, method = "guo_hall"), expected)
  })

  it("hilditch uses the published look-ahead condition sense at junctions", {
    # Regression pin for the Hilditch condition-3/4 look-ahead sense.
    # The published parallel form skips deleting the centre p1 when a
    # cardinal neighbour p2/p4 has crossing number A == 1 on the CURRENT
    # image. An earlier implementation compared the look-ahead crossing
    # number (computed with p1 removed) against 1, which is strictly
    # stronger and also spared junction neighbours where A(p2) >= 2,
    # leaving a redundant pixel beside the junction. Here the redundant
    # pixel is [4, 3]: the corrected condition deletes it, the old one
    # kept it. Cross-checked exhaustively against a pure-R implementation
    # of the published form.
    img <- matrix(c(
      0, 1, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0,
      1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 0, 1,
      0, 1, 0, 0, 0, 0,
      1, 0, 1, 1, 0, 0
    ), nrow = 6, ncol = 6, byrow = TRUE)
    expected <- matrix(c(
      0, 1, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0,
      0, 0, 1, 1, 1, 1,
      0, 1, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0,
      1, 0, 1, 1, 0, 0
    ), nrow = 6, ncol = 6, byrow = TRUE)
    expect_identical(thin(img, method = "hilditch"), expected)
  })
})

describe("isolated 2x2 block: method-dependent survival", {
  # An isolated 2x2 block is a genuine point of divergence between the
  # parallel algorithms, and it matters for small-blob masks (a marker
  # remnant, a dotted-line dash). All four of its pixels satisfy the
  # zhang_suen deletion gate (B == 3, A == 1, both sub-iteration corner
  # products zero) simultaneously, so zhang_suen -- the default method --
  # erases the block entirely. guo_hall's sub-iteration m-condition keeps
  # exactly one pixel. This is documented in vignette("choosing-a-method")
  # and pinned here as a tripwire: if either count changes, the vignette
  # guidance is now wrong and must be updated with it.
  it("zhang_suen (the default) erases an isolated 2x2 block", {
    img <- matrix(0L, nrow = 6, ncol = 6)
    img[3:4, 3:4] <- 1L
    sk <- thin(img, method = "zhang_suen")
    expect_identical(sum(sk), 0L)
    expect_identical(sk, matrix(0L, nrow = 6, ncol = 6))
  })

  it("guo_hall keeps exactly one pixel of an isolated 2x2 block", {
    img <- matrix(0L, nrow = 6, ncol = 6)
    img[3:4, 3:4] <- 1L
    sk <- thin(img, method = "guo_hall")
    expect_identical(sum(sk), 1L)
  })
})

describe("shapes touching the matrix edge are thinned like interior shapes", {
  # A 3px-wide vertical bar spanning every row: the edge rows must not
  # be left at their full 3px width just because they sit on the matrix
  # border. Row widths are pinned exactly per method (OPTA's N2
  # condition keeps the corner pixel pairs at the bar ends, matching
  # its published end-pixel behaviour on interior bars).
  expected_widths <- list(
    zhang_suen = c(0, 1, 1, 1, 1, 1, 0, 0),
    guo_hall   = c(0, 1, 1, 1, 1, 1, 1, 0),
    lee        = c(0, 1, 1, 1, 1, 1, 1, 0),
    k3m        = c(0, 1, 1, 1, 1, 1, 1, 1),
    hilditch   = c(0, 1, 1, 1, 1, 1, 1, 0),
    opta       = c(2, 1, 1, 1, 1, 1, 1, 2),
    holt       = c(0, 1, 1, 1, 1, 1, 1, 0)
  )
  for (mth in methods) {
    local({
      m <- mth
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 8, ncol = 6)
        img[, 3:5] <- 1L
        sk <- thin(img, method = m)
        expect_identical(unname(rowSums(sk)), expected_widths[[m]])
        skeleton_cols <- sort(unique(which(sk == 1L, arr.ind = TRUE)[, "col"]))
        expect_identical(skeleton_cols, if (m == "opta") c(3L, 4L, 5L) else 4L)
        expect_identical(count_components(sk), 1L)
      })
    })
  }
})
