using System.Collections.Immutable;
using System.Reflection.Metadata;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

internal sealed record SignatureContext<T>(ImmutableArray<T> TypeArguments, ImmutableArray<T> MethodArguments);

internal sealed record TypeIdentity
{
    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public required string DisplayName { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DefinitionDisplayName { get; init; }

    public required ImmutableArray<string> TypeArguments { get; init; }
}

internal record class MethodReferenceSummary
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DeclaringTypeDisplayName { get; init; }

    public required string Name { get; init; }

    public int GenericParameterCount { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string ReturnType { get; init; }

    public required IReadOnlyList<string> ParameterTypes { get; init; }

    public required int MetadataToken { get; init; }

    public required ImmutableDictionary<string, string> Substitutions { get; init; }
}

internal sealed record FieldReferenceSummary
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string Name { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string FieldType { get; init; }

    public bool IsStatic { get; init; }

    public bool IsThreadStatic { get; init; }

    public required int MetadataToken { get; init; }

    public required ImmutableDictionary<string, string> Substitutions { get; init; }
}

internal sealed record MethodSummary : MethodReferenceSummary
{
    public required string Signature { get; init; }

    public required bool IsStatic { get; init; }

    public required bool IsVirtual { get; init; }

    public bool IsPreserved { get; init; }

    public bool IsUnmanagedCallersOnly { get; init; }

    public required IReadOnlyList<ManagedParameterModel> Parameters { get; init; }

    public ManagedImportModel? Import { get; init; }
}

internal sealed record MaterializedGenericModels(
    IReadOnlyList<ManagedTypeModel> Types,
    IReadOnlyList<ManagedFieldModel> Fields,
    IReadOnlyList<ManagedPropertyModel> Properties,
    IReadOnlyList<ManagedMethodModel> Methods);

internal sealed record FieldBindingKey(string DefinitionSubjectId, string DeclaringTypeSubjectId);

internal sealed record OwnerIndex(
    IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> FieldOwners,
    IReadOnlyDictionary<PropertyDefinitionHandle, ManagedTypeModel> PropertyOwners,
    IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> MethodOwners);
