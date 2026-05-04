using System;
using System.Diagnostics;

// Auto-generated benchmark harness for GlobalizationCultureTests
// Runs 9 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo
        var sw0 = Stopwatch.StartNew();
        _ = CultureInfo.CurrentCulture;
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo
        var sw1 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture;
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String
        var sw2 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.Name;
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String
        var sw3 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.DisplayName;
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo
        var sw4 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.DateTimeFormat;
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 5: Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo
        var sw5 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.NumberFormat;
        sw5.Stop();
        Console.WriteLine($"Entry 5: {sw5.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 6: Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String
        var sw6 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello");
        sw6.Stop();
        Console.WriteLine($"Entry 6: {sw6.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 7: Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz
        var sw7 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None);
        sw7.Stop();
        Console.WriteLine($"Entry 7: {sw7.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 8: Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String
        var sw8 = Stopwatch.StartNew();
        _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello");
        sw8.Stop();
        Console.WriteLine($"Entry 8: {sw8.Elapsed.TotalMilliseconds:F3} ms");
    }
}
