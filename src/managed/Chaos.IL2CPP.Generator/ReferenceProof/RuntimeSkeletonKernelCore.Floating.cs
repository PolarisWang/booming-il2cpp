using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonKernelCore
{

    private static bool TryCreateFloatingPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        var managedTypeName = descriptor.ManagedTypeName;

        if (suffix == $"CompareTo:System.Int32({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->this_arg, request->arg0)",
                out plan);
        }

        if (suffix == $"Equals:System.Boolean({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->this_arg, request->arg0)",
                out plan);
        }

        if (suffix == $"op_Equality:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorEquals(request->arg0, request->arg1)"
                : "request->arg0 == request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_Inequality:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"!{GetHelperNamespace()}::HalfOperatorEquals(request->arg0, request->arg1)"
                : "request->arg0 != request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_LessThan:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorLessThan(request->arg0, request->arg1)"
                : "request->arg0 < request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_LessThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorLessThanOrEqual(request->arg0, request->arg1)"
                : "request->arg0 <= request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_GreaterThan:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorGreaterThan(request->arg0, request->arg1)"
                : "request->arg0 > request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        if (suffix == $"op_GreaterThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})")
        {
            var expression = descriptor.HelperPrefix == "Half"
                ? $"{GetHelperNamespace()}::HalfOperatorGreaterThanOrEqual(request->arg0, request->arg1)"
                : "request->arg0 >= request->arg1";
            return TryCreateStaticBinaryPlan(descriptor, backend, "bool", expression, out plan);
        }

        var unaryHelperName = TryResolveFloatingUnaryHelperName(descriptor, suffix);
        if (unaryHelperName is null)
        {
            return false;
        }

        return TryCreateStaticUnaryPlan(
            descriptor,
            backend,
            "bool",
            $"{GetHelperNamespace()}::{unaryHelperName}(request->arg0)",
            out plan);
    }



    private static string? TryResolveFloatingUnaryHelperName(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix)
    {
        var managedTypeName = descriptor.ManagedTypeName;
        var helperPrefix = descriptor.HelperPrefix;
        return suffix switch
        {
            var current when current == $"IsEvenInteger:System.Boolean({managedTypeName})" => $"{helperPrefix}IsEvenInteger",
            var current when current == $"IsFinite:System.Boolean({managedTypeName})" => $"{helperPrefix}IsFinite",
            var current when current == $"IsInfinity:System.Boolean({managedTypeName})" => $"{helperPrefix}IsInfinity",
            var current when current == $"IsInteger:System.Boolean({managedTypeName})" => $"{helperPrefix}IsInteger",
            var current when current == $"IsNaN:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNaN",
            var current when current == $"IsNaNOrZero:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNaNOrZero",
            var current when current == $"IsNegative:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNegative",
            var current when current == $"IsNegativeInfinity:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNegativeInfinity",
            var current when current == $"IsNormal:System.Boolean({managedTypeName})" => $"{helperPrefix}IsNormal",
            var current when current == $"IsOddInteger:System.Boolean({managedTypeName})" => $"{helperPrefix}IsOddInteger",
            var current when current == $"IsPositive:System.Boolean({managedTypeName})" => $"{helperPrefix}IsPositive",
            var current when current == $"IsPositiveInfinity:System.Boolean({managedTypeName})" => $"{helperPrefix}IsPositiveInfinity",
            var current when current == $"IsPow2:System.Boolean({managedTypeName})" => $"{helperPrefix}IsPow2",
            var current when current == $"IsRealNumber:System.Boolean({managedTypeName})" => $"{helperPrefix}IsRealNumber",
            var current when current == $"IsSubnormal:System.Boolean({managedTypeName})" => $"{helperPrefix}IsSubnormal",
            var current when current == $"IsZero:System.Boolean({managedTypeName})" => $"{helperPrefix}IsZero",
            _ => null,
        };
    }



    private static bool TryCreateWideIntegerPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        var managedTypeName = descriptor.ManagedTypeName;

        if (suffix == $"CompareTo:System.Int32({managedTypeName})")
        {
            return TryCreateWideBinaryInstancePlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->this_arg, &request->arg0)",
                out plan);
        }

        if (suffix == $"Equals:System.Boolean({managedTypeName})")
        {
            return TryCreateWideBinaryInstancePlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(&request->this_arg, &request->arg0)",
                out plan);
        }

        return suffix switch
        {
            var current when current == $"op_Equality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(&request->arg0, &request->arg1)",
                    out plan),
            var current when current == $"op_Inequality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"!{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(&request->arg0, &request->arg1)",
                    out plan),
            var current when current == $"op_LessThan:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) < 0",
                    out plan),
            var current when current == $"op_LessThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) <= 0",
                    out plan),
            var current when current == $"op_GreaterThan:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) > 0",
                    out plan),
            var current when current == $"op_GreaterThanOrEqual:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateWideBinaryStaticPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(&request->arg0, &request->arg1) >= 0",
                    out plan),
            _ => false,
        };
    }



    private static bool TryCreatePointerIntegerPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        var managedTypeName = descriptor.ManagedTypeName;

        if (suffix == $"CompareTo:System.Int32({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->this_arg, request->arg0)",
                out plan);
        }

        if (suffix == $"Equals:System.Boolean({managedTypeName})")
        {
            return TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->this_arg, request->arg0)",
                out plan);
        }

        var lessToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_LessThan:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_LessThan:System.Boolean(System.UIntPtr,System.UIntPtr)";
        var lessOrEqualToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_LessThanOrEqual:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_LessThanOrEqual:System.Boolean(System.UIntPtr,System.UIntPtr)";
        var greaterToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_GreaterThan:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_GreaterThan:System.Boolean(System.UIntPtr,System.UIntPtr)";
        var greaterOrEqualToken = descriptor.ManagedTypeName == "System.IntPtr"
            ? "System.Numerics.IComparisonOperators<nint,nint,System.Boolean>.op_GreaterThanOrEqual:System.Boolean(System.IntPtr,System.IntPtr)"
            : "System.Numerics.IComparisonOperators<nuint,nuint,System.Boolean>.op_GreaterThanOrEqual:System.Boolean(System.UIntPtr,System.UIntPtr)";

        return suffix switch
        {
            var current when current == $"op_Equality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->arg0, request->arg1)",
                    out plan),
            var current when current == $"op_Inequality:System.Boolean({managedTypeName},{managedTypeName})" =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"!{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->arg0, request->arg1)",
                    out plan),
            var current when current == lessToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) < 0",
                    out plan),
            var current when current == lessOrEqualToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) <= 0",
                    out plan),
            var current when current == greaterToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) > 0",
                    out plan),
            var current when current == greaterOrEqualToken =>
                TryCreateStaticBinaryPlan(
                    descriptor,
                    backend,
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Compare(request->arg0, request->arg1) >= 0",
                    out plan),
            _ => false,
        };
    }



    private static bool TryCreateDateTimeLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "Compare:System.Int32(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1)",
                out plan),
            "CompareTo:System.Int32(System.DateTime)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.DateTime)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeEqualsTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Equality:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::DateTimeEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.DateTime,System.DateTime)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateTimeCompareTicks(&request->arg0, &request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }



    private static bool TryCreateTimeSpanLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "Compare:System.Int32(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1)",
                out plan),
            "CompareTo:System.Int32(System.TimeSpan)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.TimeSpan)" => TryCreatePointerBackedInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->this_arg, &request->arg0)",
                out plan),
            "Equals:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Equality:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::TimeSpanEqualsTicks(&request->arg0, &request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.TimeSpan,System.TimeSpan)" => TryCreatePointerBackedStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeSpanCompareTicks(&request->arg0, &request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }



    private static bool TryCreateDateOnlyLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "CompareTo:System.Int32(System.DateOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->this_arg, request->arg0)",
                out plan),
            "Equals:System.Boolean(System.DateOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyEqualsDayNumber(request->this_arg, request->arg0)",
                out plan),
            "op_Equality:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyEqualsDayNumber(request->arg0, request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::DateOnlyEqualsDayNumber(request->arg0, request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.DateOnly,System.DateOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::DateOnlyCompareDayNumber(request->arg0, request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }



    private static bool TryCreateTimeOnlyLikePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonKernelBackendKind backend,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            "CompareTo:System.Int32(System.TimeOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "CHAOS_IL2CPP_INT32",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->this_arg, request->arg0)",
                out plan),
            "Equals:System.Boolean(System.TimeOnly)" => TryCreateInstanceUnaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyEqualsTicksValue(request->this_arg, request->arg0)",
                out plan),
            "op_Equality:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyEqualsTicksValue(request->arg0, request->arg1)",
                out plan),
            "op_Inequality:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"!{GetHelperNamespace()}::TimeOnlyEqualsTicksValue(request->arg0, request->arg1)",
                out plan),
            "op_LessThan:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) < 0",
                out plan),
            "op_LessThanOrEqual:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) <= 0",
                out plan),
            "op_GreaterThan:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) > 0",
                out plan),
            "op_GreaterThanOrEqual:System.Boolean(System.TimeOnly,System.TimeOnly)" => TryCreateStaticBinaryPlan(
                descriptor,
                backend,
                "bool",
                $"{GetHelperNamespace()}::TimeOnlyCompareTicksValue(request->arg0, request->arg1) >= 0",
                out plan),
            _ => false,
        };
    }

}
