namespace StubAssembly;

public interface ICalculator
{
    int Compute(int x, int y);
}

public sealed class Adder : ICalculator
{
    public int Compute(int x, int y) => x + y;
}

public sealed class Multiplier : ICalculator
{
    public int Compute(int x, int y) => x * y;
}
