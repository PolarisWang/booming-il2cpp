using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Driver.Tests;

public sealed class FullAssemblyEmitterTests
{
    [Fact]
    public void Emit_NullClosureResult_ThrowsArgumentNullException()
    {
        var emitter = new FullAssemblyEmitter();
        Assert.Throws<ArgumentNullException>(() => emitter.Emit(null!, "output", CodegenMode.Aot));
    }
}
