// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/snapshot/prover
// Assembly: System.Private.CoreLib
// Variant: subjects

using SnapshotTestFixtures;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class SnapshotProverSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] SnapshotTestFixtures/ArithmeticOps::RunAdd:System.Int32()
    public static void Subject_0()
    {
        try { if (ArithmeticOps.RunAdd() != ArithmeticOps.RunAdd()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] SnapshotTestFixtures/VoidCaller::DoNothing:System.Int32()
    public static void Subject_1()
    {
        try { if (VoidCaller.DoNothing() != VoidCaller.DoNothing()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] SnapshotTestFixtures/MathHelper::RunSquare:System.Int32()
    public static void Subject_2()
    {
        try { if (MathHelper.RunSquare() != MathHelper.RunSquare()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] SnapshotTestFixtures/FlowControl::IsPositive:System.Int32()
    public static void Subject_3()
    {
        try { if (FlowControl.IsPositive() != FlowControl.IsPositive()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] SnapshotTestFixtures/CallVirtHelper::CreateAndUse:System.Int32()
    public static void Subject_4()
    {
        try { if (CallVirtHelper.CreateAndUse() != CallVirtHelper.CreateAndUse()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] SnapshotTestFixtures/BoxingHelper::BoxAndUnbox:System.Int32()
    public static void Subject_5()
    {
        try { if (BoxingHelper.BoxAndUnbox() != BoxingHelper.BoxAndUnbox()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] SnapshotTestFixtures/ConversionHelper::ConvertToInt:System.Int32()
    public static void Subject_6()
    {
        try { if (ConversionHelper.ConvertToInt() != ConversionHelper.ConvertToInt()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] SnapshotTestFixtures/LoopHelper::SumToFive:System.Int32()
    public static void Subject_7()
    {
        try { if (LoopHelper.SumToFive() != LoopHelper.SumToFive()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()
    public static void CustomEntrySubject_8()
    {
    }

    // [9] SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine:System.Int32()
    public static void Subject_9()
    {
        try { if (GenericsVirtEhDemo.DemoCombine() != GenericsVirtEhDemo.DemoCombine()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}