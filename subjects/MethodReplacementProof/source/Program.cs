namespace MethodReplacementProof;

internal static class Program
{
    private static int Main(string[] args)
    {
        var slot = new ReplacementSlot(static () => "hello from AOT");

        Console.WriteLine($"method-replacement-before={slot.Invoke()}");
        slot.Register(static () => "hello from interpreter");
        Console.WriteLine($"method-replacement-after={slot.Invoke()}");
        slot.Revert();
        Console.WriteLine($"method-replacement-reverted={slot.Invoke()}");
        return 0;
    }

    private sealed class ReplacementSlot(Func<string> aotTarget)
    {
        private readonly Func<string> _aotTarget = aotTarget;
        private Func<string>? _replacementTarget;

        public void Register(Func<string> replacementTarget)
        {
            _replacementTarget = replacementTarget;
        }

        public void Revert()
        {
            _replacementTarget = null;
        }

        public string Invoke()
        {
            return (_replacementTarget ?? _aotTarget)();
        }
    }
}
