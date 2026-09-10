# Chaos IL2CPP HelloWorld Sample

This minimal C# application demonstrates the chaos-il2cpp publishing pipeline,
showing how a plain .NET console app compiles to a native executable via AOT.

## Prerequisites

- Windows x64
- Visual Studio 2022 (with C++ workload for `cmake` + `MSVC`)
- .NET 10 SDK (`dotnet --version` >= `10.0.100-rc.2`)
- chaos-il2cpp Driver (built)

```bash
# Build the Driver and Generator from the repo
dotnet build src/managed/Chaos.IL2CPP.Generator/ --configuration Release
dotnet build src/managed/Chaos.IL2CPP.Driver/ --configuration Release
```

## Build and run

```bash
# 1. Publish the sample as a native executable
dotnet src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll \
  publish samples/HelloWorld/HelloWorld.csproj \
  --mode app --config-tier check --clean --output ./hello-output

# 2. Run the native app
./hello-output/chaos_entry.exe
```

Expected output:

```
Hello from chaos-il2cpp!
args.Length=0
```

### Optional: if the Driver is installed as a dotnet global tool

```bash
dotnet tool install --global chaos-il2cpp
chaos-il2cpp publish HelloWorld.csproj --mode app --output ./build
```

## Files

| File | Purpose |
|------|---------|
| `HelloWorld.csproj` | Empty .NET console app project (net10.0) |
| `Program.cs` | `static int Main` with `Console.WriteLine` |
| `README.md` | This file |

## Known limitations

- **Entry-point invocation**: `chaos-il2cpp publish` currently emits `app_main.cpp`
  with a no-op entry call when the entry point cannot be resolved to a SubjectId.
  `ConvertService.DetectEntryPoint` resolves it via `Assembly.LoadFrom`, which fails
  when the target's TFM differs from the Driver's (`net8.0`) — the PE-metadata
  fallback deliberately returns null rather than a fabricated SubjectId.  The
  resulting `chaos_entry.exe` boots the runtime and GC correctly and exits 0, but
  does not invoke `Main`, so the `Console.WriteLine` output above is not printed.
  Tracked as a Driver/codegen defect, not a sample defect.
- **async Task Main**: The async state machine's `AwaitUnsafeOnCompleted` and
  `Start` declarations are emitted into `native-aot.generated.header.h` as
  `extern "C" ... ) {;` (an unmatched `{` inside a namespace scope), which fails
  MSVC compilation with C2598/C2601/C1075.  This is why the sample uses a
  synchronous `Main`.