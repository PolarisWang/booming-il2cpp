using System;

namespace StaticCallCtorGetter.App;

internal static class Program
{
    private static string Echo(string value)
    {
        return value;
    }

    private sealed class Holder
    {
        private readonly string _value;

        public Holder(string value)
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
        Console.WriteLine(new Holder(Echo("static-call-ctor-getter:System.Private.CoreLib|System.Console")).GetValue());
        return 0;
    }
}
