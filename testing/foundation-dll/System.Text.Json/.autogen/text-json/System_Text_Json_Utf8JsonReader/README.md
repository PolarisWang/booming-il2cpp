# AutoTestGenerator: System.Text.Json.Utf8JsonReader

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.Utf8JsonReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 41 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 37 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 4 |
| Total Subjects | 51 |

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

- `ValueTextEquals (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ValueTextEquals (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `CopyString (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `CopyString (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
