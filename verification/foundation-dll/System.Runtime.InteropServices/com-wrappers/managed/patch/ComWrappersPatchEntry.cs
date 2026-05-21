// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/com/wrappers
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ComWrappersPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr&,System.IntPtr&,System.IntPtr&)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,System.Runtime.InteropServices.CreateComInterfaceFlags)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object,System.IntPtr)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(System.Runtime.InteropServices.ComWrappers)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

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
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}