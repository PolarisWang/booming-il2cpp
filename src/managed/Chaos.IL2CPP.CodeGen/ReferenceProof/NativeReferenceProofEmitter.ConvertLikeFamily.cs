namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticIntForwarderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoolIdentityForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoolProducerForwarderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikePrimitiveCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticPrimitiveConvertStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringCharProviderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringCharProviderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProviderPassthroughStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeObjectCharProviderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticObjectCharProviderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoxedValueTypeStringInstanceCallStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.ClosureManifest,
                buildContext.ExternalMetadataTokenResolver,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var outputManagedType = GetMethodReturnType(buildContext.SubjectId);

        if (TryBuildAssemblyBoundStaticBoxedValueTypeInstanceCallStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.ClosureManifest,
                buildContext.ExternalMetadataTokenResolver,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                outputManagedType,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringCallForwarderStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.ExternalMetadataTokenResolver,
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var outputManagedType = GetMethodReturnType(buildContext.SubjectId);
        if (TryBuildAssemblyBoundStaticValueTypeCallForwarderStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.ExternalMetadataTokenResolver,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                outputManagedType,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var outputManagedType = GetMethodReturnType(buildContext.SubjectId);
        if (TryBuildAssemblyBoundInstanceValueTypeCallForwarderStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.ExternalMetadataTokenResolver,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                outputManagedType,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var outputManagedType = GetMethodReturnType(buildContext.SubjectId);

        if (TryBuildAssemblyBoundStaticValueTypeIdentityForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                outputManagedType,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoxedValueTypeCharInvalidCastStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoxedIConvertibleCharInvalidCastStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var outputManagedType = GetMethodReturnType(buildContext.SubjectId);
        if (TryBuildAssemblyBoundStaticCheckedPrimitiveConvertStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                outputManagedType,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeByteForwarderCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticByteReturnForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        if (TryBuildAssemblyBoundStaticCharReturnForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }
}
