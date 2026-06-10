# AutoTestGenerator: System.IO.Compression.BrotliStream

- Assembly: `System.IO.Compression.Brotli`
- Type: `System.IO.Compression.BrotliStream`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 19 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 13 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 45 |

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

- `Write (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Read (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
