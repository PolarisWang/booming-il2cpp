# AutoTestGenerator: System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller

- Assembly: `System.Runtime.InteropServices`
- Type: `System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 1 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 1 |

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
