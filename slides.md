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

---

# A Problem to Work With

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

`f(1) = 343 / 4 + 7 = 92`

## With rational division:

```
f(1) = 343 / 4 + 7 = 343 / 4 + 7(4 / 4)
                   = 343 / 4 + 28 / 4
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

# Code Duplication

Expanding operators/literals

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
```

## Problems:

- Not valid C at all
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

void example(typeinfo_t t) {
  void *eleven = t.add(t.four, t.eleven);
}
```

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

void example(typeinfo_t t) {
  char eleven_alloc[t.size];
  void *eleven = (void *)eleven_alloc;
  t.add(dst, t.four, t.eleven);
}
```

## Compared to last:

- `add`/`div` writes to existing memory, rather than allocating it

## Problems:

- Pointer indirection (less though)
- Clunky

---

# Using Language Features

Seems a lot like an `interface`...

```csharp
interface Number
{
    Number Four();
    Number Seven();
    Number Add(Number rhs);
    Number Div(Number rhs);
}

Number Compute(Number a_0, int n)
{
    if (n == 0)
        return a_0;
    else
        return Compute(a_0, n - 1)
            .Div(a_0.Four())
            .Add(a_0.Seven());
}

class Integer : Number { ... }

class Rational : Number { ... }
```

## Compared to C:

- Not as clunky

## Problems

- Pointer indirection!
- `Add`/`Div` return new memory again, requiring allocation
- Few type guarantees; `new Integer(0).Four()` has no guarantee of being `Integer`

---

# Let's Introduce Generics!

```csharp
interface Number<TSelf>
where
    TSelf : Number<TSelf>
{
    TSelf Four();
    TSelf Seven();
    TSelf Add(TSelf rhs);
    TSelf Div(TSelf rhs);
}

T Compute<T>(T a_0, int n) where T : Number<T>
{
    if (n == 0)
        return a_0;
    else
        return Compute(a_0, n - 1)
            .Div(a_0.Four())
            .Add(a_0.Seven());
}

struct Integer : Number<Integer> { ... }

struct Rational : Number<Rational> { ... }
```

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

# Compared to C#:

- Implicit `TSelf`
- Constants; no need for `any`
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

Still need our own constants though, maybe an `Ids` trait

```rust
trait Ids {
  const ZERO: Self;
  const ONE: Self;
}
```

Total: `T: Add + Div + Ids`

---

# What If I Want Polymorphism?

Intefaces used dynamic dispatch. Monomorphization removed it.

Can use "trait objects"

```rust
fn main() {
  let use_rational = ...;
  let x: Box<dyn Number> = if use_rational {
    Box::new(Rational::new(7, 3))
  }
  else {
    Box::new(Integer::new(2))
  }
}

// struct defs here

impl Integer {
  fn new(n: i32) -> Self {
    ...
  }
}

impl Rational {
  fn new(n: i32, d: i32) -> Self {
    ...
  }
}
```

---

# What If I Want Polymorphism (But Fast)?

We know all possible "polymorphs": `Integer`, `Rational`

```rust
fn main() {
  let use_rational = ...;
  let x = if use_rational {
    PolyNumber::Rational(Rational::new(7, 3))
  }
  else {
    PolyNumber::Integer(Integer::new(2))
  }
}

enum PolyNumber {
  Integer(Integer),
  Rational(Rational)
}

impl Number for PolyNumber {
  ...
}
```
