using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static List<LoadedAssemblyModel> ProjectCrossAssemblyMethodInstantiations(
        IReadOnlyList<string> assemblyPaths,
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies)
    {
        // Use Last-wins dedup: when --assembly-dir provides the same assembly that
        // closure analysis also discovers, prefer whichever was loaded.  ToDictionary
        // would throw "An item with the same key has already been added."
        static Dictionary<string, TValue> BuildAssemblyDict<TValue>(
            IReadOnlyList<LoadedAssemblyModel> assemblies,
            Func<LoadedAssemblyModel, TValue> valueSelector)
        {
            var dict = new Dictionary<string, TValue>(assemblies.Count, StringComparer.Ordinal);
            foreach (var a in assemblies)
            {
                var name = a.Assembly.Name;
                if (string.IsNullOrEmpty(name)) continue;
                dict[name] = valueSelector(a);  // Last wins — harmless if identical
            }
            return dict;
        }

        var genericInstantiationDemandEntriesByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            _ => new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal));
        var definitionMethodsByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            assembly => (IReadOnlyDictionary<string, ManagedMethodModel>)assembly.Methods
                .ToDictionary(method => method.SubjectId, StringComparer.Ordinal));
        var projectedMethodsByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            assembly => assembly.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal));
        var valueTypeSubjectIdsByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            assembly => (IReadOnlySet<string>)assembly.Types
                .Where(t => t.IsValueType && !string.IsNullOrEmpty(t.SubjectId))
                .Select(t => t.SubjectId)
                .ToHashSet(StringComparer.Ordinal));

        // ── Phase 1: collect cross-assembly demands ─────────────────────────
        //
        // Demands must be gathered BEFORE any projection is attempted, and
        // WITHOUT requiring the target assembly to be loaded.  The earlier
        // design tracked the demand only inside the projection attempt, whose
        // first guard returns early when the target assembly is absent — so a
        // method referencing e.g. System.Private.CoreLib recorded nothing here
        // and the instance silently vanished.
        //
        // Collection is cheap: it reads the assembly's MemberReference /
        // MethodSpec tables and derives the definition key.  No definition is
        // resolved and no body decoded at this stage.
        var pendingDemands = new Dictionary<string, List<MethodReferenceSummary>>(StringComparer.Ordinal);
        foreach (var assemblyPath in assemblyPaths)
        {
            CollectCrossAssemblyDemands(assemblyPath, pendingDemands);
        }

        // ── Phase 2: resolve the named definitions on demand ─────────────────
        //
        // Only the definitions actually demanded are resolved, and only for
        // assemblies not already loaded (the eager version, when present, is
        // authoritative and cheaper to reuse than to re-resolve).
        ResolveMissingDefinitionAssemblies(
            pendingDemands,
            assemblyPaths,
            definitionMethodsByAssembly,
            projectedMethodsByAssembly,
            genericInstantiationDemandEntriesByAssembly,
            valueTypeSubjectIdsByAssembly);

        // ── Phase 3: track + project ─────────────────────────────────────────
        foreach (var assemblyPath in assemblyPaths)
        {
            CollectProjectedCrossAssemblyMethodInstantiations(
                assemblyPath,
                definitionMethodsByAssembly,
                projectedMethodsByAssembly,
                genericInstantiationDemandEntriesByAssembly,
                valueTypeSubjectIdsByAssembly);
        }

        return loadedAssemblies
            .Select(assembly =>
            {
                var mergedDemandGraph = MergeGenericInstantiationDemandGraphs(
                    assembly.GenericInstantiationDemandGraph,
                    BuildGenericInstantiationDemandGraph(genericInstantiationDemandEntriesByAssembly[assembly.Assembly.Name]));
                var projectedMethods = projectedMethodsByAssembly[assembly.Assembly.Name].Values
                    .OrderBy(model => model.MetadataToken)
                    .ThenBy(model => model.SubjectId, StringComparer.Ordinal)
                    .ToList();

                return assembly with
                {
                    Methods = ApplyDemandDerivedRuntimeGenericContexts(projectedMethods, mergedDemandGraph),
                    GenericInstantiationDemandGraph = mergedDemandGraph,
                };
            })
            .ToList();
    }

    /// <summary>
    /// Walk an assembly's MemberReference / MethodSpec tables and record every
    /// cross-assembly method instantiation it demands, keyed by the assembly
    /// that OWNS the definition.
    ///
    /// <para>
    /// Deliberately does not check whether the owning assembly is available:
    /// that check belongs to resolution, not collection.  Splitting them is what
    /// lets a demand survive long enough to be satisfied on demand.
    /// </para>
    /// </summary>
    private static void CollectCrossAssemblyDemands(
        string assemblyPath,
        Dictionary<string, List<MethodReferenceSummary>> pendingDemands)
    {
        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();
        var sourceAssemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var typeResolver = new MetadataTypeResolver(metadataReader, sourceAssemblyName);
        var typeModels = LoadTypes(metadataReader, typeResolver, sourceAssemblyName);
        var ownerIndex = BuildOwnerIndex(metadataReader, typeResolver, typeModels);

        void Record(MethodReferenceSummary methodReference)
        {
            // Intra-assembly references are handled by the in-assembly projection
            // path; only cross-assembly ones need a definition lookup elsewhere.
            if (string.Equals(methodReference.AssemblyName, sourceAssemblyName, StringComparison.Ordinal))
            {
                return;
            }

            if (!pendingDemands.TryGetValue(methodReference.AssemblyName, out var list))
            {
                pendingDemands[methodReference.AssemblyName] = list = new List<MethodReferenceSummary>();
            }

            list.Add(methodReference);
        }

        foreach (var memberReferenceHandle in metadataReader.MemberReferences)
        {
            var memberReference = metadataReader.GetMemberReference(memberReferenceHandle);
            if (memberReference.GetKind() != MemberReferenceKind.Method)
            {
                continue;
            }

            Record(DescribeMemberReferenceMethod(metadataReader, typeResolver, memberReferenceHandle));
        }

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.MethodSpec); rowNumber++)
        {
            var methodSpecificationHandle = MetadataTokens.MethodSpecificationHandle(rowNumber);
            Record(DescribeMethodSpecification(
                metadataReader, typeResolver, typeModels, ownerIndex.MethodOwners, methodSpecificationHandle));
        }
    }

    /// <summary>
    /// Ensure every assembly named by a pending demand has its definitions
    /// available, resolving ONLY the demanded methods.
    ///
    /// <para>
    /// Assemblies already loaded keep their eagerly-built dictionary — that is
    /// the authoritative model and re-deriving it would be wasted work.  For the
    /// rest, the definition is resolved from the assembly file directly whenever
    /// the file can be located; the sparse model produced this way is
    /// field-for-field identical to the eager one (locked by
    /// OnDemandDefinitionResolverTests.SparseResolution_MatchesEagerPath).
    /// </para>
    ///
    /// <para>
    /// If the assembly file cannot be located, the demand is left unresolvable
    /// and the projection for it will simply not happen — exactly the previous
    /// behaviour, so this can only ever add projections, never remove them.
    /// </para>
    /// </summary>
    private static void ResolveMissingDefinitionAssemblies(
        Dictionary<string, List<MethodReferenceSummary>> pendingDemands,
        IReadOnlyList<string> assemblyPaths,
        Dictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        Dictionary<string, Dictionary<string, ManagedMethodModel>> projectedMethodsByAssembly,
        Dictionary<string, Dictionary<string, GenericInstantiationDemandModel>> genericInstantiationDemandEntriesByAssembly,
        Dictionary<string, IReadOnlySet<string>> valueTypeSubjectIdsByAssembly)
    {
        // Assemblies whose definitions are already available (loaded eagerly, or
        // resolved earlier in this loop) — do not re-resolve.
        var satisfied = new HashSet<string>(StringComparer.Ordinal);
        foreach (var name in definitionMethodsByAssembly.Keys)
        {
            satisfied.Add(name);
        }

        foreach (var (owningAssemblyName, references) in pendingDemands)
        {
            if (!satisfied.Add(owningAssemblyName))
            {
                continue;
            }

            var assemblyFile = LocateAssemblyFile(owningAssemblyName, assemblyPaths);
            if (assemblyFile is null)
            {
                continue;
            }

            var definitionSubjectIds = new HashSet<string>(StringComparer.Ordinal);
            foreach (var reference in references)
            {
                if (!string.IsNullOrEmpty(reference.DefinitionSubjectId))
                {
                    definitionSubjectIds.Add(reference.DefinitionSubjectId);
                }
            }

            if (definitionSubjectIds.Count == 0)
            {
                continue;
            }

            var resolved = ResolveDefinitionsOnDemand(assemblyFile, definitionSubjectIds);
            if (resolved.Count == 0)
            {
                continue;
            }

            // Publish under the assembly's own name so lookups by
            // methodReference.AssemblyName succeed.  The parameters are concrete
            // Dictionary types (not IReadOnlyDictionary) so this cannot silently
            // no-op: a read-only view handed in here would be a compile error
            // rather than a feature that quietly stops working.
            //
            // The projected map seeds with the definitions themselves — exactly
            // as BuildAssemblyDict does for eagerly-loaded assemblies — because
            // ProjectInstantiationMethod writes its results into this map and
            // expects the definition entries to already be present.
            definitionMethodsByAssembly[owningAssemblyName] = resolved;
            projectedMethodsByAssembly[owningAssemblyName] =
                new Dictionary<string, ManagedMethodModel>(resolved, StringComparer.Ordinal);
            genericInstantiationDemandEntriesByAssembly[owningAssemblyName] =
                new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
            valueTypeSubjectIdsByAssembly[owningAssemblyName] = new HashSet<string>(StringComparer.Ordinal);
        }
    }

    /// <summary>
    /// Find the file for an assembly name among the known input paths and their
    /// sibling directories.  Returns null when it cannot be found — the caller
    /// treats that as "leave the demand unsatisfied", not as an error.
    /// </summary>
    private static string? LocateAssemblyFile(string assemblyName, IReadOnlyList<string> assemblyPaths)
    {
        foreach (var path in assemblyPaths)
        {
            if (string.Equals(Path.GetFileNameWithoutExtension(path), assemblyName, StringComparison.OrdinalIgnoreCase))
            {
                return path;
            }

            var directory = Path.GetDirectoryName(path);
            if (string.IsNullOrEmpty(directory))
            {
                continue;
            }

            var candidate = Path.Combine(directory, assemblyName + ".dll");
            if (File.Exists(candidate))
            {
                return candidate;
            }
        }

        return null;
    }

    private static void CollectProjectedCrossAssemblyMethodInstantiations(
        string assemblyPath,
        IReadOnlyDictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, ManagedMethodModel>> projectedMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, GenericInstantiationDemandModel>> genericInstantiationDemandEntriesByAssembly,
        IReadOnlyDictionary<string, IReadOnlySet<string>> valueTypeSubjectIdsByAssembly)
    {
        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var typeResolver = new MetadataTypeResolver(metadataReader, assemblyName);
        var typeModels = LoadTypes(metadataReader, typeResolver, assemblyName);
        var ownerIndex = BuildOwnerIndex(metadataReader, typeResolver, typeModels);

        foreach (var memberReferenceHandle in metadataReader.MemberReferences)
        {
            var memberReference = metadataReader.GetMemberReference(memberReferenceHandle);
            if (memberReference.GetKind() != MemberReferenceKind.Method)
            {
                continue;
            }

            TryProjectCrossAssemblyMethodInstantiation(
                assemblyName,
                DescribeMemberReferenceMethod(metadataReader, typeResolver, memberReferenceHandle),
                definitionMethodsByAssembly,
                projectedMethodsByAssembly,
                genericInstantiationDemandEntriesByAssembly,
                valueTypeSubjectIdsByAssembly,
                demandSourceKind: "memberReference");
        }

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.MethodSpec); rowNumber++)
        {
            var methodSpecificationHandle = MetadataTokens.MethodSpecificationHandle(rowNumber);
            TryProjectCrossAssemblyMethodInstantiation(
                assemblyName,
                DescribeMethodSpecification(
                    metadataReader,
                    typeResolver,
                    typeModels,
                    ownerIndex.MethodOwners,
                    methodSpecificationHandle),
                definitionMethodsByAssembly,
                projectedMethodsByAssembly,
                genericInstantiationDemandEntriesByAssembly,
                valueTypeSubjectIdsByAssembly,
                demandSourceKind: "methodSpec");
        }
    }

    private static void TryProjectCrossAssemblyMethodInstantiation(
        string sourceAssemblyName,
        MethodReferenceSummary methodReference,
        IReadOnlyDictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, ManagedMethodModel>> projectedMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, GenericInstantiationDemandModel>> genericInstantiationDemandEntriesByAssembly,
        IReadOnlyDictionary<string, IReadOnlySet<string>> valueTypeSubjectIdsByAssembly,
        string demandSourceKind)
    {
        if (string.Equals(methodReference.AssemblyName, sourceAssemblyName, StringComparison.Ordinal) ||
            !definitionMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetDefinitionMethods) ||
            !projectedMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetProjectedMethods) ||
            !genericInstantiationDemandEntriesByAssembly.TryGetValue(methodReference.AssemblyName, out var targetDemandEntries) ||
            !valueTypeSubjectIdsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetValueTypeSubjectIds))
        {
            return;
        }

        TrackMethodInstantiationDemand(
            genericInstantiationDemandEntries: targetDemandEntries,
            requestingAssemblyName: sourceAssemblyName,
            methodReference: methodReference,
            demandSourceKind: demandSourceKind,
            valueTypeSubjectIds: targetValueTypeSubjectIds);
        ProjectInstantiationMethod(
            methodReference.AssemblyName,
            methodReference,
            targetDefinitionMethods,
            EmptyFieldBindings,
            targetProjectedMethods);
    }

    private static LoadedAssemblyModel ResolveEntryAssembly(
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies,
        string? entryPointSubjectIdOverride,
        bool fullAssemblyClosure)
    {
        if (!string.IsNullOrWhiteSpace(entryPointSubjectIdOverride))
        {
            var entryAssembly = loadedAssemblies.FirstOrDefault(assembly =>
                assembly.Methods.Any(method => string.Equals(method.SubjectId, entryPointSubjectIdOverride, StringComparison.Ordinal)));
            if (entryAssembly is not null)
            {
                return entryAssembly;
            }

            throw new InvalidOperationException(
                $"managed closure entry point override '{entryPointSubjectIdOverride}' does not match any loaded method");
        }

        var primaryAssembly = loadedAssemblies[0];
        if (!string.IsNullOrWhiteSpace(primaryAssembly.EntryPointSubjectId))
        {
            return primaryAssembly;
        }

        if (fullAssemblyClosure)
        {
            return primaryAssembly;
        }

        throw new InvalidOperationException("managed closure input assembly does not define an entry point");
    }

}
