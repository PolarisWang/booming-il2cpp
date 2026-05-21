namespace RuntimeSelfTest;

/// <summary>
/// Array operations proof — exercises array creation, indexing,
/// and iteration through the il2cpp codegen pipeline.
/// </summary>
public class ArrayOperations
{
    public int[] CreateArray(int length)
    {
        return new int[length];
    }

    public int CreateAndSum(int count)
    {
        var arr = new int[count];
        for (int i = 0; i < count; i++)
        {
            arr[i] = i + 1;
        }

        int sum = 0;
        for (int i = 0; i < arr.Length; i++)
        {
            sum += arr[i];
        }
        return sum;
    }
}
