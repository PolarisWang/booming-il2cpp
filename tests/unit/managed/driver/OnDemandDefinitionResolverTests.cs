using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Driver.Tests;

/// <summary>
/// Tests for on-demand generic-definition resolution (L2 B1).
///
/// The property under test is not "does it find one method" but "does it resolve
/// the ACTUAL definition keys the pipeline produces, without whole-assembly
/// modelling".  The fixture is therefore the real demand graph emitted for a
/// chunk, not a hand-written SubjectId — a hand-written key would be free to
/// disagree with what <c>ManagedNaming</c> actually emits.
/// </summary>
public class OnDemandDefinitionResolverTests
{
    private static string RepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir is not null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, ".git")) ||
                File.Exists(Path.Combine(dir.FullName, ".git")))
            {
                return dir.FullName;
            }
            dir = dir.Parent;
        }
        throw new InvalidOperationException("repo root not found");
    }

    /// <summary>Definition keys for a chunk, grouped by owning assembly.</summary>
    private static Dictionary<string, List<string>> DemandDefinitionsByAssembly(string chunkSlug)
    {
        var root = RepoRoot();
        var graph = Path.Combine(root, "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", chunkSlug, "native", "codegen", "generated",
            "generic-instantiation-demand-graph.json");
        if (!File.Exists(graph))
        {
            return new Dictionary<string, List<string>>();
        }

        using var doc = System.Text.Json.JsonDocument.Parse(File.ReadAllText(graph));
        var byAssembly = new Dictionary<string, List<string>>(StringComparer.Ordinal);
        if (!doc.RootElement.TryGetProperty("demands", out var demands))
        {
            return byAssembly;
        }

        foreach (var demand in demands.EnumerateArray())
        {
            var owning = demand.TryGetProperty("owningAssemblyName", out var o) ? o.GetString() : null;
            var definition = demand.TryGetProperty("definitionSubjectId", out var d) ? d.GetString() : null;
            if (string.IsNullOrEmpty(owning) || string.IsNullOrEmpty(definition))
            {
                continue;
            }
            if (!byAssembly.TryGetValue(owning, out var list))
            {
                byAssembly[owning] = list = new List<string>();
            }
            if (!list.Contains(definition, StringComparer.Ordinal))
            {
                list.Add(definition);
            }
        }
        return byAssembly;
    }

    /// <summary>
    /// The definition side of <c>DefinitionSubjectId</c> must be resolvable to a
    /// handle by parsing only the TypeDefinitions table.  This is the core Task 1
    /// property: arity and method name come from the SubjectId grammar, and the
    /// type comes from a shallow index — never from decoding bodies.
    ///
    /// Asserts against the REAL demand keys, including the generic-TYPE cases
    /// (<c>Action`1::.ctor</c>, <c>Func`2::.ctor</c>) and the ones whose arity
    /// suffix is easy to drop.  A hand-written fixture would not cover those.
    /// </summary>
    [Fact]
    public void ResolvesDefinitionHandles_ForRealDemandKeys()
    {
        var byAssembly = DemandDefinitionsByAssembly("threading-tasks");
        if (byAssembly.Count == 0)
        {
            return; // no artifacts in this checkout; nothing to assert against
        }

        // Chaos.TestFramework.Sdk is present next to the subjects DLL, so it is
        // the one target we can exercise without supplying external assemblies.
        var sdk = Path.Combine(RepoRoot(), "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "threading-tasks", "managed", "Chaos.TestFramework.Sdk.dll");
        Assert.True(File.Exists(sdk), "fixture missing: Chaos.TestFramework.Sdk.dll");
        Assert.True(byAssembly.ContainsKey("Chaos.TestFramework.Sdk"), "fixture has no Sdk demands");
        var definitions = byAssembly["Chaos.TestFramework.Sdk"];
        Assert.NotEmpty(definitions);

        using var stream = File.OpenRead(sdk);
        using var pe = new PEReader(stream);
        var md = pe.GetMetadataReader();

        var typeIndex = InvokeCanonicalTypeIndex(md);

        var resolved = 0;
        var missed = new List<string>();
        foreach (var definition in definitions)
        {
            if (ResolveHandle(md, typeIndex, definition) is not null)
            {
                resolved++;
            }
            else
            {
                missed.Add(definition);
            }
        }

        Assert.True(missed.Count == 0,
            $"{missed.Count}/{definitions.Count} definitions failed to resolve: " +
            string.Join(" | ", missed.Take(5)));
    }

    /// <summary>
    /// Generic-TYPE declaring types (<c>Action`1</c>, <c>Func`2</c>) and
    /// generic-METHOD arity (<c>AppendFormatted`1</c>) must both resolve — these
    /// are the shapes where a naive name lookup silently misses, which is how the
    /// original defect presented.
    /// </summary>
    [Fact]
    public void ResolvesArityBearingShapes()
    {
        // A corelib fixture is not shipped with every checkout; skip cleanly.
        var corelib = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles),
            "dotnet", "shared", "Microsoft.NETCore.App");
        if (!Directory.Exists(corelib))
        {
            return;
        }
        var version = Directory.GetDirectories(corelib).OrderBy(d => d).LastOrDefault();
        if (version is null)
        {
            return;
        }
        var dll = Path.Combine(version, "System.Private.CoreLib.dll");
        if (!File.Exists(dll))
        {
            return;
        }

        using var stream = File.OpenRead(dll);
        using var pe = new PEReader(stream);
        var md = pe.GetMetadataReader();
        var typeIndex = InvokeCanonicalTypeIndex(md);

        // Declaring type carries arity -> System.Action`1 / System.Func`2
        Assert.NotNull(ResolveHandle(md, typeIndex,
            "System.Private.CoreLib/System.Action`1::.ctor:System.Void(System.Object,System.IntPtr)"));
        Assert.NotNull(ResolveHandle(md, typeIndex,
            "System.Private.CoreLib/System.Func`2::.ctor:System.Void(System.Object,System.IntPtr)"));
        // Generic METHOD with a nested generic declaring type
        Assert.NotNull(ResolveHandle(md, typeIndex,
            "System.Private.CoreLib/System.Array::Empty`1:!!0[]()"));
        // Generic method on a generic type
        Assert.NotNull(ResolveHandle(md, typeIndex,
            "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter`1::GetResult:!0()"));
    }

    /// <summary>
    /// A well-formed SubjectId whose type does not exist must resolve to null
    /// rather than throwing — the caller treats null as "fall through to the
    /// previous behaviour", so a throw here would turn a soft miss into a crash.
    /// </summary>
    [Fact]
    public void UnknownType_ReturnsNull_InsteadOfThrowing()
    {
        var sdk = Path.Combine(RepoRoot(), "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "threading-tasks", "managed", "Chaos.TestFramework.Sdk.dll");
        if (!File.Exists(sdk))
        {
            return;
        }

        using var stream = File.OpenRead(sdk);
        using var pe = new PEReader(stream);
        var md = pe.GetMetadataReader();
        var typeIndex = InvokeCanonicalTypeIndex(md);

        var bogus = "Chaos.TestFramework.Sdk/No.Such.Type::NoSuchMethod`1:T():T()";
        Assert.Null(ResolveHandle(md, typeIndex, bogus));
    }

    /// <summary>Malformed input must not throw.</summary>
    [Theory]
    [InlineData("")]
    [InlineData("not-a-subject-id")]
    [InlineData("A/B::C")]              // no parameter list
    public void MalformedSubjectId_ReturnsNull(string subjectId)
    {
        var sdk = Path.Combine(RepoRoot(), "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "threading-tasks", "managed", "Chaos.TestFramework.Sdk.dll");
        if (!File.Exists(sdk))
        {
            return;
        }

        using var stream = File.OpenRead(sdk);
        using var pe = new PEReader(stream);
        var md = pe.GetMetadataReader();
        var typeIndex = InvokeCanonicalTypeIndex(md);

        Assert.Null(ResolveHandle(md, typeIndex, subjectId));
    }

    // ── The resolver's entry points are private on the Loader (they are an
    //    implementation detail of the loading pipeline).  Invoke them
    //    reflectively so the test exercises the REAL implementation rather than
    //    a copy of its logic — a copy could pass while the shipped path is wrong.
    private static object InvokeCanonicalTypeIndex(MetadataReader md)
    {
        var loader = typeof(Chaos.IL2CPP.Loader.LoaderStage);
        var method = loader.GetMethod("BuildTypeHandleIndex",
            BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);
        return method!.Invoke(null, new object[] { md })!;
    }

    private static MethodDefinitionHandle? ResolveHandle(MetadataReader md, object typeIndex, string subjectId)
    {
        var loader = typeof(Chaos.IL2CPP.Loader.LoaderStage);
        var method = loader.GetMethod("ResolveDefinitionHandle",
            BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);
        return (MethodDefinitionHandle?)method!.Invoke(null, new[] { (object)md, typeIndex, subjectId });
    }

    /// <summary>
    /// Resolve definitions through the sparse (on-demand) path.
    /// </summary>
    private static IReadOnlyDictionary<string, ManagedMethodModel> ResolveOnDemand(
        string assemblyPath, IReadOnlyCollection<string> definitionSubjectIds)
    {
        var loader = typeof(Chaos.IL2CPP.Loader.LoaderStage);
        var method = loader.GetMethod("ResolveDefinitionsOnDemand",
            BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);
        return (IReadOnlyDictionary<string, ManagedMethodModel>)method!.Invoke(
            null, new object[] { assemblyPath, definitionSubjectIds })!;
    }

    /// <summary>
    /// Load an assembly the eager way (whole-assembly modelling) for comparison.
    /// </summary>
    private static object LoadAssemblyEagerly(string assemblyPath)
    {
        var loader = typeof(Chaos.IL2CPP.Loader.LoaderStage);
        var method = loader.GetMethod("LoadAssembly",
            BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);
        return method!.Invoke(null, new object?[] { assemblyPath, null, false })!;
    }

    /// <summary>
    /// THE core Task 2 property: a model produced by the sparse on-demand path
    /// must be field-for-field identical to the one the eager whole-assembly path
    /// produces for the same method.
    ///
    /// Without this, "it resolved something" could still mean "it resolved
    /// something subtly different", and the difference would only surface much
    /// later as a mis-lowered method.
    /// </summary>
    [Fact]
    public void SparseResolution_MatchesEagerPath_FieldForField()
    {
        var sdk = Path.Combine(RepoRoot(), "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "threading-tasks", "managed", "Chaos.TestFramework.Sdk.dll");
        if (!File.Exists(sdk))
        {
            return;
        }

        var definitions = DemandDefinitionsByAssembly("threading-tasks")
            .TryGetValue("Chaos.TestFramework.Sdk", out var d) ? d : new List<string>();
        Assert.NotEmpty(definitions);

        var sparse = ResolveOnDemand(sdk, definitions);
        Assert.NotEmpty(sparse);

        var eager = LoadAssemblyEagerly(sdk);
        var eagerMethods = (System.Collections.IEnumerable)eager.GetType()
            .GetProperty("Methods")!.GetValue(eager)!;
        var eagerBySubject = new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal);
        foreach (ManagedMethodModel m in eagerMethods)
        {
            eagerBySubject[m.SubjectId] = m;
        }

        var compared = 0;
        foreach (var (key, sparseModel) in sparse)
        {
            Assert.True(eagerBySubject.ContainsKey(sparseModel.SubjectId),
                $"sparse produced SubjectId absent from eager load: {sparseModel.SubjectId}");
            var eagerModel = eagerBySubject[sparseModel.SubjectId];

            // Compare every field the model exposes.  MetadataToken is included
            // deliberately: if the sparse path resolved a DIFFERENT overload it
            // would differ here even when the SubjectId matched.
            Assert.Equal(eagerModel.AssemblyName, sparseModel.AssemblyName);
            Assert.Equal(eagerModel.DeclaringTypeSubjectId, sparseModel.DeclaringTypeSubjectId);
            Assert.Equal(eagerModel.DeclaringTypeDisplayName, sparseModel.DeclaringTypeDisplayName);
            Assert.Equal(eagerModel.Name, sparseModel.Name);
            Assert.Equal(eagerModel.GenericParameterCount, sparseModel.GenericParameterCount);
            Assert.Equal(eagerModel.ReturnType, sparseModel.ReturnType);
            Assert.Equal(eagerModel.SubjectId, sparseModel.SubjectId);
            Assert.Equal(eagerModel.DefinitionSubjectId, sparseModel.DefinitionSubjectId);
            Assert.Equal(eagerModel.Signature, sparseModel.Signature);
            Assert.Equal(eagerModel.IsStatic, sparseModel.IsStatic);
            Assert.Equal(eagerModel.IsVirtual, sparseModel.IsVirtual);
            Assert.Equal(eagerModel.IsFinal, sparseModel.IsFinal);
            Assert.Equal(eagerModel.IsPreserved, sparseModel.IsPreserved);
            Assert.Equal(eagerModel.IsUnmanagedCallersOnly, sparseModel.IsUnmanagedCallersOnly);
            Assert.Equal(eagerModel.IsPreserveSig, sparseModel.IsPreserveSig);
            Assert.Equal(eagerModel.MetadataToken, sparseModel.MetadataToken);
            Assert.Equal(eagerModel.Parameters.Count, sparseModel.Parameters.Count);
            for (var i = 0; i < eagerModel.Parameters.Count; i++)
            {
                Assert.Equal(eagerModel.Parameters[i].Name, sparseModel.Parameters[i].Name);
                Assert.Equal(eagerModel.Parameters[i].Type, sparseModel.Parameters[i].Type);
            }
            Assert.Equal(eagerModel.Import, sparseModel.Import);
            Assert.Equal(eagerModel.Body.Blocks.Count, sparseModel.Body.Blocks.Count);

            compared++;
        }

        Assert.True(compared > 0, $"compared {compared} methods — expected at least one");
    }

    /// <summary>
    /// The sparse path must not decode methods it was not asked for.  Decoding is
    /// where the cost lives; resolving 1 definition should cost visibly less than
    /// modelling the assembly, and must never return extra entries.
    /// </summary>
    [Fact]
    public void SparseResolution_ReturnsExactlyTheRequestedKeys()
    {
        var sdk = Path.Combine(RepoRoot(), "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "threading-tasks", "managed", "Chaos.TestFramework.Sdk.dll");
        if (!File.Exists(sdk))
        {
            return;
        }

        var one = new List<string>
        {
            "Chaos.TestFramework.Sdk/Chaos.TestFramework.SubjectInstanceFactory::Create`1:!!0()"
        };

        var sparse = ResolveOnDemand(sdk, one);

        Assert.Single(sparse);
        Assert.True(sparse.ContainsKey(one[0]));
    }
}
