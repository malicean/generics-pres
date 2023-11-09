
{
    Integer ret = default;
    Integer z343 = new(343);

    for (int i = 0; i < 10_000_000; i++)
    {
        ret = Compute(z343, 7); // = 9
    }

    Console.WriteLine(ret.Value);
}

{
    Rational ret = default;
    Rational q343 = new(343);

    for (int i = 0; i < 10_000_000; i++)
    {
        ret = Compute(q343, 7); // = 153251 / 16384 ≈ 9.353699
    }

    Console.WriteLine($"{ret.Numerator} / {ret.Denominator} ≈ {ret.Approx()}");
}

T Compute<T>(T a_0, int n) where T : Number<T>
{
    if (n == 0)
    {
        return a_0;
    }
    else
    {
        return Compute(a_0, n - 1)
            .Div(a_0.Four())
            .Add(a_0.Seven());
    }
}

interface Number<TSelf>
{
    TSelf Four();

    TSelf Seven();

    TSelf Add(TSelf rhs);

    TSelf Div(TSelf rhs);
}

struct Rational : Number<Rational>
{
    public int Numerator;
    public int Denominator;

    public Rational(int numerator, int denominator = 1)
    {
        Numerator = numerator;
        Denominator = denominator;
    }

    public Rational Four()
    {
        return new Rational(4);
    }

    public Rational Seven()
    {
        return new Rational(7);
    }

    public Rational Add(Rational rhs)
    {
        int num = (rhs.Denominator * Numerator) + (Denominator * rhs.Numerator);
        int denom = Denominator * rhs.Denominator;

        return new Rational(num, denom);
    }

    public Rational Div(Rational rhs)
    {
        int num = Numerator * rhs.Denominator;
        int denom = Denominator * rhs.Numerator;

        return new Rational(num, denom);
    }

    public float Approx()
    {
        return (float)Numerator / (float)Denominator;
    }
}

struct Integer : Number<Integer>
{
    public int Value;

    public Integer(int value)
    {
        Value = value;
    }

    public Integer Four()
    {
        return new Integer(4);
    }

    public Integer Seven()
    {
        return new Integer(7);
    }

    public Integer Add(Integer rhs)
    {
        return new Integer(Value + rhs.Value);
    }

    public Integer Div(Integer rhs)
    {
        return new Integer(Value / rhs.Value);
    }
}
