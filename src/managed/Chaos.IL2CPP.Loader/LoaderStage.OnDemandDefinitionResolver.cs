using System.Collections.Immutable;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

/// <summary>
/// Resolves generic-instantiation DEFINITIONS on demand, without paying the cost
/// of <see cref="LoaderStage.LoadAssembly"/>'s whole-assembly modelling.
///
/// <para>
/// Why this exists: cross-assembly generic method instances (e.g.
/// <c>Assert.AreEqual&lt;T&gt;</c>, <c>SubjectInstanceFactory.Create&lt;T&gt;</c>)
/// are materialised from the DEFINITION's <c>ManagedMethodModel</c>, looked up by
/// <c>DefinitionSubjectId</c>.  The previous implementation could only satisfy
/// that lookup from <c>definitionMethodsByAssembly</c>, which is built by loading
/// and fully modelling every assembly on the search path.  For
/// <c>System.Private.CoreLib</c> — ~131x the size of a subjects assembly — that
/// enumeration never converges (measured: 1.8 GB steady / 776s CPU with only five
/// BCL assemblies supplied, no codegen output).
/// </para>
///
/// <para>
/// The observation that makes this tractable: the consumer only ever needs a
/// handful of SPECIFIC methods.  On the threading-tasks chunk, 140 demands
/// collapse to <b>89 distinct definitionSubjectIds</b>.  So instead of "load two
/// assemblies and model everything", this resolves "the 89 methods that were
/// actually named".
/// </para>
///
/// <para>
/// Mechanism: the type/field/method owner dictionaries used during method
/// decoding are a CACHE with a fallback, not a hard dependency — every lookup is
/// <c>TryGetValue</c> guarded by a path that reconstructs the model from PE
/// metadata (<see cref="LoaderStage.DescribeMethodDefinition"/> falls back to
/// reconstructing the owning type, which in turn falls back to
/// <c>CreateTypeModel</c>).  <c>DecodeMethodBody</c> itself never indexes those
/// dictionaries at all.  Passing empty owner dictionaries is therefore sound;
/// it costs some repeated type reconstruction, not correctness.
/// </para>
/// </summary>
public sealed partial class LoaderStage
{
    /// <summary>
    /// Maps a type's full name (as it appears in a SubjectId's declaring-type
    /// part) to its handle.  Built by walking ONLY the TypeDefinitions table —
    /// no signatures are decoded, no bodies read.
    /// </summary>
    private static Dictionary<string, TypeDefinitionHandle> BuildTypeHandleIndex(MetadataReader metadataReader)
    {
        var index = new Dictionary<string, TypeDefinitionHandle>(StringComparer.Ordinal);

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            var typeName = metadataReader.GetString(typeDefinition.Name);
            var namespaceName = metadataReader.GetString(typeDefinition.Namespace);

            // SubjectId declaring-type part is "Namespace.TypeName" (or bare
            // "TypeName" for the global namespace).  Nested types use the
            // metadata's '+' separator in TypeDefinition.Name, so the composed
            // full name matches the metadata spelling, not the C# one.
            var fullName = string.IsNullOrEmpty(namespaceName) ? typeName : $"{namespaceName}.{typeName}";
            index[fullName] = typeHandle;
        }

