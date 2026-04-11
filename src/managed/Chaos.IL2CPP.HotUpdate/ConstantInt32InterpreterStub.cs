namespace Chaos.IL2CPP.HotUpdate;

public sealed class ConstantInt32InterpreterStub
{
    private readonly int _constantValue;

    public ConstantInt32InterpreterStub(int constantValue)
    {
        _constantValue = constantValue;
    }

    public int Execute()
    {
        return _constantValue;
    }
}
