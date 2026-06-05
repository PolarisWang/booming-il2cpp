# AutoTestGenerator: System.Diagnostics.ActivityTagsCollection

- Assembly: `System.Diagnostics.DiagnosticSource`
- Type: `System.Diagnostics.ActivityTagsCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 9 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 16 |

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

- `GetEnumerator (ref struct return type: ActivityTagsCollection.Enumerator)` — contains generic parameters, needs manual handling
