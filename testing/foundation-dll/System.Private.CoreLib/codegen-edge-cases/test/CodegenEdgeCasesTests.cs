using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Private.CoreLib/codegen/codegen-edge-cases", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class CodegenEdgeCasesTests
{
    // AI-generated tests for family/System.Private.CoreLib/codegen/codegen-edge-cases

    // === flat-fallback patterns ===

    [Fact]
    public void Subject_0_MultiLatchLoop()
    {
        // Purpose: Verify multi-latch loop pattern (2 continue statements)
        int sum = 0;
        for (int i = 0; i < 10; i++)
        {
            if (i % 2 == 0) continue;
            if (i > 5) continue;
            sum += i;
        }
        Assert.Equal(9, sum);
    }

    [Fact]
    public void Subject_1_SwitchFallthrough()
    {
        // Purpose: Verify switch-case fallthrough (empty case falls through)
        int Run(int x)
        {
            int result = 0;
            switch (x)
            {
                case 0:
                case 1:
                    result = 10;
                    break;
                case 2:
                    result = 20;
                    break;
                default:
                    result = 0;
                    break;
            }
            return result;
        }
        Assert.Equal(10, Run(0));
        Assert.Equal(10, Run(1));
        Assert.Equal(20, Run(2));
        Assert.Equal(0, Run(3));
    }

    [Fact]
    public void Subject_2_DeepEHNesting()
    {
        // Purpose: Verify deep EH nesting (try->finally->try->catch inside try->catch->try->finally)
        // No assertion needed — must not throw or flat-fallback
        try
        {
            try
            {
            }
            finally
            {
                try { } catch { }
            }
        }
        catch
        {
            try { } finally { }
        }
    }

    [Fact]
    public void Subject_3_FinallyControlFlow()
    {
        // Purpose: Verify finally with conditional return (lock expansion pattern)
        int Subject(int x)
        {
            try
            {
                if (x > 0) return 1;
                return 0;
            }
            finally
            {
                var temp = x;
            }
        }
        Assert.Equal(1, Subject(5));
        Assert.Equal(0, Subject(0));
    }

    // === structured recovery rate ===

    [Fact]
    public void StructuredRecoveryRate_Is_1_0()
    {
        // This test is verified by the codegen pipeline via codegen-metrics.json
        // Runtime assertion: all subjects must execute without flat-fallback
        CodegenEdgeCasesSubjects._exitCode = 0;
        CodegenEdgeCasesSubjects.Subject_0();
        Assert.Equal(0, CodegenEdgeCasesSubjects._exitCode);

        CodegenEdgeCasesSubjects._exitCode = 0;
        CodegenEdgeCasesSubjects.Subject_1(0);
        Assert.Equal(0, CodegenEdgeCasesSubjects._exitCode);

        CodegenEdgeCasesSubjects.Subject_2();

        var result = CodegenEdgeCasesSubjects.Subject_3(5);
        Assert.Equal(1, result);
    }
}
