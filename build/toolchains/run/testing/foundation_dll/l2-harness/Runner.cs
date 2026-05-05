using System.Reflection;
using System.Text.Json;

var dllPath = args.Length > 0 ? args[0] : null;
var className = args.Length > 1 ? args[1] : null;
var methodCount = args.Length > 2 ? int.Parse(args[2]) : 0;

if (dllPath == null || className == null || methodCount == 0)
{
    Console.Error.WriteLine("Usage: L2Harness <dll-path> <class-name> <method-count>");
    Environment.Exit(1);
    return;
}

var asm = Assembly.LoadFrom(dllPath);
var type = asm.GetType(className);
if (type == null)
{
    Console.Error.WriteLine($"Class not found: {className}");
    Environment.Exit(1);
    return;
}

var runMethod = type.GetMethod("Run", BindingFlags.Public | BindingFlags.Static);
if (runMethod == null)
{
    Console.Error.WriteLine($"Static Run(int) method not found on {className}");
    Environment.Exit(1);
    return;
}

var results = new int[methodCount];
for (int i = 0; i < methodCount; i++)
{
    try
    {
        results[i] = (int)runMethod.Invoke(null, new object[] { i });
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Method {i} failed: {ex.InnerException?.Message ?? ex.Message}");
        results[i] = -1;
    }
}

Console.WriteLine(JsonSerializer.Serialize(results));
