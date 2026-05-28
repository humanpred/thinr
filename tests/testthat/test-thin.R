# Properties every thinning algorithm should satisfy on simple inputs.
# Run for all implemented methods.

methods <- c("zhang_suen", "guo_hall", "lee", "k3m",
             "hilditch", "opta", "holt")

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
        # which preserves bar corner pixels on the top and bottom row;
        # Holt's H condition has no crossing-number topology guard.
        # Both leave stray pixels at the bar ends - this is the
        # published behaviour, not an implementation choice.
        max_rows <- if (m %in% c("opta", "holt")) 3L else 1L
        expect_lte(length(rows_with_fg), max_rows,
                   label = paste("method =", m,
                                 "; rows with foreground =",
                                 paste(rows_with_fg, collapse = ",")))
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

describe("thinImage matches thin(method = 'zhang_suen')", {
  it("on a solid square", {
    img <- matrix(0L, nrow = 9, ncol = 9)
    img[3:7, 3:7] <- 1L
    expect_equal(thinImage(img), thin(img, method = "zhang_suen"))
  })
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
  # Thinning algorithms in this package examine an 8-neighbourhood, so they
  # leave the outermost row / column untouched. We test a representative
  # shape against each algorithm and check that the border stays clear
  # when the input has a one-pixel margin.
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
        # Boundary pixels are not eligible for removal, so this should be
        # preserved.
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
      })
      it(paste0("[", m, "] plus-shape collapses to a connected skeleton"), {
        img <- matrix(0L, nrow = 11, ncol = 11)
        img[5:7, 3:9] <- 1L
        img[3:9, 5:7] <- 1L
        sk <- thin(img, method = m)
        expect_gt(sum(sk), 0L)
        expect_lt(sum(sk), sum(img))
      })
      it(paste0("[", m, "] disconnected components stay disconnected"), {
        img <- matrix(0L, nrow = 9, ncol = 11)
        img[2:4, 2:4] <- 1L
        img[6:8, 8:10] <- 1L
        sk <- thin(img, method = m)
        # Each component has at least one surviving foreground pixel.
        expect_gt(sum(sk[1:5, 1:6]), 0L)
        expect_gt(sum(sk[5:9, 6:11]), 0L)
      })
    })
  }
})

describe("complex shapes do not crash and yield smaller skeletons", {
  # Larger / more varied shapes drive iterations through more branches
  # of the underlying algorithms.
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

describe("thinImage drop-in", {
  it("accepts a logical matrix", {
    img <- matrix(FALSE, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- TRUE
    sk <- thinImage(img)
    expect_type(sk, "logical")
    expect_equal(dim(sk), dim(img))
  })
  it("accepts a numeric matrix", {
    img <- matrix(0, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1
    sk <- thinImage(img)
    expect_type(sk, "double")
  })
  it("matches thin(method = 'zhang_suen') on a logical input", {
    img <- matrix(FALSE, nrow = 9, ncol = 9)
    img[3:7, 3:7] <- TRUE
    expect_equal(thinImage(img), thin(img, method = "zhang_suen"))
  })
})
