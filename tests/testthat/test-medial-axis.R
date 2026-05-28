# Medial axis transform: behavioural tests.

describe("medial_axis: background-only image yields an empty skeleton", {
  it("returns all zeros", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    expect_true(all(medial_axis(img) == 0))
  })
})

describe("medial_axis: a single foreground pixel is preserved", {
  it("returns the same single-pixel skeleton", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    img[3, 3] <- 1L
    expect_equal(medial_axis(img), img)
  })
})

describe("medial_axis: skeleton is a subset of the foreground", {
  it("for a 5x5 solid", {
    img <- matrix(0L, nrow = 9, ncol = 9)
    img[3:7, 3:7] <- 1L
    sk <- medial_axis(img)
    # Every foreground pixel in the skeleton must be foreground in the
    # original image.
    expect_true(all(sk[img == 0] == 0))
  })
})

describe("medial_axis: a horizontal bar has a horizontal skeleton", {
  it("the middle row is in the skeleton", {
    img <- matrix(0L, nrow = 5, ncol = 11)
    img[2:4, 2:10] <- 1L
    sk <- medial_axis(img)
    expect_true(all(sk[3, 3:9] == 1))
  })
})

describe("medial_axis: return_distance returns skeleton + distance", {
  it("with the right shape and types", {
    img <- matrix(0L, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1L
    result <- medial_axis(img, return_distance = TRUE)
    expect_named(result, c("skeleton", "distance"))
    expect_equal(dim(result$skeleton), dim(img))
    expect_equal(dim(result$distance), dim(img))
    expect_true(is.numeric(result$distance))
    # Distance is 0 on background pixels and >= 1 on foreground.
    expect_true(all(result$distance[img == 0] == 0))
    expect_true(all(result$distance[img == 1] >= 1))
  })
})

describe("medial_axis: storage mode of output skeleton matches input", {
  it("logical in, logical out", {
    img <- matrix(FALSE, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- TRUE
    sk <- medial_axis(img)
    expect_type(sk, "logical")
  })
  it("numeric in, numeric out", {
    img <- matrix(0, nrow = 5, ncol = 5)
    img[2:4, 2:4] <- 1
    sk <- medial_axis(img)
    expect_type(sk, "double")
  })
})
