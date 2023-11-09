#include <stddef.h>
#include <stdio.h>
#include <string.h>

//////////////
// TYPEINFO //
//////////////

typedef struct {
  size_t size;
  void *four;
  void *seven;
  void (*add)(void*, void*, void*); // void add(void *dst, void *lhs, void *rhs)
  void (*div)(void*, void*, void*); // void div(void *dst, void *lhs, void *rhs)
} typeinfo_t;

void compute(typeinfo_t t, void *dst, void *a_0, int n) {
  if (n == 0) {
    memcpy(dst, a_0, t.size);
  }
  else {
    compute(t, dst, a_0, n - 1); // a_prev
    t.div(dst, dst, t.four);     // / 4
    t.add(dst, dst, t.seven);    // + 7
  }
}

//////////////
// RATIONAL //
//////////////

typedef struct {
  int num;
  int denom;
} rational_t;

rational_t Q_4 = { .num = 4, .denom = 1 };
rational_t Q_7 = { .num = 7, .denom = 1 };

void add_rational(void *dst_p, void *lhs_p, void *rhs_p) {
  rational_t *dst = (rational_t *)dst_p;
  rational_t *lhs = (rational_t *)lhs_p;
  rational_t *rhs = (rational_t *)rhs_p;
  
  //  a     c     da + bc
  // --- + --- = ---------
  //  b     d       bd
  dst->num = (rhs->denom * lhs->num) + (lhs->denom * rhs->num);
  dst->denom = lhs->denom * rhs->denom;
}

void div_rational(void *dst_p, void *lhs_p, void *rhs_p) {
  rational_t *dst = (rational_t *)dst_p;
  rational_t *lhs = (rational_t *)lhs_p;
  rational_t *rhs = (rational_t *)rhs_p;
  
  //  a     c     a     d     ad
  // --- / --- = --- * --- = ----
  //  b     d     b     c     bc
  dst->num = lhs->num * rhs->denom;
  dst->denom = lhs->denom * rhs->num;
}

typeinfo_t type_rational = {
  .size = sizeof(rational_t),
  .four = &Q_4,
  .seven = &Q_7,
  .add = add_rational,
  .div = div_rational
};

/////////
// INT //
/////////

int Z_4 = 4;
int Z_7 = 7;

void add_int(void *dst_p, void *lhs_p, void *rhs_p) {
  int *dst = (int *)dst_p;
  int *lhs = (int *)lhs_p;
  int *rhs = (int *)rhs_p;
  
  *dst = *lhs + *rhs;
}

void div_int(void *dst_p, void *lhs_p, void *rhs_p) {
  int *dst = (int *)dst_p;
  int *lhs = (int *)lhs_p;
  int *rhs = (int *)rhs_p;
   
  *dst = *lhs / *rhs;
}

typeinfo_t type_int = {
  .size = sizeof(int),
  .four = &Z_4,
  .seven = &Z_7,
  .add = add_int,
  .div = div_int
};

//////////
// MAIN //
//////////

double approx(rational_t q) {
  return (double) q.num / (double) q.denom;
}

int main() {
  {
    int ret;
    int z_343 = 343;

    for (int i = 0; i < 10000000; i++) { // 10 million
      compute(type_int, &ret, &z_343, 7); // = 9
    }

    printf("%d\n", ret);
  }

  {
    rational_t ret;
    rational_t q_343 = { .num = 343, .denom = 1 };

    for (int i = 0; i < 10000000; i++) { // 10 million
      compute(type_rational, &ret, &q_343, 7); // = 153251 / 16384 ≈ 9.353699
    }

    printf("%d / %d ≈ %f\n", ret.num, ret.denom, approx(ret));  
  }

  return 0;
}
