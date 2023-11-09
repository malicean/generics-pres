---
author: Alice
date: ""
paging: "%d / %d"
theme: theme.json
---

```
  ____                      _          
 / ___| ___ _ __   ___ _ __(_) ___ ___ 
| |  _ / _ \ '_ \ / _ \ '__| |/ __/ __|
| |_| |  __/ | | |  __/ |  | | (__\__ \
 \____|\___|_| |_|\___|_|  |_|\___|___/
```

What do they solve, and why does Rust (ab)use them?

We will...

- Introduce a problem in C
- Introduce a variant of the problem
- Generalize over the two
- See how an object-oriented language solves it
- See how Rust solves it

---

# A Problem to Work With

```c
int compute(int a_0, int n) {
  if (n == 0) {
    return a_0;
  }
  else {
    int a_prev = compute(a_0, n - 1);

    return a_prev / 4 + 7;
  }
}

int main() {
  compute(343, 7);
  return 0;
}
```

---

```
f(0) := 343
f(n) := f(n-1) / 4 + 7

f(1) = 343 / 4 + 7 = 92
f(2) = 30
f(3) = 14
f(4) = 10
f(5) = 9
f(6) = 9
f(7) = 9
```

---

# What About Precision?

```
f(0) := 343
f(n) := f(n-1) / 4 + 7
```

## With integer (floor) division:

```
f(1) = 343 / 4 + 7 = 92
```

## With rational division:

```
f(1) = 343 / 4 + 7 = 343 / 4 + 28 / 4
                   = 371 / 4
                   = 92.75
f(2) = 30.1875
f(3) = 14.546875
f(4) = 10.63671875
f(5) = 9.6591796875
f(6) = 9.414794921875
f(7) = 9.35369873046875
```

---

```c
typedef struct {
  int num;
  int denom;
} rational_t;

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
```

---

# Code Duplication

```c
...
    int a_prev = compute_int(a_0, n - 1);

    int z_4 = 4;
    int z_7 = 7;
    
    return add_int(div_int(a_prev, z_4), z_7);
...

...
    rational_t a_prev = compute_rational(a_0, n - 1);

    rational_t q_4 = { .num = 4, .denom = 1 };
    rational_t q_7 = { .num = 7, .denom = 1 };
    
    return add_rational(div_rational(a_prev, q_4), q_7);
...
```

---

```
f(a_0, n):
  if n = 0:
    return a_0
  else:
    a_prev := f(a_0, n-1)

    four  := <depends on type>
    seven := <depends on type>
    add   := <depends on type>
    div   := <depends on type>
    
    return add(div(a_prev, four), seven)
```

---

# Passing Around Type

```c
typedef struct {
  <int | rational_t> four;
  <int | rational_t> seven;
  <int | rational_t> add(<int | rational_t> lhs, <int | rational_t> rhs);
  <int | rational_t> div(<int | rational_t> lhs, <int | rational_t> rhs);
} typeinfo_t;

<int | rational_t> compute(typeinfo_t t, <int | rational_t> a_0, int n) {
  if (n == 0) {
    return a_0;
  }
  else {
    <int | rational_t> a_prev = compute(a_0, n - 1);

    return t.add(t.div(a_prev, t.four), t.seven);
  }
}
```

---

## Problems:

- Not valid C, at all
- `sizeof(int) != sizeof(rational_t)`

---

# Type Elision via `void*`

```c
typedef struct {
  void *four;
  void *seven;
  void *(*add)(void *lhs, void *rhs);
  void *(*div)(void *lhs, void *rhs);
} typeinfo_t;

void *compute(typeinfo_t t, void *a_0, int n) {
  if (n == 0) {
    return a_0;
  }
  else {
    void *a_prev = compute(a_0, n - 1);

    return t.add(t.div(a_prev, t.four), t.seven);
  }
}
```

---

## Compared to last:

- Now valid C
- Any-sized type can be stored

## Problems:

- Pointer indirection
- `add`/`div` returns new memory => memory allocation

---

# Stack Allocation!

```c
typedef struct {
  size_t size;
  void *four;
  void *seven;
  void (*add)(void *dst, void *lhs, void *rhs);
  void (*div)(void *dst, void *lhs, void *rhs);
} typeinfo_t;

void compute(typeinfo_t t, void *dst, void *a_0, int n) {
  if (n == 0) {
    memcpy(dst, a_0, t.size);
  }
  else {
    compute(t, dst, a_0, n - 1); // *dst = a_prev
    t.div(dst, dst, t.four);     // *dst = *dst / 4
    t.add(dst, dst, t.seven);    // *dst = *dst + 7
  }
}
```

