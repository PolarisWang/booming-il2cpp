using System;

namespace VTableDispatchProof;

/// Base class with virtual method — tests vtable dispatch through base reference.
internal abstract class Animal
{
    protected readonly string Name;

    protected Animal(string name)
    {
        Name = name;
    }

    /// Virtual method that derived classes override.
    public abstract string Speak();
}

internal sealed class Dog : Animal
{
    public Dog(string name) : base(name) { }

    public override string Speak()
    {
        return "dog:" + Name;
    }
}

internal sealed class Cat : Animal
{
    public Cat(string name) : base(name) { }

    public override string Speak()
    {
        return "cat:" + Name;
    }
}

internal static class VTableDispatchProofEntry
{
    /// Entry point. Each Console.WriteLine output is used as a proof assertion.
    public static int Run()
    {
        // Call Speak() through base class reference — triggers vtable dispatch.
        Animal dog = new Dog("Rex");
        Animal cat = new Cat("Whiskers");

        Console.WriteLine("vtable-dog=" + dog.Speak());
        Console.WriteLine("vtable-cat=" + cat.Speak());

        // Verify polymorphism: same declared type, different runtime types.
        Animal[] animals = { new Dog("Buddy"), new Cat("Luna") };
        foreach (var animal in animals)
        {
            Console.WriteLine("vtable-poly=" + animal.Speak());
        }

        return 0;
    }
}

internal static class Program
{
    public static int Main() => VTableDispatchProofEntry.Run();
}
