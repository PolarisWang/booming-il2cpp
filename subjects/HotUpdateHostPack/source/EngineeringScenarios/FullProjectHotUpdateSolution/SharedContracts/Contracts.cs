namespace GoldenHotUpdate.SharedContracts;

public interface IHostMath
{
    int Add(int left, int right);

    string Decorate(string value);
}

public interface IPatchFeature
{
    int GetPatchedValue();

    string ComposeMessage(IHostMath hostMath, string name);

    T Echo<T>(T value);

    int Apply(Func<int, int> callback, int value);

    void ThrowPatchFault();
}
