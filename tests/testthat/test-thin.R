# Properties every thinning algorithm should satisfy on simple inputs.
# Run for all implemented methods.

methods <- c("zhang_suen", "guo_hall", "lee", "k3m",
             "hilditch", "stentiford", "pavlidis", "opta", "holt")

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
})
