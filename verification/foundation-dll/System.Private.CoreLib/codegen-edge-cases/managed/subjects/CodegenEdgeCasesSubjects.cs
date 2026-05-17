using System;

public static partial class CodegenEdgeCasesSubjects
{
    public static int _exitCode;

    // Subject 0: Multi-latch loop — 2+ continue statements create multiple latch blocks
    public static void Subject_0()
    {
        int sum = 0;
        for (int i = 0; i < 10; i++)
        {
            if (i % 2 == 0) continue;
            if (i > 5) continue;
            sum += i;
        }
        if (sum != 9) _exitCode = 1;
    }

    // Subject 1: Switch-case fallthrough — empty case falls through to next
    public static void Subject_1(int x)
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
        int expected = (x == 0 || x == 1) ? 10 : (x == 2 ? 20 : 0);
        if (result != expected) _exitCode = 1;
    }

    // Subject 2: Deep EH nesting — try->finally->try->catch inside try->catch->try->finally
    public static void Subject_2()
    {
        try
        {
            // outer try
            try
            {
                // middle try
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

    // Subject 3: Finally control flow — finally with conditional (lock expansion pattern)
    public static int Subject_3(int x)
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

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(0); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(5); break;
        }
    }
}
