# AutoTestGenerator: System.Security.Principal.IdentityReferenceCollection

- Assembly: `System.Security.Principal.Windows`
- Type: `System.Security.Principal.IdentityReferenceCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 8 |

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

- `CopyTo (ICollection non-generic leak: IdentityReference[])` — contains generic parameters, needs manual handling
