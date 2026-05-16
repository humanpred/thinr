describe("zhang_suen", {
  it("produces a one-pixel-wide skeleton on a solid square", {
    m <- matrix(0L, nrow = 9, ncol = 9)
    m[3:7, 3:7] <- 1L
    sk <- thin(m, method = "zhang_suen")
    expect_lte(sum(sk), sum(m))
    # The skeleton should be much thinner than the original.
    expect_lt(sum(sk), sum(m) / 2)
  })

  it("preserves topology on a horizontal line", {
    m <- matrix(0L, nrow = 5, ncol = 11)
    m[2:4, 2:10] <- 1L
    sk <- thin(m, method = "zhang_suen")
    # All foreground pixels should be in one row after thinning.
    rows_with_fg <- which(rowSums(sk) > 0)
    expect_equal(length(rows_with_fg), 1L)
  })

  it("is idempotent (thinning a skeleton produces the same skeleton)", {
    m <- matrix(0L, nrow = 7, ncol = 11)
    m[3:5, 3:9] <- 1L
    once  <- thin(m,    method = "zhang_suen")
    twice <- thin(once, method = "zhang_suen")
    expect_equal(twice, once)
  })

  it("handles an all-background image without error", {
    m <- matrix(0L, nrow = 5, ncol = 5)
    expect_equal(thin(m, method = "zhang_suen"), m)
  })
})

describe("guo_hall", {
  it("produces a one-pixel-wide skeleton on a solid square", {
    m <- matrix(0L, nrow = 9, ncol = 9)
    m[3:7, 3:7] <- 1L
    sk <- thin(m, method = "guo_hall")
    expect_lte(sum(sk), sum(m))
    expect_lt(sum(sk), sum(m) / 2)
  })

  it("is idempotent", {
    m <- matrix(0L, nrow = 7, ncol = 11)
    m[3:5, 3:9] <- 1L
    once  <- thin(m,    method = "guo_hall")
    twice <- thin(once, method = "guo_hall")
    expect_equal(twice, once)
  })

  it("handles an all-background image without error", {
    m <- matrix(0L, nrow = 5, ncol = 5)
    expect_equal(thin(m, method = "guo_hall"), m)
  })
})

describe("lee and k3m stubs", {
  it("error informatively for lee", {
    m <- matrix(0L, nrow = 5, ncol = 5)
    m[2:4, 2:4] <- 1L
    expect_error(thin(m, method = "lee"), "not yet implemented in thinr v0.1")
  })

  it("error informatively for k3m", {
    m <- matrix(0L, nrow = 5, ncol = 5)
    m[2:4, 2:4] <- 1L
    expect_error(thin(m, method = "k3m"), "not yet implemented in thinr v0.1")
  })
})

describe("thinImage drop-in", {
  it("matches thin(method = 'zhang_suen')", {
    m <- matrix(0L, nrow = 9, ncol = 9)
    m[3:7, 3:7] <- 1L
    expect_equal(thinImage(m), thin(m, method = "zhang_suen"))
  })
})

describe("input coercion", {
  it("accepts a logical matrix and returns a logical matrix", {
    m <- matrix(FALSE, nrow = 5, ncol = 5)
    m[2:4, 2:4] <- TRUE
    sk <- thin(m, method = "zhang_suen")
    expect_type(sk, "logical")
    expect_equal(dim(sk), dim(m))
  })

  it("accepts a numeric matrix and returns a numeric matrix", {
    m <- matrix(0, nrow = 5, ncol = 5)
    m[2:4, 2:4] <- 1
    sk <- thin(m, method = "zhang_suen")
    expect_type(sk, "double")
    expect_equal(dim(sk), dim(m))
  })

  it("treats non-zero numeric values as foreground", {
    m <- matrix(0, nrow = 5, ncol = 5)
    m[2:4, 2:4] <- 0.7
    sk <- thin(m, method = "zhang_suen")
    expect_gt(sum(sk), 0)
  })
})
