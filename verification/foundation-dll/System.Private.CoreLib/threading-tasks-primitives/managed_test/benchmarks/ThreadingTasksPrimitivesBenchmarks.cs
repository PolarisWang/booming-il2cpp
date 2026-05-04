using System;
using System.Diagnostics;

// Auto-generated benchmark harness for ThreadingTasksPrimitivesTests
// Runs 5 methods as quick smoke-benchmark.
class Program
{
    static void Main()
    {
        // Benchmark entry 0: Benchmark_System_Private_CoreLib_System_Threading_Tasks_Task_Delay_System_Threading_Tasks_Task_System_Int32
        var sw0 = Stopwatch.StartNew();
        _ = Task.Delay(42);
        sw0.Stop();
        Console.WriteLine($"Entry 0: {sw0.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 1: Benchmark_System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Void
        var sw1 = Stopwatch.StartNew();
        _ = Task.CompletedTask.Wait();
        sw1.Stop();
        Console.WriteLine($"Entry 1: {sw1.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 2: Benchmark_System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Boolean_System_Int32
        var sw2 = Stopwatch.StartNew();
        _ = Task.CompletedTask.Wait(42);
        sw2.Stop();
        Console.WriteLine($"Entry 2: {sw2.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 3: Benchmark_System_Private_CoreLib_System_Threading_Tasks_Task_get_IsCompleted_System_Boolean
        var sw3 = Stopwatch.StartNew();
        _ = Task.CompletedTask.IsCompleted;
        sw3.Stop();
        Console.WriteLine($"Entry 3: {sw3.Elapsed.TotalMilliseconds:F3} ms");
        // Benchmark entry 4: Benchmark_System_Private_CoreLib_System_Threading_Tasks_Task_get_Status_System_Threading_Tasks_TaskStatus
        var sw4 = Stopwatch.StartNew();
        _ = Task.CompletedTask.Status;
        sw4.Stop();
        Console.WriteLine($"Entry 4: {sw4.Elapsed.TotalMilliseconds:F3} ms");
    }
}
