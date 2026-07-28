// Shared inline helpers used by the thinning algorithms in thinr.
//
// All algorithms use the same 8-neighbour labelling, p2 = north,
// going clockwise:
//
//   p9 p2 p3
//   p8 P1 p4
//   p7 p6 p5

#ifndef THINR_COMMON_H
#define THINR_COMMON_H

namespace thinr {

// Crossing number A(P): number of 0->1 transitions in the cyclic
// neighbour sequence p2, p3, ..., p9, p2. Equals 1 for "simple"
// pixels (deletable in 2D without changing topology).
inline int crossing_number(int p2, int p3, int p4, int p5,
                           int p6, int p7, int p8, int p9) {
  return (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1)
       + (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1)
       + (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1)
       + (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
}

// Neighbour count B(P): total foreground 8-neighbours.
inline int neighbour_count(int p2, int p3, int p4, int p5,
                           int p6, int p7, int p8, int p9) {
  return p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
}

// 4-connected background test: TRUE iff at least one 4-connected
// neighbour is background. Identifies border pixels.
//
// NO CALLER TODAY -- retained deliberately, not orphaned. Every kernel
// currently in src/ tests deletability through crossing_number() /
// neighbour_count() instead; this predicate was last used by the
// `stentiford` and `pavlidis` kernels, dropped 2026-05-20. It is kept
// because it is part of this header's reason to exist -- the shared
// 8-neighbour vocabulary a NEW thinning kernel is written against --
// and an uncalled `inline` emits no code, so it costs nothing at
// runtime or in binary size.
//
// The 2026-07-28 review left "delete it or state that it is deliberate"
// open as a judgement call; this comment is that decision. If a future
// reader finds this helper still callerless and the header's shared-
// vocabulary role no longer holds, deleting it is safe and expected.
inline bool is_border_4(int p2, int p4, int p6, int p8) {
  return (p2 == 0) || (p4 == 0) || (p6 == 0) || (p8 == 0);
}

}  // namespace thinr

#endif  // THINR_COMMON_H
