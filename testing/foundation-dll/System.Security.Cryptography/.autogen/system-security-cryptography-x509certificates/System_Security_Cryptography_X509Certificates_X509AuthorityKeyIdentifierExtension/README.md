# AutoTestGenerator: System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.X509AuthorityKeyIdentifierExtension`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 9 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 4 |
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

- `CreateFromSubjectKeyIdentifier (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `CreateFromIssuerNameAndSerialNumber (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
