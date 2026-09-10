# Chaos IL2CPP Native API Documentation

This directory contains the Doxygen-based API reference for the Chaos IL2CPP native runtime.

## Regenerate

```bash
# Install doxygen (if not present):
#   Ubuntu: sudo apt install doxygen
#   macOS:  brew install doxygen
#   Windows: choco install doxygen.install

# Generate:
doxygen docs/native-api/Doxyfile

# Open:
open docs/native-api/build/html/index.html
```

## Scope

| Phase | Input | Status |
|-------|-------|--------|
| P0    | contracts/native/v0/ | ✅ Active |
| P1    | src/native/common/chaos/ | ✅ Active |
| P2    | src/native/runtime-core/ | ⏳ Optional |

## CI

Generated on push to `main` by `.github/workflows/docs.yml` and published to GitHub Pages.