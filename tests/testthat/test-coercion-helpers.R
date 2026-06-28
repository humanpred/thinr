# Strict exact-value tests for the internal coercion helpers
# (as_binary_matrix / restore_storage) and the array-input path of
# thin(). These pin the precise dispatch behaviour rather than just
# the output shape.

describe("as_binary_matrix: collapses foreground to exactly 1", {
  it("integer input keeps zeros and maps every non-zero to 1L", {
    img <- matrix(c(0L, 2L, 0L,
                    5L, 0L, 255L,
                    0L, 9L, 0L),
                  nrow = 3, byrow = TRUE)
    out <- thinr:::as_binary_matrix(img)
    expect_identical(
      out,
      matrix(c(0L, 1L, 0L,
               1L, 0L, 1L,
               0L, 1L, 0L),
             nrow = 3, byrow = TRUE)
    )
    expect_type(out, "integer")
  })

  it("logical input becomes a 0/1 integer matrix", {
    img <- matrix(c(TRUE, FALSE, FALSE, TRUE), nrow = 2)
    out <- thinr:::as_binary_matrix(img)
    expect_identical(out, matrix(c(1L, 0L, 0L, 1L), nrow = 2))
    expect_type(out, "integer")
  })

  it("numeric input maps any non-zero (incl. negatives/fractions) to 1", {
    img <- matrix(c(0, -3.5, 0.2, 0), nrow = 2)
    out <- thinr:::as_binary_matrix(img)
    expect_identical(out, matrix(c(0L, 1L, 1L, 0L), nrow = 2))
    expect_type(out, "integer")
  })

  it("preserves matrix dimensions", {
    img <- matrix(1L, nrow = 3, ncol = 7)
    out <- thinr:::as_binary_matrix(img)
    expect_identical(dim(out), c(3L, 7L))
  })
})

describe("as_binary_matrix: array input dispatches through the matrix path", {
  # A 2-D object built with array() is, in R (>= 4.2), already a matrix,
  # so it is handled identically to the equivalent matrix() input. This
  # pins that equivalence with exact values.
  it("a 2-D integer array yields the same result as the matrix form", {
    arr <- array(0L, dim = c(5L, 5L))
    arr[2:4, 2:4] <- 1L
    mat <- matrix(0L, nrow = 5, ncol = 5)
    mat[2:4, 2:4] <- 1L
    expect_identical(
      thinr:::as_binary_matrix(arr),
      thinr:::as_binary_matrix(mat)
    )
  })

  it("thin() accepts a 2-D array and matches the matrix result exactly", {
    arr <- array(0L, dim = c(5L, 5L))
    arr[2:4, 2:4] <- 1L
    mat <- matrix(0L, nrow = 5, ncol = 5)
    mat[2:4, 2:4] <- 1L
    expect_identical(
      thin(arr, method = "zhang_suen"),
      thin(mat, method = "zhang_suen")
    )
  })
})

describe("as_binary_matrix: unsupported inputs error with the documented message", {
  it("character matrix names the storage mode in the message", {
    expect_error(
      thinr:::as_binary_matrix(matrix("a", 2, 2)),
      "does not know how to interpret a matrix of mode 'character'",
      fixed = TRUE
    )
  })

  it("a 3-D array reports the 2-D requirement", {
    expect_error(
      thinr:::as_binary_matrix(array(0L, dim = c(2L, 2L, 2L))),
      "expects a 2-D matrix"
    )
  })

  it("a 1-D array reports the 2-D requirement", {
    expect_error(
      thinr:::as_binary_matrix(array(0L, dim = 4L)),
      "expects a 2-D matrix"
    )
  })
})

describe("restore_storage: returns the storage mode of the original input", {
  skel <- matrix(c(0L, 1L, 0L, 1L), nrow = 2)

  it("logical original -> logical skeleton with identical values", {
    out <- thinr:::restore_storage(skel, matrix(TRUE, 2, 2))
    expect_identical(out, matrix(c(FALSE, TRUE, FALSE, TRUE), nrow = 2))
    expect_type(out, "logical")
  })

  it("double original -> double skeleton with identical values", {
    out <- thinr:::restore_storage(skel, matrix(1.0, 2, 2))
    expect_identical(out, matrix(c(0, 1, 0, 1), nrow = 2))
    expect_type(out, "double")
  })

  it("integer original -> the integer skeleton is returned unchanged", {
    out <- thinr:::restore_storage(skel, matrix(1L, 2, 2))
    expect_identical(out, skel)
    expect_type(out, "integer")
  })

  it("preserves the skeleton dimensions for every storage mode", {
    sk <- matrix(0L, nrow = 4, ncol = 6)
    expect_identical(dim(thinr:::restore_storage(sk, matrix(TRUE, 4, 6))),
                     c(4L, 6L))
    expect_identical(dim(thinr:::restore_storage(sk, matrix(1.0, 4, 6))),
                     c(4L, 6L))
  })
})
