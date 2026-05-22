// Auto-generated native-AOT entry point (handwritten custom entry)
// Family: family/System.Private.CoreLib/exception/throw-diagnostics
// Assembly: System.Private.CoreLib
// Variant: subjects — custom entries for Subject_14 (OSR), Subject_15 (Cpblk), Subject_16 (InitBlk)

using System;
using System.Runtime.CompilerServices;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // [14] OSR hot loop — 150 iterations to trigger RegisterExecute -> T4 promotion
    public static void Subject_14()
    {
        try {
            long sum = 0;
            for (int i = 0; i < 150; i++) {
                sum += i;
            }
            if (sum != 11175) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }

    // [15] Cpblk — Unsafe.CopyBlock produces cpblk IL
    public static void Subject_15()
    {
        try {
            byte[] src = new byte[] { 1, 2, 3, 4 };
            byte[] dst = new byte[4];
            Unsafe.CopyBlock(ref dst[0], ref src[0], 4);
            if (dst[0] != 1 || dst[1] != 2 || dst[2] != 3 || dst[3] != 4) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }

    // [16] InitBlk — Unsafe.InitBlock produces initblk IL
    public static void Subject_16()
    {
        try {
            byte[] buf = new byte[4];
            Unsafe.InitBlock(ref buf[0], 0xAB, 4);
            if (buf[0] != 0xAB || buf[1] != 0xAB || buf[2] != 0xAB || buf[3] != 0xAB) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }
}
