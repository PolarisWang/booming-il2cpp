using System.Reflection;
using Chaos.TestFramework;

namespace Chaos.TestFramework.Runtime;

internal static class FactRunner
{
    public static void Run(Assembly assembly)
    {
        int passed = 0, failed = 0;

        foreach (var type in assembly.GetTypes())
        {
            // Create instance for instance methods (if needed)
            object? instance = null;

            foreach (var method in type.GetMethods(
                BindingFlags.Static | BindingFlags.Instance |
                BindingFlags.Public | BindingFlags.NonPublic))
            {
                if (method.GetCustomAttribute<FactAttribute>() is null)
                    continue;
                if (method.GetParameters().Length != 0)
                    continue;

                // For instance methods, create an instance once per type
                if (!method.IsStatic)
                {
                    if (instance is null)
                    {
                        try
                        {
                            instance = Activator.CreateInstance(type);
                        }
                        catch (Exception ex)
                        {
                            Console.Error.WriteLine($"FAIL [{type.Name}]: cannot create instance: {ex.Message}");
                            failed++;
                            continue;
                        }
                    }
                }

                Assert.Reset();
                try
                {
                    method.Invoke(instance, null);
                    if (Assert.ExitCode == 0)
                        passed++;
                    else
                    {
                        Console.Error.WriteLine($"FAIL [{type.Name}.{method.Name}]: assertion failed");
                        failed++;
                    }
                }
                catch (Exception ex)
                {
                    var inner = ex.InnerException?.Message ?? ex.Message;
                    Console.Error.WriteLine($"FAIL [{type.Name}.{method.Name}]: {inner}");
                    failed++;
                }
            }
        }

        Console.WriteLine($"Passed: {passed}/{passed + failed}");
    }
}
