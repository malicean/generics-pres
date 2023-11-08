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
        let mut ret = Rational { num: 0, denom: 1 };
        let q343 = Rational { num: 343, denom: 1 };

        for _ in 0..10_000_000 {
            ret = compute(q343, 7); // = 153251 / 16384 ≈ 9.353699
        }

        println!("{} / {} = {}", ret.num, ret.denom, ret.approx());
    }
}

fn compute(a_0: &dyn Number, n: i32) -> Box<dyn Number> {
    if n == 0 {
        a_0
    } else {
        compute(a_0, n - 1).div(T::FOUR).add(T::SEVEN)
    }
}

trait Number {
    fn four(&self) -> &'static dyn Number;

    fn seven(&self) -> &'static dyn Number;

    fn add(&self, rhs: &dyn Number) -> Box<dyn Number>;

    fn div(&self, rhs: &dyn Number) -> Box<dyn Number>;
}

impl Number for i32 {
    const FOUR: Self = 4;

    const SEVEN: Self = 7;

    fn add(self, rhs: Self) -> Self {
        self + rhs
    }

    fn div(self, rhs: Self) -> Self {
        self / rhs
    }
}

#[derive(Copy, Clone)]
struct Rational {
    num: i32,
    denom: i32,
}

impl Number for Rational {
    const FOUR: Self = Rational { num: 4, denom: 1 };

    const SEVEN: Self = Rational { num: 7, denom: 1 };

    fn add(self, rhs: Self) -> Self {
        Rational {
            num: (rhs.denom * self.num) + (self.denom * rhs.num),
            denom: self.denom * rhs.denom,
        }
    }

    fn div(self, rhs: Self) -> Self {
        Rational {
            num: self.num * rhs.denom,
            denom: self.denom * rhs.num,
        }
    }
}

impl Rational {
    pub fn approx(self) -> f32 {
        self.num as f32 / self.denom as f32
    }
}