---

## Compared to last:

- `add`/`div` writes to existing memory, rather than allocating it

## Problems:

- Pointer indirection (less though)
- Clunky

---

# Using Language Features

```c
typedef struct {
  size_t size;
  void *four;
  void *seven;
  void (*add)(void *dst, void *lhs, void *rhs);
  void (*div)(void *dst, void *lhs, void *rhs);
} typeinfo_t;
```

...becomes...

```csharp
interface Number
{
    Number Four();
    Number Seven();
    Number Add(Number rhs);
    Number Div(Number rhs);
}

class Integer : Number { ... }

class Rational : Number { ... }
```

---

```c
void compute(typeinfo_t t, void *dst, void *a_0, int n) {
  if (n == 0) {
    memcpy(dst, a_0, t.size);
  }
  else {
    compute(t, dst, a_0, n - 1); // *dst = a_prev
    t.div(dst, dst, t.four);     // *dst = *dst / 4
    t.add(dst, dst, t.seven);    // *dst = *dst + 7
  }
}
```

...becomes...

```csharp
Number Compute(Number a_0, int n)
{
    if (n == 0)
        return a_0;
    else
        return Compute(a_0, n - 1)
            .Div(a_0.Four())
            .Add(a_0.Seven());
}
```

---

## Compared to C:

- Not as clunky

## Problems

- Pointer indirection!
- `Add`/`Div` return new memory again, requiring allocation
- Few type guarantees; `new Integer(0).Four()` has no guarantee of being `Integer`

---

# Let's Introduce Generics!

```csharp
interface Number<T>
{
    T Four();
    T Seven();
    T Add(T rhs);
    T Div(T rhs);
}

struct Integer : Number<Integer> { ... }

struct Rational : Number<Rational> { ... }
```

---

```csharp
Number Compute(Number a_0, int n)
{
    if (n == 0)
        return a_0;
    else
        return Compute(a_0, n - 1)
            .Div(a_0.Four())
            .Add(a_0.Seven());
}
```

...becomes...

```csharp
T Compute<T>(T a_0, int n) where T : Number<T>
{
    if (n == 0)
        return a_0;
    else
        return Compute(a_0, n - 1)
            .Div(a_0.Four())
            .Add(a_0.Seven());
}
```

---

# Compared to last:

- No more memory allocations
- Type guarantees; `new Integer(0).Four()` must be an `Integer`
- *Monomorphization*!

---

# Onto Rust

```rust
trait Number {
    const FOUR: Self;

    const SEVEN: Self;

    fn add(self, rhs: Self) -> Self;

    fn div(self, rhs: Self) -> Self;
}

fn compute<T>(a_0: T, n: i32) -> T
where
    T: Number,
{
    if n == 0 {
        a_0
    } else {
        compute(a_0, n - 1).div(T::FOUR).add(T::SEVEN)
    }
}
```
---

# Compared to C#:

- Implicit `TSelf`
- Constants; we can get a `Number` before being given one
- Aggressive compiler

---

# The Standard Library

Most of this is already in the standard library!

```rust
pub trait Add<Rhs = Self> {
    type Output;

    fn add(self, rhs: Rhs) -> Self::Output;
}

pub trait Div<Rhs = Self> {
    type Output;

    fn div(self, rhs: Rhs) -> Self::Output;
}
```

`T: Number` is now `T: Add + Div`

```rust
trait Ids {
  const ZERO: Self;
  const ONE: Self;
}
```

`T: Number` is now `T: Add + Div + Ids`

---

# Compared to Last

- Atomized traits; some types might impl `Add` but not
  - `Div`: vectors, incl. matrices
  - `Ids`: non-zero integers, also vectors (`Ids` should be atomized)
- Finer granularity; `lhs` and `rhs` don't need the same type f.e. scalar and vector
- In `std`; a lot of types already implement these

---

# What If I Want Polymorphism?

Not as common of a problem in Rust; you can use generics to know the exact type.

Out of scope for this presentation, but if you are interested:

## Closest to Polymorphism

Dynamic dispatch via *trait objects*

Requires references and allocations

## Simplest Method

Static dispatch via *enums*

Requires big `match` statements
