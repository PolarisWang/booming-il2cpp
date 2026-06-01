// Handwritten custom entry stubs for pre-verification audit.
// Implements CustomEntrySubject_N entries matching the capability-family-contract.

using System;
using System.Runtime.CompilerServices;
using Chaos.TestFramework;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // [14] System.Private.CoreLib/OSR::HotLoop:System.Void()
    // NOTE: no nested braces in method body — the audit regex uses lazy matching.
    public static void CustomEntrySubject_14()
    {
        long sum = 0;
        for (int i = 0; i < 150; i++)
            sum += i;
        Assert.AreEqual(11175L, sum);
    }

    // [15] System.Private.CoreLib/Memory::CopyBlock:System.Void()
    // Cpblk IL opcode — Unsafe.CopyBlock produces cpblk
    public static void CustomEntrySubject_15()
    {
        byte[] src = new byte[] { 1, 2, 3, 4 };
        byte[] dst = new byte[4];
        Unsafe.CopyBlock(ref dst[0], ref src[0], 4);
        Assert.IsTrue(dst[0] == 1 && dst[1] == 2 && dst[2] == 3 && dst[3] == 4);
    }

    // [16] System.Private.CoreLib/Memory::InitBlock:System.Void()
    // InitBlk IL opcode — Unsafe.InitBlock produces initblk
    public static void CustomEntrySubject_16()
    {
        byte[] buf = new byte[4];
        Unsafe.InitBlock(ref buf[0], 0xAB, 4);
        Assert.IsTrue(buf[0] == 0xAB && buf[1] == 0xAB && buf[2] == 0xAB && buf[3] == 0xAB);
    }
}
