using System;

namespace ChaosHelloWorld;

internal static class Program
{
    static int Main(string[] args)
    {
        Console.WriteLine("Hello from chaos-il2cpp!");
        Console.WriteLine($"args.Length={args.Length}");

        // Async state machine compilation (async Task Main) is supported
        // by the toolchain but a codegen emit issue (extern "C" declarations
        // with {; instead of ; in native-aot.generated.header.h) currently
        // blocks it via the publish path.  Pure-sync apps work end-to-end.

        return 0;
    }
}