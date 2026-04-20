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

    private sealed class AuxiliaryHolder
    {
        private readonly string _value;

        public AuxiliaryHolder(string value)
        {
            _value = value;
        }

        public string Render()
        {
            return string.Concat(
                string.Concat("corelib-reference-aux:", _value),
                "|System.Console");
        }
    }

    private static int Main()
    {
        return PrintAndExit();
    }

    private static int PrintAndExit()
    {
        return ComposeAndPrint();
    }

    private static int ComposeAndPrint()
    {
        Console.WriteLine(new Holder("System.Private.CoreLib").Render());
        return 0;
    }

    private static int ComposeAuxiliary()
    {
        Console.WriteLine(new AuxiliaryHolder("System.Runtime").Render());
        return 0;
    }
}
