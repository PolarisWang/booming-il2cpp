namespace Chaos.IL2CPP.Generator;

[RuntimeProviderPlugin("valuetype-kernel", CodeGenPluginProductLine.NativeReference, "kernel", 1, 100)]
internal sealed class ValueTypeKernelRuntimeProviderPlugin : IRuntimeProviderPlugin
{
}

[RuntimeProviderPlugin("marshal-platform", CodeGenPluginProductLine.NativeReference, "platform-capability", 1, 110)]
internal sealed class MarshalPlatformRuntimeProviderPlugin : IRuntimeProviderPlugin
{
}

[RuntimeProviderPlugin("task-runtime-kernel", CodeGenPluginProductLine.NativeReference, "task-runtime-kernel", 1, 120)]
internal sealed class TaskRuntimeKernelRuntimeProviderPlugin : IRuntimeProviderPlugin
{
}

[RuntimeProviderPlugin("interop-kernel32-platform", CodeGenPluginProductLine.NativeReference, "platform-capability", 1, 130)]
internal sealed class InteropKernel32RuntimeProviderPlugin : IRuntimeProviderPlugin
{
}

[RuntimeProviderPlugin("native-aot-external-runtime-helpers", CodeGenPluginProductLine.NativeAot, "external-runtime-helper", 1, 100)]
internal sealed class NativeAotExternalRuntimeHelperProviderPlugin : IRuntimeProviderPlugin
{
}
