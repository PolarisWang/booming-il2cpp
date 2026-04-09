using System.IO;

namespace MainlineFeaturePack;

internal sealed class FeatureBanner
{
    private readonly string _name;

    public FeatureBanner(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return "Mainline native proof: " + _name + ".";
    }
}

internal static class ProofEntry
{
    public static int Run()
    {
        var banner = new FeatureBanner("feature pack");
        Console.WriteLine(banner.BuildMessage());
        return 0;
    }
}

internal static class TraceDocumentFactory
{
    public static string GetWindowsWarmupTrace()
    {
        return """{"formatVersion":"v0","traceName":"warmupTrace","targetPlatform":"windows","samples":[{"sampleId":"windows-lazy-method-cache","scenario":"\u9996\u6b21\u61d2\u8bbf\u95ee\u89e6\u53d1\u7c7b\u578b\u4e0e\u65b9\u6cd5\u9884\u70ed\uff0c\u7b2c\u4e8c\u6b21\u8bbf\u95ee\u547d\u4e2d\u7f13\u5b58\u3002","events":[{"eventName":"warmup.type.requested","phase":"requested","subjectKind":"type","subjectId":"Game.Core/Game.Player","order":10,"status":"ok","trigger":"lazy-touch"},{"eventName":"warmup.type.started","phase":"started","subjectKind":"type","subjectId":"Game.Core/Game.Player","order":11,"status":"ok"},{"eventName":"warmup.type.completed","phase":"completed","subjectKind":"type","subjectId":"Game.Core/Game.Player","order":12,"status":"ok"},{"eventName":"warmup.method.requested","phase":"requested","subjectKind":"method","subjectId":"Game.Core/Game.Player::TakeDamage(System.Int32)","order":13,"status":"ok","trigger":"lazy-touch"},{"eventName":"warmup.method.started","phase":"started","subjectKind":"method","subjectId":"Game.Core/Game.Player::TakeDamage(System.Int32)","order":14,"status":"ok"},{"eventName":"warmup.method.completed","phase":"completed","subjectKind":"method","subjectId":"Game.Core/Game.Player::TakeDamage(System.Int32)","order":15,"status":"ok"},{"eventName":"warmup.method.hit","phase":"cached","subjectKind":"method","subjectId":"Game.Core/Game.Player::TakeDamage(System.Int32)","order":16,"status":"cached"}]}],"traceSource":"host-embedding-session","sessionTrace":["host:start","registration:assemblies","registration:managed-exports","registration:callbacks","managed-entry:begin","managed-entry:end","warmup-trace|sampleId=windows-lazy-method-cache|order=10|eventName=warmup.type.requested|phase=requested|subjectKind=type|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVy|status=ok","warmup-trace|sampleId=windows-lazy-method-cache|order=11|eventName=warmup.type.started|phase=started|subjectKind=type|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVy|status=ok","warmup-trace|sampleId=windows-lazy-method-cache|order=12|eventName=warmup.type.completed|phase=completed|subjectKind=type|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVy|status=ok","warmup-trace|sampleId=windows-lazy-method-cache|order=13|eventName=warmup.method.requested|phase=requested|subjectKind=method|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVyOjpUYWtlRGFtYWdlKFN5c3RlbS5JbnQzMik=|status=ok","warmup-trace|sampleId=windows-lazy-method-cache|order=14|eventName=warmup.method.started|phase=started|subjectKind=method|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVyOjpUYWtlRGFtYWdlKFN5c3RlbS5JbnQzMik=|status=ok","warmup-trace|sampleId=windows-lazy-method-cache|order=15|eventName=warmup.method.completed|phase=completed|subjectKind=method|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVyOjpUYWtlRGFtYWdlKFN5c3RlbS5JbnQzMik=|status=ok","warmup-trace|sampleId=windows-lazy-method-cache|order=16|eventName=warmup.method.hit|phase=cached|subjectKind=method|subjectId=R2FtZS5Db3JlL0dhbWUuUGxheWVyOjpUYWtlRGFtYWdlKFN5c3RlbS5JbnQzMik=|status=cached"]}""";
    }
}

internal static class Program
{
    private static int Main(string[] args)
    {
        File.WriteAllText(args[3], TraceDocumentFactory.GetWindowsWarmupTrace());
        return 0;
    }
}
