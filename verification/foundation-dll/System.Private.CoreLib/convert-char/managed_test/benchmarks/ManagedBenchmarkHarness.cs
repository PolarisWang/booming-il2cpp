// Managed benchmark harness — calls real Convert.ToChar() methods
using System;
using System.Diagnostics;

const int kWarmupIterations = 100;
const int kMeasureIterations = 100000;

var methodSubjects = new (string Name, Func<char>? Body, bool Throws)[]
{
    ("Convert.ToChar(bool)",              null, true),  // throws InvalidCastException
    ("Convert.ToChar(byte)",              () => Convert.ToChar((byte)42), false),
    ("Convert.ToChar(char)",              () => Convert.ToChar('A'), false),
    ("Convert.ToChar(DateTime)",           null, true),  // throws InvalidCastException
    ("Convert.ToChar(decimal)",           null, true),  // throws InvalidCastException
    ("Convert.ToChar(double)",            null, true),  // throws InvalidCastException
    ("Convert.ToChar(short)",             () => Convert.ToChar((short)42), false),
    ("Convert.ToChar(int)",               () => Convert.ToChar(42), false),
    ("Convert.ToChar(long)",              () => Convert.ToChar(42L), false),
    ("Convert.ToChar(object)",            null, true),  // boxed int doesn't implement IConvertible.ToChar
    ("Convert.ToChar(object,IFormatProvider)", null, true),
    ("Convert.ToChar(sbyte)",             () => Convert.ToChar((sbyte)42), false),
    ("Convert.ToChar(float)",             null, true),  // throws InvalidCastException
    ("Convert.ToChar(string)",            () => Convert.ToChar("A"), false),
    ("Convert.ToChar(string,IFormatProvider)", () => Convert.ToChar("A", null), false),
    ("Convert.ToChar(ushort)",            () => Convert.ToChar((ushort)42), false),
    ("Convert.ToChar(uint)",              () => Convert.ToChar(42u), false),
    ("Convert.ToChar(ulong)",             () => Convert.ToChar(42UL), false),
};

Console.WriteLine("  \"family\": \"convert-char\",");
Console.WriteLine("  \"warmupIterations\": " + kWarmupIterations + ",");
Console.WriteLine("  \"measureIterations\": " + kMeasureIterations + ",");
Console.WriteLine("  \"results\": [");

for (int i = 0; i < methodSubjects.Length; i++)
{
    var (name, body, throws) = methodSubjects[i];

    if (body == null)
    {
        string sep = (i < methodSubjects.Length - 1) ? "," : "";
        Console.WriteLine("    {");
        Console.WriteLine("      \"methodIndex\": " + i + ",");
        Console.WriteLine("      \"name\": \"" + name + "\",");
        Console.WriteLine("      \"elapsedMilliseconds\": 0,");
        Console.WriteLine("      \"opsPerSecond\": 0,");
        Console.WriteLine("      \"iterations\": 0,");
        Console.WriteLine("      \"skipped\": true");
        Console.WriteLine("    }" + sep);
        continue;
    }

    // Warmup
    for (int w = 0; w < kWarmupIterations; w++)
    {
        body();
    }

    // Force GC
    GC.Collect();
    GC.WaitForPendingFinalizers();

    // Measurement
    var sw = Stopwatch.StartNew();
    for (int m = 0; m < kMeasureIterations; m++)
    {
        _ = body();
    }
    sw.Stop();

    double elapsedMs = sw.Elapsed.TotalMilliseconds;
    double opsPerSecond = elapsedMs > 0 ? kMeasureIterations / (elapsedMs / 1000.0) : 0;

    string sep2 = (i < methodSubjects.Length - 1) ? "," : "";
    Console.WriteLine("    {");
    Console.WriteLine("      \"methodIndex\": " + i + ",");
    Console.WriteLine("      \"name\": \"" + name + "\",");
    Console.WriteLine("      \"elapsedMilliseconds\": " + elapsedMs.ToString("F4") + ",");
    Console.WriteLine("      \"opsPerSecond\": " + opsPerSecond.ToString("F0") + ",");
    Console.WriteLine("      \"iterations\": " + kMeasureIterations);
    Console.WriteLine("    }" + sep2);
}

Console.WriteLine("  ]");
Console.WriteLine("}");