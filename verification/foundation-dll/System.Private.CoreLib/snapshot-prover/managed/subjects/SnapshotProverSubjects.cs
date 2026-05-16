// Auto-generated native-AOT entry point
// Family: family/SnapshotTestFixtures/snapshot/prover
// Assembly: SnapshotProverSubjects
// Variant: subjects
//
// Runtime verification subjects that test the chaos-il2cpp codegen produces
// correct native code for basic C# patterns. Limited to arithmetic, comparisons,
// and primitive conversions — patterns that work without the full runtime
// (GC, vtable, generics, boxing). These are covered by snapshot tests instead.
//
// Expected values derived from Driver.cs method implementations.

public static partial class SnapshotProverSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] Arithmetic add: 3 + 4 = 7
    public static void Subject_0()
    {
        int a = 3;
        int b = 4;
        int result = a + b;
        if (result != 7) _exitCode = 1;
    }

    // [1] Trivial void return: ldc.i4 0, ret
    public static void Subject_1()
    {
        int result = 0;
        if (result != 0) _exitCode = 1;
    }

    // [2] Arithmetic multiply: 5 * 5 = 25
    public static void Subject_2()
    {
        int x = 5;
        int result = x * x;
        if (result != 25) _exitCode = 1;
    }

    // [3] Branch condition: 5 > 0 ? 1 : 0 = 1
    public static void Subject_3()
    {
        int val = 5;
        bool isPositive = val > 0;
        int result = isPositive ? 1 : 0;
        if (result != 1) _exitCode = 1;
    }

    // [4] Arithmetic subtraction: 100 - 58 = 42
    public static void Subject_4()
    {
        int a = 100;
        int b = 58;
        int result = a - b;
        if (result != 42) _exitCode = 1;
    }

    // [5] Arithmetic division: 84 / 2 = 42
    public static void Subject_5()
    {
        int a = 84;
        int b = 2;
        int result = a / b;
        if (result != 42) _exitCode = 1;
    }

    // [6] Long-to-int conversion: (int)100L = 100
    public static void Subject_6()
    {
        long a = 100L;
        int result = (int)a;
        if (result != 100) _exitCode = 1;
    }

    // [7] Loop sum to test flat-goto codegen: 0+1+2+3+4 = 10
    public static void Subject_7()
    {
        int s = 0;
        for (int i = 0; i < 5; i++)
            s += i;
        if (s != 10) _exitCode = 1;
    }

    // [8] Trivial return: ldc.i4 0, ret
    public static void Subject_8()
    {
        int result = 0;
        if (result != 0) _exitCode = 1;
    }

    // [9] Comparison chain: (10 > 5 && 3 < 8) ? 1 : 0 = 1
    public static void Subject_9()
    {
        bool c1 = 10 > 5;
        bool c2 = 3 < 8;
        int result = (c1 && c2) ? 1 : 0;
        if (result != 1) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
        }
    }
}
