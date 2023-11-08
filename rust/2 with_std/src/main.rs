use std::ops::{Add, Div, Mul};

fn main() {
    {
        let mut ret = 0;
        let z343 = 343;

        for _ in 0..10_000_000 {
            ret = compute(z343, 7); // = 9
        }

        println!("{ret}");
    }

    {
        let mut ret = Rational {
            num: 0i32,
            denom: 1i32,
        };
        let q343 = Rational { num: 343, denom: 1 };

        for _ in 0..10_000_000 {
            ret = compute(q343, 7); // = 153251 / 16384 ≈ 9.353699
        }

        println!("{} / {} = {}", ret.num, ret.denom, ret.approx());
    }

    {
        let mut ret = Rational {
            num: 0u64,
            denom: 1u64,
        };
        let q343 = Rational {
            num: 3434_3434_3434_3434u64,
            denom: 1u64,
        };

        for _ in 0..10_000_000 {
            ret = compute(q343, 31); // = 153251 / 16384 ≈ 9.353699
        }

        println!("{} / {} = {}", ret.num, ret.denom, ret.approx());
    }
}

fn compute<T>(a_0: T, n: i32) -> T
where
    T: Ids + Add<Output = T> + Div<Output = T>,
{
    if n == 0 {
        a_0
    } else {
        let four = n_one::<T>(4);
        let seven = n_one::<T>(7);

        compute(a_0, n - 1).div(four).add(seven)
    }
}

fn n_one<T>(n: i32) -> T
where
    T: Ids + Add<Output = T>,
{
    let mut acc = T::ZERO;

    for _ in 0..n {
        acc = acc + T::ONE;
    }

    acc
}

trait Ids {
    const ZERO: Self;

    const ONE: Self;
}

impl Ids for i32 {
    const ZERO: Self = 0;

    const ONE: Self = 1;
}

impl Ids for u64 {
    const ZERO: Self = 0;

    const ONE: Self = 1;
}

#[derive(Copy, Clone)]
struct Rational<V> {
    num: V,
    denom: V,
}

impl<V> Ids for Rational<V>
where
    V: Ids,
{
    const ZERO: Self = Rational {
        num: V::ZERO,
        denom: V::ONE,
    };

    const ONE: Self = Rational {
        num: V::ONE,
        denom: V::ONE,
    };
}

impl<V> Add for Rational<V>
where
    V: Add<Output = V> + Mul<Output = V> + Copy,
{
    type Output = Rational<V>;

    fn add(self, rhs: Self) -> Self::Output {
        Rational {
            num: (rhs.denom * self.num) + (self.denom * rhs.num),
            denom: self.denom * rhs.denom,
        }
    }
}

impl<V> Div for Rational<V>
where
    V: Mul<Output = V>,
{
    type Output = Rational<V>;

    fn div(self, rhs: Self) -> Self::Output {
        Rational {
            num: self.num * rhs.denom,
            denom: self.denom * rhs.num,
        }
    }
}

impl Rational<i32> {
    pub fn approx(self) -> f32 {
        self.num as f32 / self.denom as f32
    }
}

impl Rational<u64> {
    pub fn approx(self) -> f32 {
        self.num as f32 / self.denom as f32
    }
}
