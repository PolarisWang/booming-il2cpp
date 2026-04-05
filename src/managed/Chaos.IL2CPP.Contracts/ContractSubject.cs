namespace Chaos.IL2CPP.Contracts;

public readonly record struct SubjectId(string Value)
{
    public override string ToString() => Value;
}

public sealed record ContractSubject(SubjectId Id, string Kind, string DisplayName);
