// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/snapshot/prover
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using SnapshotTestFixtures;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class SnapshotProverSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] SnapshotTestFixtures/ArithmeticOps::RunAdd:System.Int32()
    public static void Subject_0()
    {
        try { _ = ArithmeticOps.RunAdd(); }
        catch { _exitCode = 1; }
    }

    // [1] SnapshotTestFixtures/VoidCaller::DoNothing:System.Int32()
    public static void Subject_1()
    {
        try { _ = VoidCaller.DoNothing(); }
        catch { _exitCode = 1; }
    }

    // [2] SnapshotTestFixtures/MathHelper::RunSquare:System.Int32()
    public static void Subject_2()
    {
        try { _ = MathHelper.RunSquare(); }
        catch { _exitCode = 1; }
    }

    // [3] SnapshotTestFixtures/FlowControl::IsPositive:System.Int32()
    public static void Subject_3()
    {
        try { _ = FlowControl.IsPositive(); }
        catch { _exitCode = 1; }
    }

    // [4] SnapshotTestFixtures/CallVirtHelper::CreateAndUse:System.Int32()
    public static void Subject_4()
    {
        try { _ = CallVirtHelper.CreateAndUse(); }
        catch { _exitCode = 1; }
    }

    // [5] SnapshotTestFixtures/BoxingHelper::BoxAndUnbox:System.Int32()
    public static void Subject_5()
    {
        try { _ = BoxingHelper.BoxAndUnbox(); }
        catch { _exitCode = 1; }
    }

    // [6] SnapshotTestFixtures/ConversionHelper::ConvertToInt:System.Int32()
    public static void Subject_6()
    {
        try { _ = ConversionHelper.ConvertToInt(); }
        catch { _exitCode = 1; }
    }

    // [7] SnapshotTestFixtures/LoopHelper::SumToFive:System.Int32()
    public static void Subject_7()
    {
        try { _ = LoopHelper.SumToFive(); }
        catch { _exitCode = 1; }
    }

    // [8] SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()
    // [9] SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine:System.Int32()
    public static void Subject_9()
    {
        try { _ = GenericsVirtEhDemo.DemoCombine(); }
        catch { _exitCode = 1; }
    }

}