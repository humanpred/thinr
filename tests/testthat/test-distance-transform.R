# Distance transform: behavioural tests across all three metrics.

describe("distance_transform: background pixels have distance 0", {
  for (metric in c("euclidean", "manhattan", "chessboard")) {
    local({
      m <- metric
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 5, ncol = 5)
        img[3, 3] <- 1L
        d <- distance_transform(img, metric = m)
        # All background pixels are 0; the one foreground pixel has
        # the distance to the nearest background, which for an
        # interior single FG pixel is some positive value (it touches
        # background on the side, so distance is 1 in any metric).
        expect_equal(d[1, 1], 0)
        expect_equal(d[5, 5], 0)
        expect_true(d[3, 3] > 0)
      })
    })
  }
})

describe("distance_transform: manhattan distance from a corner", {
  it("returns L1 distance values", {
    img <- matrix(1L, nrow = 4, ncol = 4)
    img[1, 1] <- 0L
    d <- distance_transform(img, metric = "manhattan")
    expect_equal(d[1, 1], 0)
    expect_equal(d[1, 2], 1)
    expect_equal(d[2, 1], 1)
    expect_equal(d[2, 2], 2)
    expect_equal(d[1, 4], 3)
    expect_equal(d[4, 1], 3)
    expect_equal(d[4, 4], 6)
  })
})

describe("distance_transform: chessboard distance from a corner", {
  it("returns L_infinity distance values", {
    img <- matrix(1L, nrow = 4, ncol = 4)
    img[1, 1] <- 0L
    d <- distance_transform(img, metric = "chessboard")
    expect_equal(d[1, 1], 0)
    expect_equal(d[2, 2], 1)
    expect_equal(d[4, 4], 3)
    expect_equal(d[1, 4], 3)
  })
})

describe("distance_transform: euclidean distance", {
  it("agrees with brute-force on a small image", {
    img <- matrix(1L, nrow = 5, ncol = 5)
    img[1, 1] <- 0L
    d <- distance_transform(img, metric = "euclidean")
    # The only background pixel is at (1, 1); each foreground pixel's
    # L2 distance to it is sqrt((r-1)^2 + (c-1)^2).
    for (r in 1:5) for (c in 1:5) {
      expected <- sqrt((r - 1)^2 + (c - 1)^2)
      expect_equal(d[r, c], expected,
                   info = sprintf("(%d, %d)", r, c))
    }
  })
})

describe("distance_transform: exact full-matrix values from a corner", {
  # A 4x4 all-foreground image with a single background pixel at (1, 1).
  # Pin every cell of every metric, not just a handful.
  img <- matrix(1L, nrow = 4, ncol = 4)
  img[1, 1] <- 0L

  it("manhattan: every cell equals (r-1) + (c-1)", {
    expected <- outer(0:3, 0:3, `+`)
    expect_equal(distance_transform(img, metric = "manhattan"), expected)
  })

  it("chessboard: every cell equals max(r-1, c-1)", {
    expected <- outer(0:3, 0:3, pmax)
    expect_equal(distance_transform(img, metric = "chessboard"), expected)
  })

  it("euclidean: every cell equals sqrt((r-1)^2 + (c-1)^2)", {
    expected <- outer(0:3, 0:3, function(a, b) sqrt(a^2 + b^2))
    expect_equal(distance_transform(img, metric = "euclidean"), expected)
  })
})

describe("distance_transform: all-background image returns zeros", {
  for (metric in c("euclidean", "manhattan", "chessboard")) {
    local({
      m <- metric
      it(paste0("[", m, "]"), {
        img <- matrix(0L, nrow = 4, ncol = 4)
        d <- distance_transform(img, metric = m)
        expect_true(all(d == 0))
      })
    })
  }
})
