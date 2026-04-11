namespace Chaos.IL2CPP.EngineBinding;

[AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = false)]
public sealed class EngineCallbackAttribute(string callbackName) : Attribute
{
    public string CallbackName { get; } = string.IsNullOrWhiteSpace(callbackName)
        ? throw new ArgumentException("callbackName must not be empty.", nameof(callbackName))
        : callbackName;
}
