namespace StubAssembly;

public class Animal
{
    public virtual string Speak() => "...";
}

public class Dog : Animal
{
    public override string Speak() => "Woof";
}

public class Cat : Animal
{
    public override string Speak() => "Meow";
}

public static class VirtualDispatchDemo
{
    // Exercises virtual call + polymorphic dispatch across the hierarchy
    public static string MakeSound(Animal animal) => animal.Speak();

    // Exercises constrained virtual call on value type implementing interface
    public static int StructInterfaceDispatch(int x, int y)
    {
        var adder = new Adder();
        return CallCompute(adder, x, y);
    }

    private static int CallCompute(ICalculator calc, int x, int y) =>
        calc.Compute(x, y);
}
