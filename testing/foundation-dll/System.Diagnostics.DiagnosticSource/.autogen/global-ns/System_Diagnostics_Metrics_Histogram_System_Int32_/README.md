# AutoTestGenerator: System.Diagnostics.Metrics.Histogram<System.Int32>

- Assembly: `System.Diagnostics.DiagnosticSource`
- Type: `System.Diagnostics.Metrics.Histogram<System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 6 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 14 |

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

- `Record (ref struct parameter: System.ReadOnlySpan<System.Collections.Generic.KeyValuePair<System.String,System.Object>>)` — contains generic parameters, needs manual handling
