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
}
