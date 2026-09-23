using System;
using System.IO;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Infra;

/// <summary>
/// A <see cref="FactAttribute"/> for tests that assert properties of <b>real
/// emitted codegen output</b> — the <c>artifacts/foundation-dll/&lt;assembly&gt;/chunks/&lt;chunk&gt;
/// /native/subjects</c> tree produced by a chunk build.
///
/// <para>
/// These tests exist because some contracts can only be checked against what the
/// compiler actually sees: a synthetic model built in-process never exercises the
/// payload partitioning, the cross-section declarations, or the reflection
/// table grouping, so a unit test driving a fixture would pass vacuously. That
/// makes them valuable — and unusable on a machine that has not run a chunk
/// build, which is exactly the situation in CI (a fresh checkout has no
/// <c>artifacts/</c>).
/// </para>
///
/// <para>
/// Failing there would be wrong: the absence of build output says nothing about
/// the code under test. Skipping silently would also be wrong — these guards
/// caught real defects, and a guard that quietly does nothing is worse than no
/// guard. So the skip is <b>conditional on the artifacts genuinely being
/// absent</b>, and the reason is reported on the test result, where a reviewer
/// can see that the guard did not run.
/// </para>
///
/// <para>
/// To run them locally:
/// <code>
/// python -m verification.chunk_pipeline --assembly System.Private.CoreLib \
///     --chunk system --stages build
/// </code>
/// </para>
/// </summary>
public sealed class RequiresEmittedCodegenFactAttribute : FactAttribute
{
    public RequiresEmittedCodegenFactAttribute(
        string assembly = "System.Private.CoreLib",
        string chunk = "system")
    {
        string subjectsDir = Path.Combine(
            RepoRootLocator.FindFromBaseDirectory(),
            "artifacts", "foundation-dll", assembly,
            "chunks", chunk, "native", "subjects");

        if (!Directory.Exists(subjectsDir))
        {
            Skip = $"no emitted codegen output at '{subjectsDir}' — run the {chunk} chunk "
                 + "build first: python -m verification.chunk_pipeline "
                 + $"--assembly {assembly} --chunk {chunk} --stages build";
        }
    }
}
