using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.HotUpdate;

public sealed class BridgeGenerator
{
    public BridgePlan Generate(BridgeGenerationRequest request)
    {
        ArgumentNullException.ThrowIfNull(request);

        return new BridgePlan
        {
            AotToHotUpdate = request.AotToHotUpdate
                .Select(spec => new AotToHotUpdateBridgeEntry
                {
                    BridgeId = RequireValue(spec.BridgeId, nameof(spec.BridgeId)),
                    HotUpdateAuthorityKey = ResolveAuthorityKey(
                        spec.HotUpdateIdentity,
                        spec.HotUpdateAuthorityKey,
                        nameof(spec.HotUpdateAuthorityKey)),
                    HotUpdateIdentity = spec.HotUpdateIdentity,
                })
                .ToList(),
            HotUpdateToAot = request.HotUpdateToAot
                .Select(spec => new HotUpdateToAotBridgeEntry
                {
                    BridgeId = RequireValue(spec.BridgeId, nameof(spec.BridgeId)),
                    AotAuthorityKey = ResolveAuthorityKey(
                        spec.AotIdentity,
                        spec.AotAuthorityKey,
                        nameof(spec.AotAuthorityKey)),
                    AotIdentity = spec.AotIdentity,
                })
                .ToList(),
            HotUpdateToEngine = request.HotUpdateToEngine
                .Select(spec => new HotUpdateToEngineBridgeEntry
                {
                    BridgeId = RequireValue(spec.BridgeId, nameof(spec.BridgeId)),
                    EngineAuthorityKey = ResolveAuthorityKey(
                        spec.EngineIdentity,
                        spec.EngineAuthorityKey,
                        nameof(spec.EngineAuthorityKey)),
                    EngineIdentity = spec.EngineIdentity,
                })
                .ToList(),
            DelegateWrappers = request.DelegateWrappers
                .Select(spec => new DelegateWrapperEntry
                {
                    WrapperId = RequireValue(spec.WrapperId, nameof(spec.WrapperId)),
                    HotUpdateAuthorityKey = ResolveAuthorityKey(
                        spec.HotUpdateIdentity,
                        spec.HotUpdateAuthorityKey,
                        nameof(spec.HotUpdateAuthorityKey)),
                    HotUpdateIdentity = spec.HotUpdateIdentity,
                })
                .ToList(),
        };
    }

    private static string ResolveAuthorityKey(
        ManagedMethodIdentityArtifact? identity,
        string? authorityKey,
        string parameterName)
    {
        if (!string.IsNullOrWhiteSpace(authorityKey))
        {
            return authorityKey;
        }

        if (identity is not null)
        {
            return ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity);
        }

        throw new InvalidOperationException($"bridge generation requires '{parameterName}' or an identity-backed execution authority.");
    }

    private static string RequireValue(string? value, string parameterName)
    {
        if (!string.IsNullOrWhiteSpace(value))
        {
            return value;
        }

        throw new InvalidOperationException($"bridge generation requires '{parameterName}'.");
    }
}

public sealed record BridgeGenerationRequest
{
    public IReadOnlyList<AotToHotUpdateBridgeSpec> AotToHotUpdate { get; init; } = [];

    public IReadOnlyList<HotUpdateToAotBridgeSpec> HotUpdateToAot { get; init; } = [];

    public IReadOnlyList<HotUpdateToEngineBridgeSpec> HotUpdateToEngine { get; init; } = [];

    public IReadOnlyList<DelegateWrapperSpec> DelegateWrappers { get; init; } = [];
}

public sealed record AotToHotUpdateBridgeSpec
{
    public required string BridgeId { get; init; }

    public string? HotUpdateAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? HotUpdateIdentity { get; init; }
}

public sealed record HotUpdateToAotBridgeSpec
{
    public required string BridgeId { get; init; }

    public string? AotAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? AotIdentity { get; init; }
}

public sealed record HotUpdateToEngineBridgeSpec
{
    public required string BridgeId { get; init; }

    public string? EngineAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? EngineIdentity { get; init; }
}

public sealed record DelegateWrapperSpec
{
    public required string WrapperId { get; init; }

    public string? HotUpdateAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? HotUpdateIdentity { get; init; }
}

public sealed record BridgePlan
{
    public required IReadOnlyList<AotToHotUpdateBridgeEntry> AotToHotUpdate { get; init; }

    public required IReadOnlyList<HotUpdateToAotBridgeEntry> HotUpdateToAot { get; init; }

    public required IReadOnlyList<HotUpdateToEngineBridgeEntry> HotUpdateToEngine { get; init; }

    public required IReadOnlyList<DelegateWrapperEntry> DelegateWrappers { get; init; }

    public IReadOnlyList<AutoGeneratedBridgeEntry> AutoGenerated { get; init; } = [];
}

public sealed record AotToHotUpdateBridgeEntry
{
    public required string BridgeId { get; init; }

    public required string HotUpdateAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? HotUpdateIdentity { get; init; }
}

public sealed record HotUpdateToAotBridgeEntry
{
    public required string BridgeId { get; init; }

    public required string AotAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? AotIdentity { get; init; }
}

public sealed record HotUpdateToEngineBridgeEntry
{
    public required string BridgeId { get; init; }

    public required string EngineAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? EngineIdentity { get; init; }
}

public sealed record DelegateWrapperEntry
{
    public required string WrapperId { get; init; }

    public required string HotUpdateAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? HotUpdateIdentity { get; init; }
}

public sealed record AutoGeneratedBridgeEntry
{
    public required string BridgeId { get; init; }

    public required string TargetAuthorityKey { get; init; }

    public ManagedMethodIdentityArtifact? TargetIdentity { get; init; }

    public required string SignatureKey { get; init; }

    public required BridgeDispatchStyle DispatchStyle { get; init; }

    public required BridgeCarrierSchema CarrierSchema { get; init; }
}
