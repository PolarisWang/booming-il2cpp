# AutoTestGenerator: System.ComponentModel.Design.DesignerVerbCollection

- Assembly: `System.ComponentModel.TypeConverter`
- Type: `System.ComponentModel.Design.DesignerVerbCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 7 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 10 |

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

- `CopyTo (ICollection non-generic leak: DesignerVerb[])` — contains generic parameters, needs manual handling
