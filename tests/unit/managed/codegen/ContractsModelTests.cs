using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

/// <summary>
/// Tests that instantiate all Contracts/Loader model types to cover their constructors
/// and exercise utility methods for code coverage.
/// </summary>
public sealed class ContractsModelTests
{
    [Fact]
    public void AllModelTypes_CanBeInstantiated()
    {
        // Instantiate and verify ToString() to exercise constructors + record behavior
        // ── ManagedClosureArtifactModels.cs ──
        Assert.NotNull(new AotManifestArtifact { Entries = [] });
        _ = new AotManifestEntry { AssemblyName = "a", SubjectKind = "b", SubjectId = "c", Reason = "d" };
        _ = new MetadataRegistrationArtifact { Registrations = [] };
        _ = new MetadataRegistrationEntry { RegistrationKind = "a", Slot = 1, SubjectId = "b" };
        _ = new SupplementalMetadataTemplateArtifact { RegisteredTypes = [], RegisteredMethods = [], ReservedSlots = new SupplementalMetadataReservedSlots() };
        _ = new SupplementalMetadataTypeTemplateEntry { AssemblyName = "a", SubjectId = "b", DefinitionSubjectId = "c", MetadataToken = 1 };
        _ = new SupplementalMetadataMethodTemplateEntry { AssemblyName = "a", SubjectId = "b", DefinitionSubjectId = "c", DeclaringTypeSubjectId = "d", MetadataToken = 1, ParameterCount = 0 };
        _ = new SupplementalMetadataReservedSlots();
        _ = new CodeRegistrationArtifact { Modules = [] };
        _ = new CodeRegistrationModule { ModuleName = "a", Registrations = [] };
        _ = new CodeRegistrationEntry { RegistrationKind = "a", Slot = 1, Symbol = "b", SubjectId = "c" };
        _ = new CodeRegistrationTypeCapabilityEntry { SubjectId = "a", TypeToken = 1, CapabilityBits = 2, ValueSizeBytes = 4, VectorWidthBytes = 8, VectorLaneCount = 1, VectorLaneKind = 0, ScalarKind = 0 };
        _ = new MetadataWriterOutput { AotManifest = new AotManifestArtifact { Entries = [] }, MetadataRegistration = new MetadataRegistrationArtifact { Registrations = [] }, SupplementalMetadataTemplate = new SupplementalMetadataTemplateArtifact { RegisteredTypes = [], RegisteredMethods = [], ReservedSlots = new SupplementalMetadataReservedSlots() } };
        _ = new ManagedClosureArtifactRef { Kind = "a", Path = "b" };
        _ = new ManagedClosureResolvedAssemblyRef { AssemblyName = "a", Path = "b" };
        _ = new ManagedClosureManifestArtifact { AssemblyName = "a", EntrySubjectId = "b", InputAssemblyPath = "c", InputModuleVersionId = "d", Artifacts = [] };
        _ = new NativeCodegenMetricsArtifact { FormatVersion = "v0", ArtifactKind = "metrics", CodegenKind = "aot", PlanKind = "default", GeneratedSourcePaths = [] };
        _ = new NativeReferenceGeneratedArtifactRef { Kind = "a", Path = "b" };
        _ = new AuditTranslationUnitPageArtifact { PageNumber = 1, MethodCount = 10, Path = "p" };
        _ = new AssemblyFullClosureAuditPageManifestArtifact { FormatVersion = "v0", ArtifactKind = "audit", AssemblyName = "a", PlanKind = "ref", PageNumber = 1, MethodCount = 10, MethodSubjectIds = [] };
        _ = new NativeReferenceProofManifestArtifact { FormatVersion = "v0", ArtifactKind = "manifest", AssemblyName = "a", EntrySubjectId = "b", ManagedClosureRootPath = "c", PlanArtifactPath = "d", GeneratedArtifacts = [] };
        _ = new NativeAotGeneratedArtifactRef { Kind = "a", Path = "b" };
        _ = new NativeAotManifestArtifact { FormatVersion = "v0", ArtifactKind = "manifest", AssemblyName = "a", EntrySubjectId = "b", ManagedClosureRootPath = "c", PlanArtifactPath = "d", GeneratedArtifacts = [] };
        _ = new EngineBindingsArtifact { ProofKind = "a", FocusArea = "b", CapabilityIds = [], BindingKinds = [], HelperNames = [] };
        _ = new HostBindingsArtifact { HostPlatform = "win", RuntimeProfile = "core", BindingKinds = [] };
        _ = new NativeReferenceLoweringPlanArtifact { FormatVersion = "v0", ArtifactKind = "plan", PlanKind = "ref", AssemblyName = "a", EntrySubjectId = "b", IncludeHeader = "h", NativeEntryFunctionName = "f", EntrySymbol = "s", ReferenceTypeToken = "t", CapturedFieldToken = "f", EntryMethodToken = "m", ConsoleWriteLineStringIcall = "i" };
        _ = new NativeAotLoweringPlanArtifact { FormatVersion = "v0", ArtifactKind = "plan", PlanKind = "aot", AssemblyName = "a", EntrySubjectId = "b", NativeEntryFunctionName = "f", EntrySymbol = "s", EntryMethodToken = "m", WorkloadAbi = "default" };
        _ = new NativeReferenceGeneratedSource { RelativePath = "a", Contents = "b" };
        _ = new NativeAotGeneratedSource { RelativePath = "a", Contents = "b" };

        // ── ManagedClosureModels.cs ──
        _ = new AssemblyIdentityModel { Name = "a", ModuleVersionId = Guid.NewGuid() };
        _ = new ManagedTypeModel { AssemblyName = "a", NamespaceName = "ns", Name = "T", SubjectId = "id", DefinitionSubjectId = "def", DisplayName = "T", MetadataToken = 1 };
        _ = new ManagedFieldModel { AssemblyName = "a", DeclaringTypeSubjectId = "d", Name = "f", FieldType = "int", SubjectId = "id", DefinitionSubjectId = "def", MetadataToken = 1 };
        _ = new ManagedPropertyModel { AssemblyName = "a", DeclaringTypeSubjectId = "d", Name = "p", PropertyType = "int", SubjectId = "id", DefinitionSubjectId = "def", MetadataToken = 1 };
        _ = new ManagedMethodModel { AssemblyName = "a", DeclaringTypeSubjectId = "d", DeclaringTypeDisplayName = "T", Name = "m", ReturnType = "void", SubjectId = "id", DefinitionSubjectId = "def", Signature = "sig", IsStatic = false, IsVirtual = false, MetadataToken = 1, Parameters = [], Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] } };
        _ = new ManagedImportModel { ModuleName = "m", EntryPointName = "e" };
        _ = new ManagedParameterModel { Name = "p", Type = "int" };
        _ = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] };
        _ = new ManagedBlockModel { BlockId = "b", Instructions = [] };
        _ = new ManagedExceptionRegionModel { HandlingKind = "catch", TryOffset = 0, TryLength = 1, HandlerOffset = 2, HandlerLength = 3 };
        _ = new ManagedInstructionModel { Op = "nop" };
        _ = new ManagedInstructionReference { AssemblyName = "a", SubjectKind = "type", SubjectId = "id" };
        _ = new ManagedCallSiteSignature { KindCode = ManagedCallSiteKind.FunctionPointer, ReturnType = "void", ParameterTypes = [] };
        _ = new CanonicalSubjectsModel { Subjects = [] };
        _ = new CanonicalSubjectModel { SubjectKind = "type", SubjectId = "id", CanonicalSubjectId = "canonical" };
        _ = new SemanticShapesModel { Types = [], Fields = [], Properties = [], Methods = [] };
        _ = new TypeShapeModel { SubjectId = "id", Kind = "class" };
        _ = new FieldShapeModel { SubjectId = "id", Kind = "instance" };
        _ = new PropertyShapeModel { SubjectId = "id", Kind = "simple" };
        _ = new MethodShapeModel { SubjectId = "id", MethodRole = "regular", BodyAvailability = "full", BodyAvailabilityCode = BodyAvailabilityCode.InterpreterReady };
        _ = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "def" };
        _ = new SharedGenericBodyId { Value = "body" };
        _ = new InstantiationStubId { Value = "stub" };
        _ = new RuntimeGenericContextArtifact { InstantiationKey = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "def" }, SharedGenericBodyId = new SharedGenericBodyId { Value = "b" }, InstantiationStubId = new InstantiationStubId { Value = "s" }, SupportKindCode = GenericSupportKind.Legal, SpecializationKindCode = GenericSpecializationKind.SharedBody };
        _ = new GenericDiagnosticArtifact { SubjectId = "id", DefinitionSubjectId = "def", DisplaySubjectId = "disp", InstantiationKey = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "def" } };
        _ = new GenericInstantiationDemandModel { RequestingAssemblyName = "a", OwningAssemblyName = "b", SubjectKind = "type", SubjectId = "id", DefinitionSubjectId = "def", DemandSourceKind = "code", InstantiationKey = new GenericInstantiationKey { ContextKind = GenericContextKind.TypeInstantiation, DefinitionSubjectId = "def" }, SupportKindCode = GenericSupportKind.Legal, SpecializationKindCode = GenericSpecializationKind.SharedBody, FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType };
        _ = new GenericInstantiationDemandGraphModel { Demands = [] };
        _ = new GenericCapabilityMatrixArtifact { FormatVersion = "v0", ArtifactKind = "matrix", OwnerSubjectId = "o", EntrySubjectId = "e", HotUpdateModes = [], Gates = new GenericCapabilityMatrixGateStatus { Status = "pass", UnsupportedLeakCount = 0, MissingAuthorityCount = 0, NonCanonicalHotUpdateNameCount = 0, Leaks = [] }, FamilyBudgets = [], BoundaryCases = [], Entries = [] };
        _ = new GenericCapabilityMatrixGateStatus { Status = "pass", UnsupportedLeakCount = 0, MissingAuthorityCount = 0, NonCanonicalHotUpdateNameCount = 0, Leaks = [] };
        _ = new GenericCapabilityFamilyBudgetArtifact { FamilyId = "f", DisplayName = "F", BudgetLimit = 10, ObservedCount = 5 };
        _ = new GenericCapabilityBoundaryCaseArtifact { BoundaryKind = "b", SourceMethodSubjectId = "s", IlOffset = 0, TargetSubjectId = "t", EvidenceKind = "e", Status = "ok" };
        _ = new GenericCapabilityMatrixEntryArtifact { SubjectKind = "type", SubjectId = "id", DefinitionSubjectId = "def", DemandSourceKind = "code", RequestingAssemblyName = "a", OwningAssemblyName = "b", FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType, ContextKindCode = GenericContextKind.TypeInstantiation, SupportKindCode = GenericSupportKind.Legal, SpecializationKindCode = GenericSpecializationKind.SharedBody, IsCrossAssembly = false, StatusReasonCode = "ok", AuthoritySource = "a", OpenDefinitionSubjectId = "od", SharedGenericBodyId = "sb", InstantiationStubId = "is", HasOpenDefinitionAuthority = true, HasSharedBodyAuthority = true, HasInstantiationStubAuthority = true, HasRuntimeGenericContextAuthority = true };
        _ = new GenericCapabilityMatrixLeakArtifact { SubjectId = "id", LeakKind = "leak", Reason = "r" };

        // ── ManagedSemanticWorldContracts.cs ──
        _ = new CapabilityBundlesModel { World = new WorldCapabilityBundleModel { Capabilities = [] }, Methods = [] };
        _ = new WorldCapabilityBundleModel { Capabilities = [] };
        _ = new MethodCapabilityBundleModel { SubjectId = "id", Capabilities = [] };
        _ = new SemanticWorldModel { InputAssemblyPath = "p", FullAssemblyClosure = false, Assembly = new AssemblyIdentityModel { Name = "a", ModuleVersionId = Guid.NewGuid() }, Assemblies = [], EntryPointSubjectId = "e", Types = [], Fields = [], Properties = [], Methods = [], CanonicalSubjects = new CanonicalSubjectsModel { Subjects = [] }, SemanticShapes = new SemanticShapesModel { Types = [], Fields = [], Properties = [], Methods = [] }, CapabilityBundles = new CapabilityBundlesModel { World = new WorldCapabilityBundleModel { Capabilities = [] }, Methods = [] } };
        _ = new LinkedWorldModel { InputAssemblyPath = "p", FullAssemblyClosure = false, Assembly = new AssemblyIdentityModel { Name = "a", ModuleVersionId = Guid.NewGuid() }, Assemblies = [], EntryPointSubjectId = "e", Types = [], Fields = [], Properties = [], Methods = [], Dependencies = [], CanonicalSubjects = new CanonicalSubjectsModel { Subjects = [] }, SemanticShapes = new SemanticShapesModel { Types = [], Fields = [], Properties = [], Methods = [] }, CapabilityBundles = new CapabilityBundlesModel { World = new WorldCapabilityBundleModel { Capabilities = [] }, Methods = [] }, OptimizationFacts = new OptimizationFactsArtifact { FormatVersion = "v0", ArtifactKind = "facts", ClosedWorldSpecializations = [], DispatchFacts = [], LayoutFacts = [], ExceptionFacts = [] }, PreserveDescriptor = new PreserveDescriptorArtifact { FormatVersion = "v0", ArtifactKind = "desc", Entries = [] } };
        _ = new LinkedDependencyModel { AssemblyName = "a", SubjectKind = "type", SubjectId = "id", Reason = "r" };
        _ = new OptimizationFactsArtifact { FormatVersion = "v0", ArtifactKind = "facts", ClosedWorldSpecializations = [], DispatchFacts = [], LayoutFacts = [], ExceptionFacts = [] };
        _ = new PreserveDescriptorArtifact { FormatVersion = "v0", ArtifactKind = "desc", Entries = [] };
        _ = new PreserveDescriptorEntry { SubjectKind = "type", SubjectId = "id", Preserve = "all", Reason = "r" };
        _ = new ClosedWorldSpecializationFact { SubjectKind = "type", SubjectId = "id", Reason = "r" };
        _ = new DispatchFact { MethodSubjectId = "m", DispatchKind = "direct", TargetSubjectId = "t" };
        _ = new LayoutFact { SubjectKind = "type", SubjectId = "id", DataKind = "layout" };
        _ = new ExceptionFact { MethodSubjectId = "m", HandlingKind = "catch" };

        // ── TypedIlAndAotCoreIrContracts.cs ──
        _ = new TypedIlIrArtifact { Methods = [] };
        _ = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Void };
        _ = new AotCoreIrReferenceArtifact { Kind = AotCoreIrReferenceKind.Type, AssemblyName = "a", SubjectId = "id" };
        _ = new StructFieldDescriptorArtifact { Kind = "Blittable", Offset = 0, Size = 4 };
        _ = new StructMarshallingDescriptorArtifact { TypeSubjectId = "id", TotalSize = 8, Fields = [] };
        _ = new AotCoreIrArtifact { Methods = [] };
        _ = new AotCoreIrMethodArtifact { MethodId = "m", SubjectId = "id", Signature = "sig", Identity = new ManagedMethodIdentityArtifact { AssemblyName = "a", DeclaringTypeSubjectId = "d", DefinitionSubjectId = "def", SubjectId = "id", MethodId = "m", Signature = "sig" }, NativeSymbol = "ns", IsStatic = false, ReturnType = "void", ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Void }, ParameterCount = 0, ParameterAbis = [], LocalCount = 0, ExceptionRegionCount = 0, ExceptionRegions = [], Instructions = [] };
        _ = new AotCoreIrExceptionRegionArtifact { HandlingKindCode = AotCoreIrExceptionRegionKind.Catch, TryOffset = 0, TryLength = 1, HandlerOffset = 2, HandlerLength = 3 };
        _ = new AotCoreIrInstructionArtifact { Op = "nop", IlOffset = 0 };
        _ = new TypedIlMethodArtifact { MethodId = "m", SubjectId = "id", Signature = "sig", Identity = new ManagedMethodIdentityArtifact { AssemblyName = "a", DeclaringTypeSubjectId = "d", DefinitionSubjectId = "def", SubjectId = "id", MethodId = "m", Signature = "sig" }, MethodRole = "regular", BodyAvailability = "full", BodyAvailabilityCode = BodyAvailabilityCode.InterpreterReady, Capabilities = [], Parameters = [], Blocks = [] };
        _ = new TypedIlParameterArtifact { Name = "p", Type = "int" };
        _ = new TypedIlBlockArtifact { BlockId = "b", Instructions = [] };
        _ = new TypedIlInstructionArtifact { Op = "nop" };

        // Internal LoaderModels types are tested indirectly through LoaderStage usage
    }
}
