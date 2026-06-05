# AutoTestGenerator: System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>

- Assembly: `System.Linq.Expressions`
- Type: `System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 13 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 12 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 21 |

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

- `CopyTo (ICollection non-generic leak: Int32[])` — contains generic parameters, needs manual handling
