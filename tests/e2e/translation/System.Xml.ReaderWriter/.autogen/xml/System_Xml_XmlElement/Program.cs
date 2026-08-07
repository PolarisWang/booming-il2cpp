// Auto-generated test runner
using System.Reflection;
using Chaos.TestFramework;

var testType = typeof(Program).Assembly.GetTypes().FirstOrDefault(t => t.Name.EndsWith("Tests"));
if (testType is null) { Console.Error.WriteLine("[FATAL] Test class not found"); return 1; }

object? instance = null;
if (!testType.IsAbstract && testType.GetConstructors().Any(c => c.GetParameters().Length == 0))
    instance = Activator.CreateInstance(testType);

int passed = 0, failed = 0;
foreach (var method in testType.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static | BindingFlags.Instance))
{
    if (method.GetCustomAttribute<FactAttribute>() is null) continue;
    if (method.GetParameters().Length != 0) continue;
    try
    {
        method.Invoke(method.IsStatic ? null : instance, null);
        Console.WriteLine($"[PASS] {testType.Name}.{method.Name}");
        passed++;
    }
    catch (TargetInvocationException ex) when (ex.InnerException is not null)
    {
        var msg = ex.InnerException.Message;
        Console.WriteLine($"[FAIL] {testType.Name}.{method.Name}: {msg}");
        failed++;
    }
    catch (Exception ex)
    {
        Console.WriteLine($"[FAIL] {testType.Name}.{method.Name}: {ex.Message}");
        failed++;
    }
}

Console.WriteLine($"Summary: {passed} passed, {failed} failed");
return failed > 0 ? 1 : 0;
