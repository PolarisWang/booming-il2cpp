# AutoTestGenerator: System.Delegate

- Assembly: `System.Private.CoreLib`
- Type: `System.Delegate`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 20 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 17 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 38 |

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

- `Combine (ref struct parameter: System.ReadOnlySpan<System.Delegate>)` — contains generic parameters, needs manual handling
- `EnumerateInvocationList (ref struct return type: System.Delegate.InvocationListEnumerator<System.Delegate>)` — contains generic parameters, needs manual handling
- `GetObjectData` — contains generic parameters, needs manual handling
