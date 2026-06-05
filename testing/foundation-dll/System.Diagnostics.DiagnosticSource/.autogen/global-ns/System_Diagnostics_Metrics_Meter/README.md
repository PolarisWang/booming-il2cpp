# AutoTestGenerator: System.Diagnostics.Metrics.Meter

- Assembly: `System.Diagnostics.DiagnosticSource`
- Type: `System.Diagnostics.Metrics.Meter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 29 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 29 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 114 |

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
