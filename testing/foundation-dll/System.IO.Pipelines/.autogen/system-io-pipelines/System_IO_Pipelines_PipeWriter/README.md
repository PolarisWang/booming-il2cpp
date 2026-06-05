# AutoTestGenerator: System.IO.Pipelines.PipeWriter

- Assembly: `System.IO.Pipelines`
- Type: `System.IO.Pipelines.PipeWriter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 11 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 4 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 13 |

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

- `GetSpan (ref struct return type: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
