using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.HotUpdate;

public sealed record HotUpdateConstantInt32Binding
{
    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public required int ConstantValue { get; init; }
}

public sealed record HotUpdateInt32UnaryBinding
{
    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public required Func<int, int> Target { get; init; }
}

public sealed record HotUpdateGenericMethodBinding
{
    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public required Func<IReadOnlyList<object?>, object?> Target { get; init; }
}

public sealed record HotUpdateMethodBindingSet
{
    public IReadOnlyList<HotUpdateConstantInt32Binding> ConstantInt32Bindings { get; init; } = [];

    public IReadOnlyList<HotUpdateInt32UnaryBinding> Int32UnaryBindings { get; init; } = [];

    public IReadOnlyList<HotUpdateGenericMethodBinding> GenericBindings { get; init; } = [];
}