        return index;
    }

    /// <summary>
    /// Resolve a method's <c>DefinitionSubjectId</c> to its handle in this
    /// assembly, or <c>null</c> when it cannot be found.
    ///
    /// <para>
    /// Grammar (see <see cref="ManagedNaming.CreateMethodSubjectId"/>):
    /// <c>{declaringTypeSubjectId}::{methodName}`N:returnType(paramTypes)</c>.
    /// The declaring-type part carries an assembly prefix
    /// (<c>Assembly/Namespace.Type</c>), which is stripped here — the caller has
    /// already opened the assembly named in that prefix.
    /// </para>
    ///
    /// <para>
    /// Matching is by (stripped method name, generic arity).  Arity comes from
    /// the "`N" suffix the naming helper appends; it is NOT recomputed, so this
    /// cannot drift from the definitive arity logic that
    /// <c>MetadataResolution</c> uses when producing the key in the first place.
    /// </para>
    /// </summary>
    private static MethodDefinitionHandle? ResolveDefinitionHandle(
        MetadataReader metadataReader,
        Dictionary<string, TypeDefinitionHandle> typeIndex,
        string definitionSubjectId)
    {
        if (!ManagedNaming.TryParseMethodSubjectId(
                definitionSubjectId,
                out var declaringTypeSubjectId,
                out var methodName,
                out _,
                out _))
        {
            return null;
        }

        // "Assembly/Namespace.Type" -> "Namespace.Type"
        var slashIndex = declaringTypeSubjectId.IndexOf('/');
        var typeFullName = slashIndex >= 0
            ? declaringTypeSubjectId[(slashIndex + 1)..]
            : declaringTypeSubjectId;

        if (!typeIndex.TryGetValue(typeFullName, out var typeHandle))
        {
            return null;
        }

        // "Create`1" -> name "Create", arity 1; "Create" -> arity 0.
        var tickIndex = methodName.LastIndexOf('`');
        var bareName = tickIndex >= 0 ? methodName[..tickIndex] : methodName;
        var arity = 0;
        if (tickIndex >= 0 &&
            !int.TryParse(methodName[(tickIndex + 1)..], out arity))
        {
            return null;
        }

        var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
        foreach (var candidateHandle in typeDefinition.GetMethods())
        {
            var candidate = metadataReader.GetMethodDefinition(candidateHandle);
            if (!string.Equals(metadataReader.GetString(candidate.Name), bareName, StringComparison.Ordinal))
            {
                continue;
            }

            if (candidate.GetGenericParameters().Count == arity)
            {
                return candidateHandle;
            }
        }

        return null;
    }

    /// <summary>
    /// Resolve the named definitions into <see cref="ManagedMethodModel"/>s,
    /// decoding ONLY those methods.
    ///
    /// <para>
    /// Reuses the existing per-method units
    /// (<see cref="LoaderStage.DescribeMethodDefinition"/> +
    /// <see cref="LoaderStage.DecodeMethodBody"/>) rather than re-implementing
    /// them.  The owner dictionaries are passed EMPTY on purpose — see the type
    /// remarks: every consumer of them falls back to metadata reconstruction.
    /// </para>
    /// </summary>
    private static Dictionary<string, ManagedMethodModel> ResolveDefinitionsOnDemand(
        string assemblyPath,
        IReadOnlyCollection<string> definitionSubjectIds)
    {
        var resolved = new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal);
        if (definitionSubjectIds.Count == 0)
        {
            return resolved;
        }

        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var typeResolver = new MetadataTypeResolver(metadataReader, assemblyName);

        var typeIndex = BuildTypeHandleIndex(metadataReader);
        var emptyTypes = new Dictionary<TypeDefinitionHandle, ManagedTypeModel>();
        var emptyFieldOwners = new Dictionary<FieldDefinitionHandle, ManagedTypeModel>();
        var emptyMethodOwners = new Dictionary<MethodDefinitionHandle, ManagedTypeModel>();

        foreach (var definitionSubjectId in definitionSubjectIds)
        {
            if (resolved.ContainsKey(definitionSubjectId))
            {
                continue;
            }

            var handle = ResolveDefinitionHandle(metadataReader, typeIndex, definitionSubjectId);
            if (handle is null)
            {
                continue;
            }

            var methodHandle = handle.Value;

            // Prefer the method's OWN record of its SubjectId over the requested
            // key: they are normally identical, but deriving the model's identity
            // from metadata keeps this consistent with the eager path even if a
            // caller passes an equivalent-but-differently-spelled key.
            var summary = DescribeMethodDefinition(
                metadataReader,
                typeResolver,
                emptyTypes,
                emptyMethodOwners,
                assemblyName,
                methodHandle);

            var declaringType = ResolveOwningTypeModel(metadataReader, typeResolver, emptyTypes, methodHandle);
            var body = DecodeMethodBody(
                peReader,
                metadataReader,
                typeResolver,
                emptyTypes,
                emptyFieldOwners,
                emptyMethodOwners,
                methodHandle,
                summary);

            resolved[definitionSubjectId] = new ManagedMethodModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = declaringType.SubjectId,
                DeclaringTypeDisplayName = declaringType.DisplayName,
                Name = summary.Name,
                GenericParameterCount = summary.GenericParameterCount,
                ReturnType = summary.ReturnType,
                SubjectId = summary.SubjectId,
                DefinitionSubjectId = summary.DefinitionSubjectId,
                Signature = ManagedNaming.CreateMethodSignature(
                    summary.ReturnType,
                    declaringType.DisplayName,
                    summary.Name,
                    summary.Parameters.Select(parameter => parameter.Type).ToList()),
                IsStatic = summary.IsStatic,
                IsVirtual = summary.IsVirtual,
                IsFinal = summary.IsFinal,
                IsPreserved = summary.IsPreserved,
                IsUnmanagedCallersOnly = summary.IsUnmanagedCallersOnly,
                IsPreserveSig = summary.IsPreserveSig,
                MetadataToken = MetadataTokens.GetToken(methodHandle),
                Parameters = summary.Parameters,
                Import = null,
                Body = body,
            };
        }

        return resolved;
    }
}
