namespace RuntimeSelfTest;

/// <summary>
/// String operations proof — exercises string allocation, formatting,
/// and concatenation through the il2cpp codegen pipeline.
/// </summary>
public class StringOperations
{
    public string FormatString(string prefix, int number)
    {
        return prefix + number.ToString();
    }

    public string ConcatStrings(string a, string b)
    {
        return a + b;
    }
}
