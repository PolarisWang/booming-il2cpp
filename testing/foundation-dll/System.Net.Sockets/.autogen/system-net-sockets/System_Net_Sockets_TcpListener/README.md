# AutoTestGenerator: System.Net.Sockets.TcpListener

- Assembly: `System.Net.Sockets`
- Type: `System.Net.Sockets.TcpListener`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 17 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 14 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 15 |

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

- `Start` — contains generic parameters, needs manual handling
- `Start` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
