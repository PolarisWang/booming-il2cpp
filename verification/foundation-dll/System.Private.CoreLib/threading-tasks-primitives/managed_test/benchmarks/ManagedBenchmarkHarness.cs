// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/threading/tasks-primitives
using System;
using System.Diagnostics;
using Chaos.Benchmark.threading_tasks_primitives;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_System_Action),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_1_System_Func_1),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_Delay_System_Threading_Tasks_Task_System_Int32),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Void),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Boolean_System_Int32),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_get_IsCompleted_System_Boolean),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_get_Status_System_Threading_Tasks_TaskStatus),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_ContinueWith_System_Threading_Tasks_Task_System_Action_System_Threading_Tasks_Task),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_WhenAll_System_Threading_Tasks_Task_System_Threading_Tasks_Task),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_WhenAny_System_Threading_Tasks_Task_System_Threading_Tasks_Task_System_Threading_Tasks_Task),
    ("System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Tasks_Task_FromResult_System_Threading_Tasks_Task_1_TResult),
    ("System.Private.CoreLib/System.Threading.Thread::Start:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Thread_Start_System_Void),
    ("System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Thread_Sleep_System_Void_System_Int32),
    ("System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Thread_get_CurrentThread_System_Threading_Thread),
    ("System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Thread_get_ManagedThreadId_System_Int32)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/threading/tasks-primitives\",");
Console.WriteLine("  \"warmupIterations\": " + kWarmupIterations + ",");
Console.WriteLine("  \"measureIterations\": " + kMeasureIterations + ",");
Console.WriteLine("  \"results\": [");

for (int i = 0; i < methodSubjects.Length; i++)
{
    var (subjectId, body) = methodSubjects[i];

    // Warmup
    for (int w = 0; w < kWarmupIterations; w++)
    {
        body();
    }

    // Measurement
    var sw = Stopwatch.StartNew();
    for (int m = 0; m < kMeasureIterations; m++)
    {
        body();
    }
    sw.Stop();

    double elapsedMs = sw.Elapsed.TotalMilliseconds;
    double opsPerSecond = kMeasureIterations / (elapsedMs / 1000.0);

    string comma = (i < methodSubjects.Length - 1) ? "," : "";
    Console.WriteLine("    {");
    Console.WriteLine("      \"methodIndex\": " + i + ",");
    Console.WriteLine("      \"methodSubjectId\": \"" + subjectId.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\",");
    Console.WriteLine("      \"elapsedMilliseconds\": " + elapsedMs.ToString("F6") + ",");
    Console.WriteLine("      \"opsPerSecond\": " + opsPerSecond.ToString("F6") + ",");
    Console.WriteLine("      \"iterations\": " + kMeasureIterations);
    Console.WriteLine("    }" + comma);
}

Console.WriteLine("  ]");
Console.WriteLine("}");