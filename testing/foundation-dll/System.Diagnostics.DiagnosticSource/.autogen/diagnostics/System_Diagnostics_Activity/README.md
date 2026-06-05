# AutoTestGenerator: System.Diagnostics.Activity

- Assembly: `System.Diagnostics.DiagnosticSource`
- Type: `System.Diagnostics.Activity`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 24 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 19 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 33 |

## Build

```bash
# Build with verification (Assert enabled)
dotnet build -p:DefineConstants=VERIFY

# Build for benchmark (Assert eliminated)
dotnet build
```

## Run

```bash
# Run with verification
dotnet run --project .
```

## Skipped Methods

- `EnumerateTagObjects (ref struct return type: System.Diagnostics.Activity.Enumerator<System.Collections.Generic.KeyValuePair<System.String,System.Object>>)` — contains generic parameters, needs manual handling
- `EnumerateEvents (ref struct return type: System.Diagnostics.Activity.Enumerator<System.Diagnostics.ActivityEvent>)` — contains generic parameters, needs manual handling
- `EnumerateLinks (ref struct return type: System.Diagnostics.Activity.Enumerator<System.Diagnostics.ActivityLink>)` — contains generic parameters, needs manual handling
- `Start` — contains generic parameters, needs manual handling
- `Dispose (enumerator leak - target has nested Enumerator type)` — contains generic parameters, needs manual handling
