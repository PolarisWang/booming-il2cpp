using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonGlobalizationCalendarManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Globalization.Calendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.GregorianCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.HijriCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.UmAlQuraCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.PersianCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.ChineseLunisolarCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.JapaneseCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.JapaneseLunisolarCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.JulianCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.KoreanCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.KoreanLunisolarCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.TaiwanCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.TaiwanLunisolarCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.ThaiBuddhistCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.HebrewCalendar::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Globalization.EastAsianLunisolarCalendar::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonGlobalizationCalendarManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("globalization-calendar-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonGlobalizationCalendarManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundGlobalizationCalendarManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundGlobalizationCalendarManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "globalization-calendar-managed-invoke-v1", out stub);
}
