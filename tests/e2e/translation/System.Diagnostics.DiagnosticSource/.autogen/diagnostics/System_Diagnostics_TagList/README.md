# AutoTestGenerator: System.Diagnostics.TagList

- Assembly: `System.Diagnostics.DiagnosticSource`
- Type: `System.Diagnostics.TagList`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 11 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 10 |
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

- `CopyTo (ref struct parameter: System.Span<System.Collections.Generic.KeyValuePair<System.String,System.Object>>)` — contains generic parameters, needs manual handling
