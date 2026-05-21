namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringReturnForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringReturnForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringLiteralAppendCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringLiteralAppendStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringForwarderConsoleWriteLineStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderConsoleWriteLineStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorGetterConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorRenderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticLiteralStringReturnCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticLiteralStringReturnStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticResourceKeyStringReturnCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticResourceKeyStringReturnStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerConsoleWriteLineStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var executableLoweringPlan = TryBuildAssemblyFullClosureExecutableLoweringPlan(
            buildContext.LoweringPlan.AssemblyName,
            buildContext.SubjectId,
            buildContext.Methods,
            buildContext.MetadataRegistration,
            buildContext.MethodPointers);
        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "staticCallCtorGetter", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundStaticCallCtorGetterStub(
                    executableLoweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var executableLoweringPlan = TryBuildAssemblyFullClosureExecutableLoweringPlan(
            buildContext.LoweringPlan.AssemblyName,
            buildContext.SubjectId,
            buildContext.Methods,
            buildContext.MetadataRegistration,
            buildContext.MethodPointers);
        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "constructorThenInstanceCall", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundConstructorThenInstanceCallStub(
                    executableLoweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }
}
