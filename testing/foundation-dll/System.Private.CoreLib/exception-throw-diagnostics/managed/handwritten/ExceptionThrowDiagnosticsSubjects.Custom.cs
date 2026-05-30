// Auto-generated native-AOT entry point (handwritten custom entry)
// Family: family/System.Private.CoreLib/exception/throw-diagnostics
// Assembly: System.Private.CoreLib
// Variant: subjects — custom entries for Subject_14 (OSR), Subject_15 (Cpblk), Subject_16 (InitBlk)

using System;
using System.Runtime.CompilerServices;
using Chaos.TestFramework;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // [14] OSR hot loop — 150 iterations to trigger RegisterExecute -> T4 promotion
    [Fact]
    public static void Subject_14()
    {
        long sum = 0;
        for (int i = 0; i < 150; i++) {
            sum += i;
        }
        Assert.AreEqual(11175L, sum);
    }

    // [15] Cpblk — Unsafe.CopyBlock produces cpblk IL
    [Fact]
    public static void Subject_15()
    {
        byte[] src = new byte[] { 1, 2, 3, 4 };
        byte[] dst = new byte[4];
        Unsafe.CopyBlock(ref dst[0], ref src[0], 4);
        Assert.IsTrue(dst[0] == 1 && dst[1] == 2 && dst[2] == 3 && dst[3] == 4);
    }

    // [16] InitBlk — Unsafe.InitBlock produces initblk IL
    [Fact]
    public static void Subject_16()
    {
        byte[] buf = new byte[4];
        Unsafe.InitBlock(ref buf[0], 0xAB, 4);
        Assert.IsTrue(buf[0] == 0xAB && buf[1] == 0xAB && buf[2] == 0xAB && buf[3] == 0xAB);
    }
}
