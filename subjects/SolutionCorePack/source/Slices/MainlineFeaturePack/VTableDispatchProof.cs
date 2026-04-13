using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal abstract class Animal
{
    protected readonly string Name;

    protected Animal(string name)
    {
        Name = name;
    }

    public abstract string Speak();
}

internal sealed class Dog : Animal
{
    public Dog(string name)
        : base(name)
    {
    }

    public override string Speak()
    {
        return "dog:" + Name;
    }
}

internal sealed class Cat : Animal
{
    public Cat(string name)
        : base(name)
    {
    }

    public override string Speak()
    {
        return "cat:" + Name;
    }
}

internal static class VTableDispatchProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "vtable-dispatch-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 3)]
    public static int Run()
    {
        Animal dog = new Dog("Rex");
        Animal cat = new Cat("Whiskers");

        Console.WriteLine("vtable-dog=" + dog.Speak());
        Console.WriteLine("vtable-cat=" + cat.Speak());

        Animal[] animals = [new Dog("Buddy"), new Cat("Luna")];
        foreach (Animal animal in animals)
        {
            Console.WriteLine("vtable-poly=" + animal.Speak());
        }

        return 0;
    }
}
