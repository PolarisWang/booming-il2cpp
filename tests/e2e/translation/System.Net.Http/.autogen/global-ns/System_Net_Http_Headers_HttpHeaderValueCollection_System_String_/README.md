# AutoTestGenerator: System.Net.Http.Headers.HttpHeaderValueCollection<System.String>

- Assembly: `System.Net.Http`
- Type: `System.Net.Http.Headers.HttpHeaderValueCollection<System.String>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 7 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 12 |

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

- `CopyTo (ICollection non-generic leak: String[])` — contains generic parameters, needs manual handling
