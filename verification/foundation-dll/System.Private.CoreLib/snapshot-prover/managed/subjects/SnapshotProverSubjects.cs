// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/snapshot/prover
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class SnapshotProverSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] SnapshotTestFixtures/ArithmeticOps::RunAdd:System.Int32()
    public static void Subject_0()
    {
        if (ArithmeticOps.RunAdd() != ArithmeticOps.RunAdd()) _exitCode = 1;
    }

    // [1] SnapshotTestFixtures/VoidCaller::DoNothing:System.Int32()
    public static void Subject_1()
    {
        if (VoidCaller.DoNothing() != VoidCaller.DoNothing()) _exitCode = 1;
    }

    // [2] SnapshotTestFixtures/MathHelper::RunSquare:System.Int32()
    public static void Subject_2()
    {
        if (MathHelper.RunSquare() != MathHelper.RunSquare()) _exitCode = 1;
    }

    // [3] SnapshotTestFixtures/FlowControl::IsPositive:System.Int32()
    public static void Subject_3()
    {
        if (FlowControl.IsPositive() != FlowControl.IsPositive()) _exitCode = 1;
    }

    // [4] SnapshotTestFixtures/CallVirtHelper::CreateAndUse:System.Int32()
    public static void Subject_4()
    {
        if (CallVirtHelper.CreateAndUse() != CallVirtHelper.CreateAndUse()) _exitCode = 1;
    }

    // [5] SnapshotTestFixtures/BoxingHelper::BoxAndUnbox:System.Int32()
    public static void Subject_5()
    {
        if (BoxingHelper.BoxAndUnbox() != BoxingHelper.BoxAndUnbox()) _exitCode = 1;
    }

    // [6] SnapshotTestFixtures/ConversionHelper::ConvertToInt:System.Int32()
    public static void Subject_6()
    {
        if (ConversionHelper.ConvertToInt() != ConversionHelper.ConvertToInt()) _exitCode = 1;
    }

    // [7] SnapshotTestFixtures/LoopHelper::SumToFive:System.Int32()
    public static void Subject_7()
    {
        if (LoopHelper.SumToFive() != LoopHelper.SumToFive()) _exitCode = 1;
    }

    // [8] SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()
    public static void Subject_8()
    {
        if (LdftnHelper.GetFnPtr() != LdftnHelper.GetFnPtr()) _exitCode = 1;
    }

    // [9] SnapshotTestFixtures/GenericsVirtEhDemo::DemoCombine:System.Int32()
    public static void Subject_9()
    {
        if (GenericsVirtEhDemo.DemoCombine() != GenericsVirtEhDemo.DemoCombine()) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
        }
    }

}