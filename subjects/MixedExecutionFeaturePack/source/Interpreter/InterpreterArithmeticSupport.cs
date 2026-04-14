using Chaos.IL2CPP.Interpreter;
using Chaos.IL2CPP.Loader;

namespace MixedExecutionFeaturePack;

internal static class InterpreterArithmeticSupport
{
    private const string AssemblyFileName = "InterpreterArithmeticProof.dll";

    public static string ResolveAssemblyPath()
    {
        var assemblyPath = Path.Combine(AppContext.BaseDirectory, AssemblyFileName);
        if (!File.Exists(assemblyPath))
        {
            throw new FileNotFoundException($"missing support assembly: {assemblyPath}", assemblyPath);
        }

        return assemblyPath;
    }

    public static IReadOnlyDictionary<string, IRMethod> BuildLoweredMethods()
    {
        var assemblyPath = ResolveAssemblyPath();
        var loader = new LoaderStage();
        var assembly = loader.Load(new Chaos.IL2CPP.Contracts.ManagedClosureRequest(
            assemblyPath,
            Path.Combine(Path.GetTempPath(), "chaos-il2cpp", "mixed-execution-capability-closure")));
        var lowering = new ILToIRLowering();
        return assembly.Methods.ToDictionary(method => method.SubjectId, method => lowering.Lower(method), StringComparer.Ordinal);
    }

    public static IRMethod GetRequiredMethod(IReadOnlyDictionary<string, IRMethod> methods, string methodName)
    {
        ArgumentNullException.ThrowIfNull(methods);
        ArgumentException.ThrowIfNullOrWhiteSpace(methodName);

        return methods.Values.Single(method =>
            string.Equals(method.SubjectId.Split("::", StringSplitOptions.None)[1].Split('(')[0], methodName, StringComparison.Ordinal));
    }

    public static ManagedInterpreterExecutor CreateExecutor(IReadOnlyDictionary<string, IRMethod> methods)
    {
        ArgumentNullException.ThrowIfNull(methods);

        return new ManagedInterpreterExecutor((bridgeId, bridgeArguments) =>
        {
            if (string.Equals(bridgeId, "System.Private.CoreLib/System.String::get_Length()", StringComparison.Ordinal))
            {
                if (bridgeArguments.Count != 1 || bridgeArguments[0] is not string instance)
                {
                    throw new InvalidOperationException($"bridge '{bridgeId}' expects 1 string receiver.");
                }

                return instance.Length;
            }

            throw new InvalidOperationException($"unsupported bridge '{bridgeId}'.");
        },
        subjectKey => methods[subjectKey]);
    }
}
