using Chaos.IL2CPP.EngineBinding;

namespace Chaos.IL2CPP.EngineBinding.Smoke;

public interface SmokeHostExports
{
    [EngineExport("start")]
    void Start();

    [EngineCallback("on_reload")]
    void OnReload();
}
