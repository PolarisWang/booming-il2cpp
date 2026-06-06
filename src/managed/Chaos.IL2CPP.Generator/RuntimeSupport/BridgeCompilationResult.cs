using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Holds bridge compilation results for Phase 1b (C++ redirect table emission).
/// Created in CodeGenStage and consumed by NativeAotEmitter.
/// </summary>
public sealed class BridgeCompilationResult
{
    /// <summary>Compiled method artifacts with chaos_bridge_ symbols.</summary>
    public IReadOnlyList<AotCoreIrMethodArtifact> CompiledMethods { get; init; }
        = [];

    /// <summary>Maps SubjectId → chaos_bridge_ symbol name.</summary>
    public IReadOnlyDictionary<string, string> RedirectMap { get; init; }
        = new Dictionary<string, string>();
}
