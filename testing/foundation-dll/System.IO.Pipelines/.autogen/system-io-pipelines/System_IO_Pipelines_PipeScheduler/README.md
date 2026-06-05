# AutoTestGenerator: System.IO.Pipelines.PipeScheduler

- Assembly: `System.IO.Pipelines`
- Type: `System.IO.Pipelines.PipeScheduler`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 1 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 2 |

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
