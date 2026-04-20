using System;

namespace ConstructorThenInstanceCall.App;

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
                string.Concat("constructor-then-instance-call:", _value),
                "|System.Private.CoreLib");
        }
    }

    private static int Main()
    {
        Console.WriteLine(new Holder("System.Console").Render());
        return 0;
    }
}
