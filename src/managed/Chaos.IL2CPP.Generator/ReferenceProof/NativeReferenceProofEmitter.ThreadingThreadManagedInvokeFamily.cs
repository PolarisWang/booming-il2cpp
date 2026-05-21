using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    [RuntimeSkeletonFamilyPlugin("threading-thread-managed-invoke", CodeGenPluginProductLine.NativeReference, 440, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingThreadManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!IsThreadingThreadSubjectId(buildContext.SubjectId))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonThreadingThreadManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("threading-thread-managed-invoke-unsupported-shape");
    }

    private static bool IsThreadingThreadSubjectId(string subjectId)
    {
        return (subjectId.Contains("/System.Threading.", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Tasks.", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Interlocked", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Volatile", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.PortableThreadPool", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.ThreadPool", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.CancellationToken", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.WaitHandle", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Monitor", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Mutex", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Semaphore", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.EventWaitHandle", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.ReaderWriterLock", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.RegisteredWaitHandle", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.ThreadPoolBoundHandle", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.ThreadPoolWorkQueue", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.HostExecutionContext", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.ExecutionContext", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.AsyncLocal", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.SynchronizationContext", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Timer", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Overlapped", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Barrier", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.CountdownEvent", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.SpinLock", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.SpinWait", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.IOCompletion", StringComparison.Ordinal) &&
                !subjectId.Contains("/System.Threading.Lock", StringComparison.Ordinal));
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingThreadManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundThreadingThreadManagedInvokeStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static bool TryBuildAssemblyBoundThreadingThreadManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundThreadingThreadManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundThreadingThreadManagedInvokeCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonThreadingManagedInvokeAbiFactory.TryCreate(
                method,
                "threading-thread-managed-invoke-v1",
                out var abi))
        {
            return false;
        }

        return TryRenderValueTypeManagedInvokeStub(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonIdentityStructManagedInvokeStubTemplateRelativePath),
            assemblyName,
            subjectId,
            metadataRegistration,
            stubName,
            abi.ContractId,
            abi.ThisFieldDeclaration,
            abi.ThisValidationStatement,
            abi.ThisArgumentExpression,
            abi.ArgumentShapes,
            abi.ArgumentValidationStatements,
            abi.ArgumentCount,
            abi.ArgumentStorageSize,
            abi.ReturnShape,
            out stub);
    }
}
