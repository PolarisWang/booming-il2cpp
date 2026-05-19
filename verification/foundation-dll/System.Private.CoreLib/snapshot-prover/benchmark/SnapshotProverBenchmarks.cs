using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/SnapshotTestFixtures/snapshot/prover
public static class SnapshotProverBenchmarks
{
    // Benchmark ArithmeticOps.RunAdd native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/ArithmeticOps::RunAdd:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_ArithmeticOps_RunAdd_System_Int32() { _ = ArithmeticOps.RunAdd(); }
    // Benchmark VoidCaller.DoNothing native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/VoidCaller::DoNothing:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_VoidCaller_DoNothing_System_Int32() { _ = VoidCaller.DoNothing(); }
    // Benchmark MathHelper.RunSquare native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/MathHelper::RunSquare:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_MathHelper_RunSquare_System_Int32() { _ = MathHelper.RunSquare(); }
    // Benchmark FlowControl.IsPositive native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/FlowControl::IsPositive:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_FlowControl_IsPositive_System_Int32() { _ = FlowControl.IsPositive(); }
    // Benchmark CallVirtHelper.CreateAndUse native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/CallVirtHelper::CreateAndUse:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_CallVirtHelper_CreateAndUse_System_Int32() { _ = CallVirtHelper.CreateAndUse(); }
    // Benchmark BoxingHelper.BoxAndUnbox native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/BoxingHelper::BoxAndUnbox:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_BoxingHelper_BoxAndUnbox_System_Int32() { _ = BoxingHelper.BoxAndUnbox(); }
    // Benchmark ConversionHelper.ConvertToInt native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/ConversionHelper::ConvertToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_ConversionHelper_ConvertToInt_System_Int32() { _ = ConversionHelper.ConvertToInt(); }
    // Benchmark LoopHelper.SumToFive native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/LoopHelper::SumToFive:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_LoopHelper_SumToFive_System_Int32() { _ = LoopHelper.SumToFive(); }
    // Benchmark LdftnHelper.GetFnPtr native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_LdftnHelper_GetFnPtr_System_Int32() { _ = LdftnHelper.GetFnPtr(); }
    // Benchmark GenericsVirtEhDemo.DemoCombine native-runtime throughput
    [BenchmarkSubjectId("SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SnapshotTestFixtures_SnapshotProver)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SnapshotTestFixtures_GenericsVirtEhDemo_DemoCombine_System_Int32() { _ = GenericsVirtEhDemo.DemoCombine(); }}
