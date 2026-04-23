using System.Reflection;
using Chaos.TestFramework;
using GoldenHotUpdate.SharedContracts;

namespace GoldenHotUpdateHost.App;

internal static class Program
{
    public static int Main()
    {
        var assemblyPath = Path.Combine(AppContext.BaseDirectory, "GoldenHotUpdate.PatchModule.dll");
        Assert.True(File.Exists(assemblyPath), $"missing patch assembly: {assemblyPath}");

        var patchAssembly = Assembly.LoadFrom(assemblyPath);
        var patchType = patchAssembly.GetType("GoldenHotUpdate.PatchModule.PatchFeature", throwOnError: true);
        Assert.NotNull(patchType);

        var patchInstance = Activator.CreateInstance(patchType!);
        Assert.NotNull(patchInstance);

        var patch = patchInstance as IPatchFeature;
        Assert.NotNull(patch);

        var hostMath = new HostMath();
        Assert.Equal(42, patch!.GetPatchedValue());
        Assert.Equal("host-chaos-7", patch.ComposeMessage(hostMath, "chaos"));
        Assert.Equal("payload", patch.Echo("payload"));
        Assert.Equal(12, patch.Apply(static value => value + 5, 7));

        var fault = Assert.Throws<InvalidOperationException>(() => patch.ThrowPatchFault());
        Assert.Equal("patch-fault", fault.Message);
        return 0;
    }

    private sealed class HostMath : IHostMath
    {
        public int Add(int left, int right)
        {
            return left + right;
        }

        public string Decorate(string value)
        {
            return $"host-{value}";
        }
    }
}
