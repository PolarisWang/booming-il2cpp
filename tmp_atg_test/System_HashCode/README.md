# AutoTestGenerator: System.HashCode

- Assembly: `System.Private.CoreLib`
- Type: `System.HashCode`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 12 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 9 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 35 |

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

- `AddBytes (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
