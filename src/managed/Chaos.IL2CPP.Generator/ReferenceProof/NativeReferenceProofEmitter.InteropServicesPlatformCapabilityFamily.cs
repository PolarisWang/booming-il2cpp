using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static bool IsInteropServicesSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Runtime.InteropServices.", StringComparison.Ordinal) &&
               !subjectId.Contains("/System.Runtime.InteropServices.Marshal::", StringComparison.Ordinal) &&
               !subjectId.Contains("SafeBuffer", StringComparison.Ordinal) &&
               !subjectId.Contains("GCHandle", StringComparison.Ordinal) &&
               !subjectId.Contains("OSPlatform", StringComparison.Ordinal) &&
               !subjectId.Contains("NFloat", StringComparison.Ordinal) &&
               !subjectId.Contains("NativeMemory", StringComparison.Ordinal) &&
               !subjectId.Contains("ComAwareEventInfo", StringComparison.Ordinal) &&
               !subjectId.Contains("ImportedFromTypeLib", StringComparison.Ordinal) &&
               !subjectId.Contains("DefaultDllImportSearchPaths", StringComparison.Ordinal) &&
               !subjectId.Contains("DefaultParameterValue", StringComparison.Ordinal) &&
               !subjectId.Contains("DefaultCharSet", StringComparison.Ordinal) &&
               !subjectId.Contains("UnmanagedFunctionPointer", StringComparison.Ordinal) &&
               !subjectId.Contains("AllowReversePInvoke", StringComparison.Ordinal) &&
               !subjectId.Contains("BestFitMapping", StringComparison.Ordinal) &&
               !subjectId.Contains("LCIDConversion", StringComparison.Ordinal) &&
               !subjectId.Contains("ComSourceInterfaces", StringComparison.Ordinal) &&
               !subjectId.Contains("PrimaryInteropAssembly", StringComparison.Ordinal) &&
               !subjectId.Contains("TypeIdentifier", StringComparison.Ordinal) &&
               !subjectId.Contains("AutomationProxy", StringComparison.Ordinal) &&
               !subjectId.Contains("DllImportSearchPath", StringComparison.Ordinal) &&
               !subjectId.Contains("StandardOleMarshalObject", StringComparison.Ordinal) &&
               !subjectId.Contains("Architecture", StringComparison.Ordinal) &&
               !subjectId.Contains("OperatingSystem", StringComparison.Ordinal) &&
               !subjectId.Contains("IDispatch", StringComparison.Ordinal) &&
               !subjectId.Contains("ComTypes", StringComparison.Ordinal) &&
               !subjectId.Contains("ComWrappers", StringComparison.Ordinal) &&
               !subjectId.Contains("ErrorWrapper", StringComparison.Ordinal) &&
               !subjectId.Contains("CurrencyWrapper", StringComparison.Ordinal) &&
               !subjectId.Contains("DispatchWrapper", StringComparison.Ordinal) &&
               !subjectId.Contains("ObjectHandle", StringComparison.Ordinal) &&
               !subjectId.Contains("HandleRef", StringComparison.Ordinal) &&
               !subjectId.Contains("ArrayWithOffset", StringComparison.Ordinal) &&
               !subjectId.Contains("PosixSignalRegistration", StringComparison.Ordinal) &&
               !subjectId.Contains("RuntimeInformation", StringComparison.Ordinal) &&
               !subjectId.Contains("ICustomAdapter", StringComparison.Ordinal) &&
               !subjectId.Contains("ICustomQueryInterface", StringComparison.Ordinal) &&
               !subjectId.Contains("ICustomMarshaler", StringComparison.Ordinal) &&
               !subjectId.Contains("ITypeLibConverter", StringComparison.Ordinal) &&
               !subjectId.Contains("ITypeLibExporter", StringComparison.Ordinal) &&
               !subjectId.Contains("ITypeLibExporterNotifySink", StringComparison.Ordinal) &&
               !subjectId.Contains("ITypeLibImporter", StringComparison.Ordinal) &&
               !subjectId.Contains("ITypeLibImporterNotifySink", StringComparison.Ordinal) &&
               !subjectId.Contains("ExporterEventKind", StringComparison.Ordinal) &&
               !subjectId.Contains("ImporterEventKind", StringComparison.Ordinal) &&
               !subjectId.Contains("TypeLibConverter", StringComparison.Ordinal) &&
               !subjectId.Contains("TypeLibExporter", StringComparison.Ordinal) &&
               !subjectId.Contains("TypeLibImporter", StringComparison.Ordinal) &&
               !subjectId.Contains("InvalidOleVariantType", StringComparison.Ordinal) &&
               !subjectId.Contains("InvalidComObject", StringComparison.Ordinal) &&
               !subjectId.Contains("SYSKIND", StringComparison.Ordinal) &&
               !subjectId.Contains("LIBFLAGS", StringComparison.Ordinal) &&
               !subjectId.Contains("TYPELIBATTR", StringComparison.Ordinal) &&
               !subjectId.Contains("ComEventInterface", StringComparison.Ordinal) &&
               !subjectId.Contains("ComDefaultInterface", StringComparison.Ordinal) &&
               !subjectId.Contains("ComCompatibleVersion", StringComparison.Ordinal) &&
               !subjectId.Contains("ManagedToNativeComInteropStub", StringComparison.Ordinal) &&
               !subjectId.Contains("SetWin32ContextInIDispatch", StringComparison.Ordinal) &&
               !subjectId.Contains("ComRegisterFunction", StringComparison.Ordinal) &&
               !subjectId.Contains("ComUnregisterFunction", StringComparison.Ordinal) &&
               !subjectId.Contains("ProgIdAttribute", StringComparison.Ordinal) &&
               !subjectId.Contains("Guid", StringComparison.Ordinal);
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInteropServicesPlatformCapabilityFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!IsInteropServicesSubjectId(buildContext.SubjectId))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonInteropServicesPlatformCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("interop-services-platform-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInteropServicesPlatformCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundInteropServicesPlatformCapabilityCore(
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

    private static bool TryBuildAssemblyBoundInteropServicesPlatformCapabilityCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;

        // First try: managed-invoke for methods with canonical body
        if (methodsBySubjectId.TryGetValue(subjectId, out var method) &&
            RuntimeSkeletonGenericManagedInvokeAbiFactory.TryCreate(
                method,
                "interop-services-managed-invoke-v1",
                out var abi))
        {
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

        // Fallback: residual stub returning default/zero
        return TryBuildInteropServicesResidualStub(subjectId, stubName, out stub);
    }

    private static bool TryBuildInteropServicesResidualStub(
        string subjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        var managedReturnType = RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(subjectId);
        var parameterTypes = GetMethodParameterTypesFromSubjectId(subjectId);
        var argumentShapes = parameterTypes
            .Select((_, index) => new RuntimeSkeletonManagedInvokePointerArgShape(
                $"void* arg{index};",
                string.Empty,
                $"request->arg{index}"))
            .ToArray();

        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                managedReturnType,
                out var returnShape,
                includeIndirectFlag: true))
        {
            return false;
        }

        var helperStatements = BuildInteropServicesResidualHelperStatements(managedReturnType);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = "interop-services-residual-stub-v1",
            ["capability_area"] = "interop-services",
            ["this_field_declaration"] = string.Empty,
            ["this_validation_statement"] = string.Empty,
            ["arg_field_declarations"] = string.Join("\n    ", argumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = string.Empty,
            ["return_managed_type"] = managedReturnType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["helper_statements"] = helperStatements,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath),
            model);
        return true;
    }

    private static string BuildInteropServicesResidualHelperStatements(string managedReturnType)
    {
        return managedReturnType switch
        {
            "System.Void" => "// interop residual stub: no managed metadata engine",
            "System.Boolean" => "*static_cast<bool*>(return_value_ptr) = false;",
            "System.Byte" => "*static_cast<CHAOS_IL2CPP_UINT8*>(return_value_ptr) = 0u;",
            "System.SByte" => "*static_cast<CHAOS_IL2CPP_INT8*>(return_value_ptr) = 0;",
            "System.Int16" => "*static_cast<CHAOS_IL2CPP_INT16*>(return_value_ptr) = 0;",
            "System.UInt16" => "*static_cast<CHAOS_IL2CPP_UINT16*>(return_value_ptr) = 0u;",
            "System.Int32" => "*static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = 0;",
            "System.UInt32" => "*static_cast<CHAOS_IL2CPP_UINT32*>(return_value_ptr) = 0u;",
            "System.Int64" => "*static_cast<CHAOS_IL2CPP_INT64*>(return_value_ptr) = 0;",
            "System.UInt64" => "*static_cast<CHAOS_IL2CPP_UINT64*>(return_value_ptr) = 0u;",
            "System.Single" => "*static_cast<float*>(return_value_ptr) = 0.0f;",
            "System.Double" => "*static_cast<double*>(return_value_ptr) = 0.0;",
            "System.IntPtr" or "System.UIntPtr" => "*static_cast<CHAOS_IL2CPP_INTPTR*>(return_value_ptr) = 0;",
            _ => "*request->return_value = nullptr;",
        };
    }
}
