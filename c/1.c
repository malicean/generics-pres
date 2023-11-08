#include <stdio.h>

//////////////
// RATIONAL //
//////////////

typedef struct {
  int num;
  int denom;
} rational_t;

rational_t add_rational(rational_t lhs, rational_t rhs) {
  rational_t sum;
  
  //  a     c     da + bc
  // --- * --- = ---------
  //  b     d       bd
  sum.num = (rhs.denom * lhs.num) + (lhs.denom * rhs.num);
  sum.denom = lhs.denom * rhs.denom;

  return sum;
}

rational_t div_rational(rational_t lhs, rational_t rhs) {
  rational_t quo;
  
  //  a     c     a     d     ad
  // --- / --- = --- * --- = ----
  //  b     d     b     c     bc
  quo.num = lhs.num * rhs.denom;
  quo.denom = lhs.denom * rhs.num;

  return quo;
}

rational_t compute_rational(rational_t a_0, int n) {
  if (n == 0) {
    return a_0;
  }
  else {
    rational_t a_prev = compute_rational(a_0, n - 1);

    rational_t q_4 = { .num = 4, .denom = 1 };
    rational_t q_7 = { .num = 7, .denom = 1 };
    
    return add_rational(div_rational(a_prev, q_4), q_7);
  }
}

/////////
// INT //
/////////

int compute_int(int a_0, int n) {
  if (n == 0) {
    return a_0;
  }
  else {
    int a_prev = compute_int(a_0, n - 1);

    return a_prev / 4 + 7;
  }
}

//////////
// MAIN //
//////////

float approx(rational_t q) {
  return (float) q.num / (float) q.denom;
}

int main() {
  {
    int ret;
    
    for (int i = 0; i < 10000000; i++) { // 10 million
      ret = compute_int(343, 7); // = 9
    }

    printf("%d\n", ret);
  }

  {
    rational_t ret;
    rational_t q_343 = { .num = 343, .denom = 1 };
    
    for (int i = 0; i < 10000000; i++) { // 10 million
      ret = compute_rational(q_343, 7);
    }

    printf("%d / %d ≈ %f\n", ret.num, ret.denom, approx(ret));
  }

  return 0;
}
