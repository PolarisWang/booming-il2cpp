using System.Reflection;
using Chaos.TestFramework;

namespace SolutionCorePack;

internal static class Program
{
    private static int Main(string[] args)
    {
        if (!ChaosSubjectEntryArguments.TryParse(args, out var selection) || selection.IsNone)
        {
            return InvokeStaticEntry("CoreRuntimeFeatures", "CoreRuntimeFeatures.ProofEntry", "Run");
        }

        return selection switch
        {
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.CoreRuntimeFeaturesProof } =>
                InvokeStaticEntry("CoreRuntimeFeatures", "CoreRuntimeFeatures.ProofEntry", "Run"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.CoreRuntimeInterfaceDispatchProof } =>
                InvokeStaticEntry("CoreRuntimeFeatures", "CoreRuntimeFeatures.InterfaceDispatchProofEntry", "Run"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.SimpleLibrarySolutionManagedOutput } =>
                InvokeStaticEntry("GoldenSimpleLib.App", "GoldenSimpleLib.App.Program", "Main"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.MultiProjectSolutionManagedOutput } =>
                InvokeStaticEntry("GoldenMultiProject.App", "GoldenMultiProject.App.Program", "Main"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.PackageReferenceSolutionManagedOutput } =>
                InvokeStaticEntry("GoldenWithPackage.App", "GoldenWithPackage.App.Program", "Main"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.ReferenceAssemblySolutionManagedOutput } =>
                InvokeStaticEntry("GoldenReferenceAssembly.App", "GoldenReferenceAssembly.App.Program", "Main"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.CoreLibReferenceSolutionManagedOutput } =>
                InvokeStaticEntry("GoldenCoreLibReference.App", "GoldenCoreLibReference.App.Program", "Main"),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.MixedReferenceClosureSolutionManagedOutput } =>
                InvokeStaticEntry("GoldenMixedReference.App", "GoldenMixedReference.App.Program", "Main"),
            _ => throw new ArgumentOutOfRangeException(nameof(args), "unsupported SolutionCorePack entry selection."),
        };
    }

    private static int InvokeStaticEntry(string assemblyName, string typeName, string methodName)
    {
        var assemblyPath = Path.Combine(AppContext.BaseDirectory, $"{assemblyName}.dll");
        var assembly = File.Exists(assemblyPath)
            ? Assembly.LoadFrom(assemblyPath)
            : Assembly.Load(assemblyName);
        var type = ResolveType(assembly, typeName);
        var method = type.GetMethod(
            methodName,
            BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic)
            ?? throw new InvalidOperationException($"failed to load method '{typeName}::{methodName}'.");
        var parameters = method.GetParameters();
        object? result = parameters.Length switch
        {
            0 => method.Invoke(obj: null, parameters: null),
            1 when parameters[0].ParameterType == typeof(string[]) => method.Invoke(obj: null, parameters: new object?[] { Array.Empty<string>() }),
            _ => throw new InvalidOperationException($"unsupported method signature: '{typeName}::{methodName}'."),
        };

        return result is int exitCode ? exitCode : 0;
    }

    private static Type ResolveType(Assembly assembly, string typeName)
    {
        var direct = assembly.GetType(typeName, throwOnError: false, ignoreCase: false);
        if (direct is not null)
        {
            return direct;
        }

        foreach (var candidate in assembly.GetTypes())
        {
            if (string.Equals(candidate.Name, typeName, StringComparison.Ordinal))
            {
                return candidate;
            }
        }

        throw new InvalidOperationException($"failed to load type '{typeName}'.");
    }
}
