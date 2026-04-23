using System;
using System.Collections.Generic;
using System.Linq;

namespace GoldenCoreLibReference.App;

internal static class Program
{
    private static int Main()
    {
        var values = new List<int> { 3, 5, 8 };
        var sum = values.Sum();
        Console.WriteLine($"corelib:{sum}:{values.Count}");
        return sum == 16 && values.Count == 3 ? 0 : 1;
    }
}
