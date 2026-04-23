using System;
using System.Threading.Tasks;

namespace GoldenCoreLibReference.NativeProofApp;

internal interface IMessageBanner
{
    string Render();
}

internal delegate string CoreLibMessageFormatter(string prefix);

internal delegate string CoreLibTailFormatter(string value);

internal sealed class InterfaceDispatchBanner : IMessageBanner
{
    private readonly string _value;

    public InterfaceDispatchBanner(string value)
    {
        _value = value;
    }

    public string Render()
    {
        return string.Concat(
            string.Concat("interface-dispatch-message:", _value),
            "|System.Runtime");
    }
}

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

    private sealed class DelegateRelayBanner
    {
        private readonly string _name;

        public DelegateRelayBanner(string name)
        {
            _name = name;
        }

        public string BuildMessage(string prefix)
        {
            return string.Concat(prefix, _name, ".");
        }
    }

    private static class DelegateRelayTail
    {
        public static string AppendSuffix(string value)
        {
            return string.Concat(value, "|System.Console");
        }
    }

    private static class BoxingSink
    {
        public static void Consume(object value)
        {
            _ = value;
        }
    }

    private sealed class ArrayBanner
    {
        private readonly string _name;

        public ArrayBanner(string name)
        {
            _name = name;
        }

        public string BuildMessage()
        {
            return string.Concat(
                string.Concat("Array boxing native proof: ", _name),
                ".");
        }
    }

    private sealed class ArrayCopyBanner
    {
        private readonly string _name;

        public ArrayCopyBanner(string name)
        {
            _name = name;
        }

        public string BuildMessage()
        {
            return string.Concat(
                string.Concat("Array copy native proof: ", _name),
                ".");
        }
    }

    private sealed class ArrayReverseBanner
    {
        private readonly string _name;

        public ArrayReverseBanner(string name)
        {
            _name = name;
        }

        public string BuildMessage()
        {
            return string.Concat(
                string.Concat("Array reverse native proof: ", _name),
                ".");
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

    private static int ComposeInterfaceDispatchMessage()
    {
        IMessageBanner banner = new InterfaceDispatchBanner("System.Private.CoreLib");
        Console.WriteLine(banner.Render());
        return 0;
    }

    private static int ComposeDelegateClosedTargetRelayMessage()
    {
        var banner = new DelegateRelayBanner("delegate packet");
        CoreLibMessageFormatter formatter = banner.BuildMessage;
        CoreLibTailFormatter tail = DelegateRelayTail.AppendSuffix;
        Console.WriteLine(tail(formatter("corelib-reference-delegate:System.Private.CoreLib|System.Runtime:")));
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

    private static int ComposeProducedRender()
    {
        Console.WriteLine(new Holder(BuildLiteralMessage()).Render());
        return 0;
    }

    private static int ComposeProducedForwardedRender()
    {
        Console.WriteLine(EchoValue(new Holder(BuildLiteralMessage()).Render()));
        return 0;
    }

    private static int ComposeProducedForwardedEcho()
    {
        Console.WriteLine(EchoValue(new EchoHolder(BuildLiteralMessage()).GetValue()));
        return 0;
    }

    private static int ComposeForwardedProducedEcho()
    {
        Console.WriteLine(new EchoHolder(EchoValue(BuildLiteralMessage())).GetValue());
        return 0;
    }

    private static int ComposeForwardedProducedRender()
    {
        Console.WriteLine(new Holder(EchoValue(BuildLiteralMessage())).Render());
        return 0;
    }

    private static int ComposeForwardedProducedForwardedRender()
    {
        Console.WriteLine(EchoValue(new Holder(EchoValue(BuildLiteralMessage())).Render()));
        return 0;
    }

    private static int ComposeForwardedProducedForwardedEcho()
    {
        Console.WriteLine(EchoValue(new EchoHolder(EchoValue(BuildLiteralMessage())).GetValue()));
        return 0;
    }

    private static int ComposeProducedForwardedForwardedRender()
    {
        Console.WriteLine(EchoValue(EchoValue(new Holder(BuildLiteralMessage()).Render())));
        return 0;
    }

    private static int ComposeProducedForwardedForwardedEcho()
    {
        Console.WriteLine(EchoValue(EchoValue(new EchoHolder(BuildLiteralMessage()).GetValue())));
        return 0;
    }

    private static int ComposeForwardedProducedForwardedForwardedRender()
    {
        Console.WriteLine(EchoValue(EchoValue(new Holder(EchoValue(BuildLiteralMessage())).Render())));
        return 0;
    }

    private static int ComposeForwardedProducedForwardedForwardedEcho()
    {
        Console.WriteLine(EchoValue(EchoValue(new EchoHolder(EchoValue(BuildLiteralMessage())).GetValue())));
        return 0;
    }

    private static int ComposeProducedForwardedForwardedForwardedRender()
    {
        Console.WriteLine(EchoValue(EchoValue(EchoValue(new Holder(BuildLiteralMessage()).Render()))));
        return 0;
    }

    private static int ComposeProducedForwardedForwardedForwardedEcho()
    {
        Console.WriteLine(EchoValue(EchoValue(EchoValue(new EchoHolder(BuildLiteralMessage()).GetValue()))));
        return 0;
    }

    private static int ComposeForwardedProducedForwardedForwardedForwardedRender()
    {
        Console.WriteLine(EchoValue(EchoValue(EchoValue(new Holder(EchoValue(BuildLiteralMessage())).Render()))));
        return 0;
    }

    private static int ComposeForwardedProducedForwardedForwardedForwardedEcho()
    {
        Console.WriteLine(EchoValue(EchoValue(EchoValue(new EchoHolder(EchoValue(BuildLiteralMessage())).GetValue()))));
        return 0;
    }

    private static int ComposeArrayBoxingReferenceArray()
    {
        BoxingSink.Consume(42);
        var banners = new ArrayBanner[1];
        banners[0] = new ArrayBanner("boxed int 42");
        Console.WriteLine(banners[0].BuildMessage());
        return 0;
    }

    private static int ComposeArrayCopyReferenceArray()
    {
        var source = new ArrayCopyBanner[2];
        source[1] = new ArrayCopyBanner("copied banner");
        var target = new ArrayCopyBanner[2];
        Array.Copy(source, 0, target, 0, 2);
        Console.WriteLine(target[1].BuildMessage());
        return 0;
    }

    private static int ComposeArrayCopyLengthOnlyInt64ReferenceArray()
    {
        var source = new ArrayCopyBanner[2];
        source[1] = new ArrayCopyBanner("copied banner");
        var target = new ArrayCopyBanner[2];
        Array.Copy(source, target, 2L);
        Console.WriteLine(target[1].BuildMessage());
        return 0;
    }

    private static int ComposeArrayCopyInt64ReferenceArray()
    {
        var source = new ArrayCopyBanner[2];
        source[1] = new ArrayCopyBanner("copied banner");
        var target = new ArrayCopyBanner[2];
        Array.Copy(source, 0L, target, 0L, 2L);
        Console.WriteLine(target[1].BuildMessage());
        return 0;
    }

    private static int ComposeArrayConstrainedCopyReferenceArray()
    {
        var source = new ArrayCopyBanner[2];
        source[1] = new ArrayCopyBanner("copied banner");
        var target = new ArrayCopyBanner[2];
        Array.ConstrainedCopy(source, 0, target, 0, 2);
        Console.WriteLine(target[1].BuildMessage());
        return 0;
    }

    private static int ComposeArrayCopyToReferenceArray()
    {
        var source = new ArrayCopyBanner[2];
        source[1] = new ArrayCopyBanner("copied banner");
        var target = new ArrayCopyBanner[3];
        source.CopyTo(target, 1);
        Console.WriteLine(target[2].BuildMessage());
        return 0;
    }

    private static int ComposeArrayCopyToInt64ReferenceArray()
    {
        var source = new ArrayCopyBanner[2];
        source[1] = new ArrayCopyBanner("copied banner");
        var target = new ArrayCopyBanner[3];
        source.CopyTo(target, 1L);
        Console.WriteLine(target[2].BuildMessage());
        return 0;
    }

    private static int ComposeArrayReverseReferenceArray()
    {
        var banners = new ArrayReverseBanner[3];
        banners[1] = new ArrayReverseBanner("reversed banner");
        Array.Reverse(banners, 1, 2);
        Console.WriteLine(banners[2].BuildMessage());
        return 0;
    }

    private static int ComposeArrayReverseWholeReferenceArray()
    {
        var banners = new ArrayReverseBanner[3];
        banners[1] = new ArrayReverseBanner("reversed banner");
        Array.Reverse(banners);
        Console.WriteLine(banners[1].BuildMessage());
        return 0;
    }

    private static int ComposeArrayClearReferenceArray()
    {
        var messages = new string[3];
        messages[1] = "cleared banner";
        Array.Clear(messages, 1, 2);
        Console.WriteLine(string.Concat("Array clear native proof: ", messages[1]));
        return 0;
    }

    private static int ComposeArrayClearWholeReferenceArray()
    {
        var messages = new string[3];
        messages[1] = "cleared banner";
        Array.Clear(messages);
        Console.WriteLine(string.Concat("Array clear native proof: ", messages[1]));
        return 0;
    }

    private static async Task<int> AddAsync(int left, int right)
    {
        await Task.Yield();
        return left + right;
    }

    private static async Task<int> ComputeAsyncValue(int seed)
    {
        await Task.Yield();
        return await AddAsync(seed, 2);
    }

    private static async Task<bool> CheckAsyncFlag(bool left, bool right)
    {
        await Task.Yield();
        return left && right;
    }

    private static async Task<bool> ComputeAsyncFlag(bool seed)
    {
        await Task.Yield();
        return await CheckAsyncFlag(seed, false);
    }

    private static async Task<bool> RelayAsyncFlag(bool left, bool right)
    {
        await Task.Yield();
        return await CheckAsyncFlag(left, right);
    }

    private static async Task<bool> RelayAsyncComputedFlag(bool seed)
    {
        await Task.Yield();
        return await ComputeAsyncFlag(seed);
    }

    private static int ComposeAsyncValue()
    {
        return ComputeAsyncValue(40).GetAwaiter().GetResult();
    }

    private static int ComposeAsyncAddDirectValue()
    {
        return AddAsync(40, 2).GetAwaiter().GetResult();
    }

    private static int ComposeForwardedAsyncAddDirectValue()
    {
        return ComposeAsyncAddDirectValue();
    }

    private static bool ComposeAsyncFlagDirectValue()
    {
        return CheckAsyncFlag(true, false).GetAwaiter().GetResult();
    }

    private static bool ComposeForwardedAsyncFlagDirectValue()
    {
        return ComposeAsyncFlagDirectValue();
    }

    private static bool ComposeAsyncComputedFlagValue()
    {
        return ComputeAsyncFlag(true).GetAwaiter().GetResult();
    }

    private static bool ComposeRelayedAsyncFlagDirectValue()
    {
        return RelayAsyncFlag(true, false).GetAwaiter().GetResult();
    }

    private static bool ComposeForwardedRelayedAsyncFlagDirectValue()
    {
        return ComposeRelayedAsyncFlagDirectValue();
    }

    private static bool ComposeForwardedForwardedRelayedAsyncFlagDirectValue()
    {
        return ComposeForwardedRelayedAsyncFlagDirectValue();
    }

    private static bool ComposeRelayedAsyncComputedFlagValue()
    {
        return RelayAsyncComputedFlag(true).GetAwaiter().GetResult();
    }

    private static bool ComposeForwardedRelayedAsyncComputedFlagValue()
    {
        return ComposeRelayedAsyncComputedFlagValue();
    }

    private static bool ComposeForwardedForwardedRelayedAsyncComputedFlagValue()
    {
        return ComposeForwardedRelayedAsyncComputedFlagValue();
    }

    private static bool ComposeForwardedAsyncComputedFlagValue()
    {
        return ComposeAsyncComputedFlagValue();
    }

    private static async Task<string> BuildAsyncLiteralMessage()
    {
        await Task.Yield();
        return BuildLiteralMessage();
    }

    private static async Task<string> RelayAsyncLiteralMessage()
    {
        await Task.Yield();
        return await BuildAsyncLiteralMessage();
    }

    private static string ComposeAsyncLiteralMessageValue()
    {
        return BuildAsyncLiteralMessage().GetAwaiter().GetResult();
    }

    private static string ComposeRelayedAsyncLiteralMessageValue()
    {
        return RelayAsyncLiteralMessage().GetAwaiter().GetResult();
    }

    private static string ComposeForwardedRelayedAsyncLiteralMessageValue()
    {
        return ComposeRelayedAsyncLiteralMessageValue();
    }

    private static string ComposeForwardedForwardedRelayedAsyncLiteralMessageValue()
    {
        return ComposeForwardedRelayedAsyncLiteralMessageValue();
    }

    private static string ComposeForwardedAsyncLiteralMessageValue()
    {
        return ComposeAsyncLiteralMessageValue();
    }

    private static string ComposeForwardedForwardedAsyncLiteralMessageValue()
    {
        return ComposeForwardedAsyncLiteralMessageValue();
    }

    private static int ComposeAsyncLiteralMessage()
    {
        Console.WriteLine(ComposeAsyncLiteralMessageValue());
        return 0;
    }

    private static int ComposeRelayedAsyncLiteralMessage()
    {
        Console.WriteLine(ComposeRelayedAsyncLiteralMessageValue());
        return 0;
    }

    private static int ComposeForwardedRelayedAsyncLiteralMessage()
    {
        Console.WriteLine(ComposeForwardedRelayedAsyncLiteralMessageValue());
        return 0;
    }

    private static int ComposeForwardedForwardedRelayedAsyncLiteralMessage()
    {
        Console.WriteLine(ComposeForwardedForwardedRelayedAsyncLiteralMessageValue());
        return 0;
    }

    private static int ComposeForwardedAsyncLiteralMessage()
    {
        Console.WriteLine(ComposeForwardedAsyncLiteralMessageValue());
        return 0;
    }

    private static int ComposeForwardedForwardedAsyncLiteralMessage()
    {
        Console.WriteLine(ComposeForwardedForwardedAsyncLiteralMessageValue());
        return 0;
    }

    private static async Task<string> JoinAsync(string left, string right)
    {
        await Task.Yield();
        return string.Concat(left, right);
    }

    private static async Task<string> RelayJoinAsync(string left, string right)
    {
        await Task.Yield();
        return await JoinAsync(left, right);
    }

    private static async Task<string> BuildAsyncComposedMessage(string prefix)
    {
        await Task.Yield();
        return await JoinAsync(prefix, "|System.Console");
    }

    private static async Task<string> RelayAsyncComposedMessage(string prefix)
    {
        await Task.Yield();
        return await BuildAsyncComposedMessage(prefix);
    }

    private static string ComposeAsyncComposedMessageValue()
    {
        return BuildAsyncComposedMessage("corelib-reference-async-string:System.Private.CoreLib|System.Runtime").GetAwaiter().GetResult();
    }

    private static string ComposeRelayedAsyncComposedMessageValue()
    {
        return RelayAsyncComposedMessage("corelib-reference-async-string:System.Private.CoreLib|System.Runtime").GetAwaiter().GetResult();
    }

    private static string ComposeForwardedRelayedAsyncComposedMessageValue()
    {
        return ComposeRelayedAsyncComposedMessageValue();
    }

    private static string ComposeForwardedForwardedRelayedAsyncComposedMessageValue()
    {
        return ComposeForwardedRelayedAsyncComposedMessageValue();
    }

    private static string ComposeForwardedAsyncComposedMessageValue()
    {
        return ComposeAsyncComposedMessageValue();
    }

    private static string ComposeForwardedForwardedAsyncComposedMessageValue()
    {
        return ComposeForwardedAsyncComposedMessageValue();
    }

    private static int ComposeAsyncComposedMessage()
    {
        Console.WriteLine(ComposeAsyncComposedMessageValue());
        return 0;
    }

    private static int ComposeRelayedAsyncComposedMessage()
    {
        Console.WriteLine(ComposeRelayedAsyncComposedMessageValue());
        return 0;
    }

    private static int ComposeForwardedRelayedAsyncComposedMessage()
    {
        Console.WriteLine(ComposeForwardedRelayedAsyncComposedMessageValue());
        return 0;
    }

    private static int ComposeForwardedForwardedRelayedAsyncComposedMessage()
    {
        Console.WriteLine(ComposeForwardedForwardedRelayedAsyncComposedMessageValue());
        return 0;
    }

    private static int ComposeForwardedAsyncComposedMessage()
    {
        Console.WriteLine(ComposeForwardedAsyncComposedMessageValue());
        return 0;
    }

    private static int ComposeForwardedForwardedAsyncComposedMessage()
    {
        Console.WriteLine(ComposeForwardedForwardedAsyncComposedMessageValue());
        return 0;
    }

    private static string ComposeJoinAsyncDirectValue()
    {
        return JoinAsync("corelib-reference-async-join:System.Private.CoreLib|System.Runtime", "|System.Console").GetAwaiter().GetResult();
    }

    private static string ComposeRelayedJoinAsyncDirectValue()
    {
        return RelayJoinAsync("corelib-reference-async-join:System.Private.CoreLib|System.Runtime", "|System.Console").GetAwaiter().GetResult();
    }

    private static string ComposeForwardedRelayedJoinAsyncDirectValue()
    {
        return ComposeRelayedJoinAsyncDirectValue();
    }

    private static string ComposeForwardedForwardedRelayedJoinAsyncDirectValue()
    {
        return ComposeForwardedRelayedJoinAsyncDirectValue();
    }

    private static string ComposeForwardedJoinAsyncDirectValue()
    {
        return ComposeJoinAsyncDirectValue();
    }

    private static string ComposeForwardedForwardedJoinAsyncDirectValue()
    {
        return ComposeForwardedJoinAsyncDirectValue();
    }

    private static int ComposeJoinAsyncDirectMessage()
    {
        Console.WriteLine(ComposeJoinAsyncDirectValue());
        return 0;
    }

    private static int ComposeRelayedJoinAsyncDirectMessage()
    {
        Console.WriteLine(ComposeRelayedJoinAsyncDirectValue());
        return 0;
    }

    private static int ComposeForwardedRelayedJoinAsyncDirectMessage()
    {
        Console.WriteLine(ComposeForwardedRelayedJoinAsyncDirectValue());
        return 0;
    }

    private static int ComposeForwardedForwardedRelayedJoinAsyncDirectMessage()
    {
        Console.WriteLine(ComposeForwardedForwardedRelayedJoinAsyncDirectValue());
        return 0;
    }

    private static int ComposeForwardedJoinAsyncDirectMessage()
    {
        Console.WriteLine(ComposeForwardedJoinAsyncDirectValue());
        return 0;
    }

    private static int ComposeForwardedForwardedJoinAsyncDirectMessage()
    {
        Console.WriteLine(ComposeForwardedForwardedJoinAsyncDirectValue());
        return 0;
    }

    private static void ThrowExceptionNow()
    {
        throw new InvalidOperationException("corelib-reference-exception-proof");
    }

    private static string CaptureExceptionMessage()
    {
        try
        {
            ThrowExceptionNow();
            return "unreachable";
        }
        catch (InvalidOperationException)
        {
            return "corelib-reference-exception:System.Private.CoreLib|System.Console";
        }
    }

    private static int ComposeExceptionMessage()
    {
        Console.WriteLine(CaptureExceptionMessage());
        return 0;
    }
}
