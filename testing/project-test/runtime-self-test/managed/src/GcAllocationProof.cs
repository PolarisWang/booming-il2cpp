namespace RuntimeSelfTest;

/// <summary>
/// GC allocation proof — exercises managed object allocation
/// through the il2cpp GC interface (GcAllocate / GcAllocateAtomic).
/// </summary>
public class GcAllocationProof
{
    public int Value { get; set; }

    public int AllocateAndReturn(int input)
    {
        var obj = new GcAllocationProof { Value = input };
        return obj.Value;
    }

    public int AllocateAtomic(int input)
    {
        // Allocate and immediately use the value
        var arr = new int[1];
        arr[0] = input;
        return arr[0];
    }
}
