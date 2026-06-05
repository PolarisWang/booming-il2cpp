# AutoTestGenerator: System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder

- Assembly: `System.Reflection.Metadata`
- Type: `System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 4 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 4 |

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

- `AddMethodBody (ref struct return type: MethodBodyStreamEncoder.MethodBody)` — contains generic parameters, needs manual handling
- `AddMethodBody (ref struct return type: MethodBodyStreamEncoder.MethodBody)` — contains generic parameters, needs manual handling
