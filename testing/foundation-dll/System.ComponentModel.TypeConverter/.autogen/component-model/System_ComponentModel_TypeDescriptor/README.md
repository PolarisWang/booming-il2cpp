# AutoTestGenerator: System.ComponentModel.TypeDescriptor

- Assembly: `System.ComponentModel.TypeConverter`
- Type: `System.ComponentModel.TypeDescriptor`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 69 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 64 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 104 |

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

- `GetConverterFromRegisteredType` — contains generic parameters, needs manual handling
- `GetConverterFromRegisteredType` — contains generic parameters, needs manual handling
- `GetEventsFromRegisteredType` — contains generic parameters, needs manual handling
- `GetPropertiesFromRegisteredType` — contains generic parameters, needs manual handling
- `GetPropertiesFromRegisteredType` — contains generic parameters, needs manual handling
