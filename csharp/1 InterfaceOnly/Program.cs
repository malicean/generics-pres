
{
    Integer ret = null!;
    Integer z343 = new(343);

    for (int i = 0; i < 10_000_000; i++)
    {
        ret = (Integer)Compute(z343, 7); // = 9
    }

    Console.WriteLine(ret.Value);
}

{
    Rational ret = null!;
    Rational q343 = new(343);

    for (int i = 0; i < 10_000_000; i++)
    {
        ret = (Rational)Compute(q343, 7); // = 153251 / 16384 ≈ 9.353699
    }

    Console.WriteLine($"{ret.Numerator} / {ret.Denominator} ≈ {ret.Approx()}");
}

Number Compute(Number a_0, int n)
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

interface Number
{
    Number Four();

    Number Seven();

    Number Add(Number rhs);

    Number Div(Number rhs);
}

class Rational : Number
{
    public int Numerator;
    public int Denominator;

    public Rational(int numerator, int denominator = 1)
    {
        Numerator = numerator;
        Denominator = denominator;
    }

    public Number Four()
    {
        return new Rational(4);
    }

    public Number Seven()
    {
        return new Rational(7);
    }

    public Number Add(Number rhs)
    {
        Rational rhsCast = (Rational)rhs;

        int num = (rhsCast.Denominator * Numerator) + (Denominator * rhsCast.Numerator);
        int denom = Denominator * rhsCast.Denominator;

        return new Rational(num, denom);
    }

    public Number Div(Number rhs)
    {
        Rational rhsCast = (Rational)rhs;

        int num = Numerator * rhsCast.Denominator;
        int denom = Denominator * rhsCast.Numerator;

        return new Rational(num, denom);
    }

    public float Approx()
    {
        return (float)Numerator / (float)Denominator;
    }
}

class Integer : Number
{
    public int Value;

    public Integer(int value)
    {
        Value = value;
    }

    public Number Four()
    {
        return new Integer(4);
    }

    public Number Seven()
    {
        return new Integer(7);
    }

    public Number Add(Number rhs)
    {
        Integer rhsCast = (Integer)rhs;

        return new Integer(Value + rhsCast.Value);
    }

    public Number Div(Number rhs)
    {
        Integer rhsCast = (Integer)rhs;

        return new Integer(Value / rhsCast.Value);
    }
}
