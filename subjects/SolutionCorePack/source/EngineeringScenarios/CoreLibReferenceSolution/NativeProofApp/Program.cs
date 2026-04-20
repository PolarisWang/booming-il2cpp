using System;

namespace GoldenCoreLibReference.NativeProofApp;

internal static class Program
{
    private sealed class Holder
    {
        private readonly string _value;

        public Holder(string value)
        {
            _value = value;
        }

        public string Render()
        {
            return string.Concat(
                string.Concat("corelib-reference-native:", _value),
                "|System.Runtime|System.Console");
        }
    }

    private static int Main()
    {
        Console.WriteLine(new Holder("System.Private.CoreLib").Render());
        return 0;
    }
}
