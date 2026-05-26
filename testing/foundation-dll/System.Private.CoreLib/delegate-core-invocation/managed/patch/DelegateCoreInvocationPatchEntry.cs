// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/delegate/core/invocation
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class DelegateCoreInvocationPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Delegate::get_Target:System.Object()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)
    // [6] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)
    // [7] System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)
    // [9] System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)
    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: Subject_7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: CustomEntryMethod9(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}