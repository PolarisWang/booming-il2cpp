using System.Collections.Generic;

namespace Chaos.TestFramework.Runtime.Collection;

/// <summary>
/// Runtime representation of a collected test subject entry, as materialized by
/// <see cref="ChaosTestCollectionLoader"/> from a collection manifest. Kept minimal and
/// frozen so the contract is stable.
/// </summary>
public sealed class ChaosTestCollection
{
    public int SchemaVersion { get; set; } = 1;
    public string SubjectId { get; set; } = string.Empty;
    public bool FrameworkReferenced { get; set; }
    public string SubjectKind { get; set; } = string.Empty;
    public List<string> WarningCodes { get; set; } = [];
}
