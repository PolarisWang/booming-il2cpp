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

    private sealed class EchoHolder
    {
        private readonly string _value;

        public EchoHolder(string value)
        {
            _value = value;
        }

        public string GetValue()
        {
            return _value;
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

    private static string EchoValue(string value)
    {
        return value;
    }

    private static int ComposeEcho()
    {
        Console.WriteLine(new EchoHolder(EchoValue("corelib-reference-echo:System.Private.CoreLib|System.Console")).GetValue());
        return 0;
    }

    private static int ComposeLiteral()
    {
        Console.WriteLine("corelib-reference-literal:System.Console");
        return 0;
    }

    private static string BuildLiteralMessage()
    {
        return "corelib-reference-static-message:System.Private.CoreLib|System.Console";
    }

    private static int ComposeStaticMessage()
    {
        Console.WriteLine(BuildLiteralMessage());
        return 0;
    }

    private static int ComposeForwardedLiteral()
    {
        Console.WriteLine(EchoValue("corelib-reference-forwarded:System.Private.CoreLib|System.Console"));
        return 0;
    }

    private static int ComposeForwardedStaticMessage()
    {
        Console.WriteLine(EchoValue(BuildLiteralMessage()));
        return 0;
    }

    private static int ComposeProducedEcho()
    {
        Console.WriteLine(new EchoHolder(BuildLiteralMessage()).GetValue());
        return 0;
    }
}
