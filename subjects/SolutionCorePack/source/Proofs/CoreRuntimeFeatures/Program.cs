using Chaos.TestFramework;
using System.IO;

namespace CoreRuntimeFeatures;

internal static class ProofEntry
{
    public static int Run()
    {
        RunProof(AsyncAwaitProofEntry.Run());
        RunProof(ArithmeticOpsProofEntry.Run());
        RunProof(ArrayCovarianceProofEntry.Run());
        RunProof(ArrayOpsProofEntry.Run());
        RunProof(ArrayBoxingProofEntry.Run());
        RunProof(BitwiseOpsProofEntry.Run());
        RunProof(BranchOpsProofEntry.Run());
        RunProof(BoxedStructFieldReadProofEntry.Run());
        RunProof(CustomAttributeLookupProofEntry.Run());
        RunProof(ConversionOpsProofEntry.Run());
        RunProof(IntegerCarrierProofEntry.Run());
        RunProof(CrossBoundaryExceptionProofEntry.Run());
        RunProof(CastTypeCheckProofEntry.Run());
        RunProof(AotClosureValidationProofEntry.Run());
        RunProof(DelegateCallbackInteropProofEntry.Run());
        RunProof(DelegateChainProofEntry.Run());
        RunProof(DelegateProofEntry.Run());
        RunProof(DirectCallVirtProofEntry.Run());
        RunProof(DispatchProofEntry.Run());
        RunProof(EventCallbackFlowProofEntry.Run());
        RunProof(ExceptionProofEntry.Run());
        RunProof(FinallyAndFilterProofEntry.Run());
        RunProof(FunctionPointerProofEntry.Run());
        RunProof(GcSensitiveFlowProofEntry.Run());
        RunProof(GenericCollectionProofEntry.Run());
        RunProof(GenericConstraintProofEntry.Run());
        RunProof(GenericInstantiationProofEntry.Run());
        RunProof(GenericSharingBoundaryProofEntry.Run());
        RunProof(GenericLayoutProofEntry.Run());
        RunProof(InterfaceDispatchProofEntry.Run());
        RunProof(IteratorStateMachineProofEntry.Run());
        RunProof(LambdaClosureCaptureProofEntry.Run());
        RunProof(LinkerPreserveContractProofEntry.Run());
        RunProof(LinkerStrippingProofEntry.Run());
        RunProof(LoaderContractProofEntry.Run());
        RunProof(MarshalingProofEntry.Run());
        RunProof(MemberMetadataLookupProofEntry.Run());
        RunProof(MetadataSupplementProofEntry.Run());
        RunProof(MonitorAndLockingProofEntry.Run());
        RunProof(NativeCallInteropProofEntry.Run());
        RunProof(NestedExceptionProofEntry.Run());
        RunProof(ObjectFieldProofEntry.Run());
        RunProof(ObjectReturnProofEntry.Run());
        RunProof(OverflowOpsProofEntry.Run());
        RunProof(ReflectionInteropClosureEntry.Run());
        RunProof(RefStructAndStackallocProofEntry.Run());
        RunProof(RequiredInstantiationClosureProofEntry.Run());
        RunProof(ResourceLifecycleProofEntry.Run());
        RunProof(RuntimeHelpersProofEntry.Run());
        RunProof(SpanAndMemoryProofEntry.Run());
        RunProof(StructMarshalingProofEntry.Run());
        RunProof(TaskAndValueTaskFlowProofEntry.Run());
        RunProof(TaskSchedulingProofEntry.Run());
        RunProof(ThreadingProofEntry.Run());
        RunProof(ThreadLocalStateProofEntry.Run());
        RunProof(UnsafePointerProofEntry.Run());
        RunProof(VTableDispatchProofEntry.Run());

        return 0;
    }

    private static void RunProof(int exitCode)
    {
        Assert.Equal(0, exitCode);
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
