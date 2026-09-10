# AutoTestGenerator: System.ComponentModel.CustomTypeDescriptor

- Assembly: `System.ComponentModel.TypeConverter`
- Type: `System.ComponentModel.CustomTypeDescriptor`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 15 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 12 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 16 |

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
- `GetEventsFromRegisteredType` — contains generic parameters, needs manual handling
- `GetPropertiesFromRegisteredType` — contains generic parameters, needs manual handling
