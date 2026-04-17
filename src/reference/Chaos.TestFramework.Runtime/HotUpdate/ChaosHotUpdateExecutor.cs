using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Loader;
using System.Threading;
using Chaos.TestFramework;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Executes compact hotupdate proof and benchmark entries through reflection.
/// </summary>
public static class ChaosHotUpdateExecutor
{
    private static readonly bool BaseDirectoryResolverRegistered = RegisterBaseDirectoryResolver();
    private static readonly AsyncLocal<string?> PreferredAssemblyBaseDirectory = new();

    /// <summary>
    /// Executes a declared proof entry.
    /// </summary>
    public static void ExecuteProof(string collectionPath, string bindingManifestPath, int entryIndex)
    {
        _ = ExecuteProofWorkload(collectionPath, bindingManifestPath, entryIndex);
    }

    /// <summary>
    /// Executes a declared benchmark entry.
    /// </summary>
    public static void ExecuteBenchmark(string collectionPath, string bindingManifestPath, int entryIndex)
    {
        _ = ExecuteBenchmarkWorkload(collectionPath, bindingManifestPath, entryIndex);
    }

    /// <summary>
    /// Executes a declared proof entry and returns the reflected result.
    /// </summary>
    public static object? ExecuteProofWorkload(
        string collectionPath,
        string bindingManifestPath,
        int entryIndex,
        string? assemblyBaseDirectory = null)
    {
        var entry = ResolveEntry(collectionPath, ChaosManagedHostKind.Proof, entryIndex);
        return InvokeEntry(bindingManifestPath, entry, assemblyBaseDirectory);
    }

    /// <summary>
    /// Executes a declared benchmark entry and returns the reflected result.
    /// </summary>
    public static object? ExecuteBenchmarkWorkload(
        string collectionPath,
        string bindingManifestPath,
        int entryIndex,
        string? assemblyBaseDirectory = null)
    {
        var entry = ResolveEntry(collectionPath, ChaosManagedHostKind.Benchmark, entryIndex);
        return InvokeEntry(bindingManifestPath, entry, assemblyBaseDirectory);
    }

    private static ChaosTestCollectionEntry ResolveEntry(string collectionPath, ChaosManagedHostKind hostKind, int entryIndex)
    {
        ChaosTestCollectionLoader.EnsureEntryExists(collectionPath, hostKind, entryIndex);
        var collection = ChaosTestCollectionLoader.Load(collectionPath);
        var entries = hostKind == ChaosManagedHostKind.Proof
            ? collection.DeclaredUnitTests
            : collection.DeclaredBenchmarks;
        var entry = entries.FirstOrDefault(candidate => candidate.EntryIndex == entryIndex);
        if (entry is null)
        {
            throw new InvalidOperationException($"entry index '{entryIndex}' is missing from collection '{collectionPath}'.");
        }

        if (string.IsNullOrWhiteSpace(entry.AssemblyName)
            || string.IsNullOrWhiteSpace(entry.DeclaringType)
            || string.IsNullOrWhiteSpace(entry.MethodName))
        {
            throw new InvalidOperationException($"entry index '{entryIndex}' does not provide reflection metadata.");
        }

        return entry;
    }

    private static object? InvokeEntry(
        string bindingManifestPath,
        ChaosTestCollectionEntry entry,
        string? assemblyBaseDirectory = null)
    {
        _ = BaseDirectoryResolverRegistered;
        var previousBaseDirectory = PreferredAssemblyBaseDirectory.Value;
        PreferredAssemblyBaseDirectory.Value = assemblyBaseDirectory;
        try
        {
            var bindingManifest = ChaosHotUpdateBindingManifestLoader.Load(bindingManifestPath);
            foreach (var patchAssemblyName in bindingManifest.PatchAssemblyNames)
            {
                if (string.IsNullOrWhiteSpace(patchAssemblyName))
                {
                    continue;
                }

                LoadAssemblyByName(patchAssemblyName, assemblyBaseDirectory);
            }

            var assembly = LoadAssemblyByName(entry.AssemblyName, assemblyBaseDirectory);
            var declaringType = assembly.GetType(entry.DeclaringType, throwOnError: true, ignoreCase: false)
                ?? throw new TypeLoadException($"unable to load declaring type '{entry.DeclaringType}'.");
            var method = declaringType.GetMethod(
                entry.MethodName,
                BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static)
                ?? throw new MissingMethodException(entry.DeclaringType, entry.MethodName);

            try
            {
                return method.Invoke(null, null);
            }
            catch (TargetInvocationException exception) when (exception.InnerException is ChaosAssertionException assertion)
            {
                throw assertion;
            }
            catch (TargetInvocationException exception) when (exception.InnerException is not null)
            {
                throw exception.InnerException!;
            }
        }
        finally
        {
            PreferredAssemblyBaseDirectory.Value = previousBaseDirectory;
        }
    }

    private static Assembly LoadAssemblyByName(string assemblyName, string? assemblyBaseDirectory = null)
    {
        var loadedAssembly = FindLoadedAssembly(assemblyName);
        if (loadedAssembly is not null)
        {
            return loadedAssembly;
        }

        var candidatePath = ResolveBaseDirectoryAssemblyPath(assemblyName, assemblyBaseDirectory);
        if (candidatePath is not null)
        {
            return AssemblyLoadContext.Default.LoadFromAssemblyPath(candidatePath);
        }

        return Assembly.Load(new AssemblyName(assemblyName));
    }

    private static bool RegisterBaseDirectoryResolver()
    {
        AssemblyLoadContext.Default.Resolving += ResolveFromBaseDirectory;
        return true;
    }

    private static Assembly? ResolveFromBaseDirectory(AssemblyLoadContext context, AssemblyName assemblyName)
    {
        var simpleName = assemblyName.Name;
        if (string.IsNullOrWhiteSpace(simpleName))
        {
            return null;
        }

        var loadedAssembly = FindLoadedAssembly(simpleName);
        if (loadedAssembly is not null)
        {
            return loadedAssembly;
        }

        var preferredBaseDirectory = PreferredAssemblyBaseDirectory.Value;
        if (!string.IsNullOrWhiteSpace(preferredBaseDirectory))
        {
            var preferredCandidatePath = ResolveBaseDirectoryAssemblyPath(simpleName, preferredBaseDirectory);
            if (preferredCandidatePath is not null)
            {
                return context.LoadFromAssemblyPath(preferredCandidatePath);
            }
        }

        var candidatePath = ResolveBaseDirectoryAssemblyPath(simpleName);
        if (candidatePath is null)
        {
            return null;
        }

        return context.LoadFromAssemblyPath(candidatePath);
    }

    private static Assembly? FindLoadedAssembly(string assemblyName)
    {
        return AppDomain.CurrentDomain
            .GetAssemblies()
            .FirstOrDefault(candidate => string.Equals(candidate.GetName().Name, assemblyName, StringComparison.Ordinal));
    }

    private static string? ResolveBaseDirectoryAssemblyPath(string assemblyName, string? baseDirectory = null)
    {
        var probeRoot = string.IsNullOrWhiteSpace(baseDirectory)
            ? AppContext.BaseDirectory
            : baseDirectory;
        var candidatePath = Path.Combine(probeRoot, $"{assemblyName}.dll");
        return File.Exists(candidatePath) ? candidatePath : null;
    }
}
