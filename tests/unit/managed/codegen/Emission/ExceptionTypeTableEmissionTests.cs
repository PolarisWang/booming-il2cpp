using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Regression tests for the emitted exception type table
/// (<c>BuildExceptionTypeTable</c> in ModuleRegistration.Dispatch.cs).
///
/// Background: this table was introduced so <c>ResolveTypeByName()</c> can resolve
/// types that are only ever thrown (never lowered). A malformed entry breaks the
/// *whole translation unit*, not just the table — so these tests assert the emitted
/// text is syntactically well-formed C++ rather than merely non-empty.
/// </summary>
public sealed class ExceptionTypeTableEmissionTests
{
    private readonly PlannerFixture _fixture = new();

    private const string CatchMarker = "--- Exception Type Table";

    /// <summary>
    /// Emits a method whose catch clause names a type, then returns the slice of
    /// generated object-model code covering the exception type table.
    /// </summary>
    private string EmitExceptionTable()
    {
        const string catchType = "System.Private.CoreLib/System.Exception";

        // Production IR carries the caught type BOTH as an exception-region
        // catchTypeSubjectId AND (via the exception object's own reference) as a
        // ReferenceType TargetReference. BuildExceptionTypeTable intersects the
        // catch set with the reference-type set, so the test must supply both to
        // reach the emit path rather than the count==0 early return.
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CatchSomething",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "newobj",
                    IlOffset = 0x00,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        AssemblyName = "System.Private.CoreLib",
                        SubjectId = catchType,
                        BaseTypeSubjectId = null,
                    },
                },
                ModelFactory.Instruction("ldc.i4", ilOffset: 0x02, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 0x03),
            },
            exceptionRegions: new[]
            {
                ModelFactory.CatchRegion(
                    tryOffset: 0x00, tryLength: 0x02,
                    handlerOffset: 0x02, handlerLength: 0x01,
                    catchTypeSubjectId: catchType),
            });

        var artifact = ModelFactory.CreateArtifact(method);
        var loweringPlan = ModelFactory.CreateDefaultPlan(method.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);

        var templateModel = _fixture.RunPlanner(artifact, loweringPlan, manifest);

        // The table is appended to the module-registration block, not the
        // object-model block (see NativeAotLoweringPlanner.Methods.cs).
        var code = templateModel.ModuleRegistrationCode;
        Assert.NotNull(code);

        var start = code.IndexOf(CatchMarker, StringComparison.Ordinal);
        Assert.True(start >= 0,
            "expected the exception type table to be emitted for a method with a catch "
            + "clause naming a reference type:\n" + code);

        return code.Substring(start);
    }

    /// <summary>
    /// THE regression this file exists for.
    ///
    /// Each <c>kExcDesc&lt;i&gt;</c> is a *standalone* declaration:
    /// <code>
    /// static const ...ReflectionQueryTypeDescriptor kExcDesc0 = { ... };
    /// static const ...ReflectionQueryTypeDescriptor kExcDesc1 = { ... };
    /// </code>
    /// so each initializer must close with <c>};</c>. When it closed with <c>},</c>
    /// instead, MSVC parsed <c>kExcDesc&lt;i+1&gt;</c> as a continued declarator of
    /// <c>kExcDesc&lt;i&gt;</c> and reported, at the following <c>static const</c>:
    /// <code>
    /// error C2226: syntax error: unexpected type 'ReflectionQueryTypeDescriptor'
    /// error C2143: syntax error: missing ';' before '{'
    /// error C2447: '{': missing function header
    /// </code>
    /// </summary>
    [Fact]
    public void EveryExcDescDeclaration_IsTerminatedWithSemicolon()
    {
        var table = EmitExceptionTable();

        // Pair each declaration with the terminator that closes ITS initializer.
        // Counting terminators globally is not enough: the kChaosExceptionTypes[]
        // array that follows also ends with `};`, so a globally-counted assertion
        // passes even when every kExcDesc ends with `},`. Anchor each search to the
        // declaration's own start instead.
        var matches = Regex.Matches(
            table,
            @"static const [^\n]* kExcDesc(\d+) = \{.*?(\};|\},)[ \t\r]*$",
            RegexOptions.Multiline | RegexOptions.Singleline);

        Assert.True(matches.Count > 0, "expected at least one kExcDesc declaration:\n" + table);

        foreach (Match m in matches)
        {
            Assert.True(m.Groups[2].Value == "};",
                $"kExcDesc{m.Groups[1].Value} closes its initializer with "
                + $"'{m.Groups[2].Value}' but each kExcDesc is a standalone declaration "
                + "and must end with '};'. A trailing ',' makes MSVC read the next "
                + "kExcDesc as a continued declarator (C2226/C2143/C2447).\n" + table);
        }
    }

    /// <summary>
    /// Pins the exact production symptom: an initializer closed with <c>},</c>
    /// directly followed by another declaration (here the <c>extern "C"</c> table,
    /// or the next <c>kExcDesc</c> when there are several).
    /// </summary>
    [Fact]
    public void NoDeclarationInitializer_EndsWithBareComma()
    {
        var table = EmitExceptionTable();

        var bad = Regex.Match(
            table,
            @"(\}[ \t\r]*,[ \t\r]*\r?\n)[ \t]*(static const|extern)",
            RegexOptions.Multiline);

        Assert.False(bad.Success,
            "found an initializer closed with '},' followed by another declaration — "
            + "the '},' must be '};':\n"
            + table.Substring(System.Math.Max(0, bad.Index - 200),
                              System.Math.Min(400, table.Length - System.Math.Max(0, bad.Index - 200))));
    }

    /// <summary>
    /// The exported lookup table must reference exactly the descriptors that were
    /// emitted — no dangling <c>&amp;kExcDescN</c> for an N that was never declared,
    /// and no declared descriptor left out.
    /// </summary>
    [Fact]
    public void ExportedTable_ReferencesEveryDeclaredDescriptor()
    {
        var table = EmitExceptionTable();

        var declared = Regex.Matches(table, @"kExcDesc(\d+) = \{", RegexOptions.Multiline)
            .Select(m => m.Groups[1].Value)
            .ToHashSet(StringComparer.Ordinal);

        var referenced = Regex.Matches(table, @"&kExcDesc(\d+)", RegexOptions.Multiline)
            .Select(m => m.Groups[1].Value)
            .ToHashSet(StringComparer.Ordinal);

        Assert.True(declared.Count > 0, "expected declarations:\n" + table);
        Assert.Equal(declared.OrderBy(x => int.Parse(x)),
                     referenced.OrderBy(x => int.Parse(x)));
    }
}
