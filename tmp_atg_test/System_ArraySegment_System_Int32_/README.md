# AutoTestGenerator: System.ArraySegment<System.Int32>

- Assembly: `System.Private.CoreLib`
- Type: `System.ArraySegment<System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 11 |

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

- `GetEnumerator (ref struct return type: ArraySegment<System.Int32>.Enumerator)` — contains generic parameters, needs manual handling
- `CopyTo (ICollection non-generic leak: Int32[])` — contains generic parameters, needs manual handling
