using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

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
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericVirtualDispatch,
        Priority = 3)]
    public static int Run()
    {
        Animal dog = new Dog("Rex");
        Animal cat = new Cat("Whiskers");
        Assert.Equal("dog:Rex", dog.Speak());
        Assert.Equal("cat:Whiskers", cat.Speak());


        Animal[] animals = [new Dog("Buddy"), new Cat("Luna")];
        Assert.Equal("dog:Buddy", animals[0].Speak());
        Assert.Equal("cat:Luna", animals[1].Speak());
        foreach (Animal animal in animals)
        {
        }

        return 0;
    }
}
