#!/usr/bin/env bash
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO_ROOT"

echo "=== Chaos IL2CPP Test Framework ==="
echo ""

# L1 + L2 + L4: C# tests
echo "--- C# Tests (dotnet test) ---"
dotnet test src/managed/ --verbosity normal 2>&1 | tail -5

# L3: C++ tests
echo ""
echo "--- C++ Tests (cmake + ctest) ---"
BUILD_DIR="build/native-testing"
cmake -S src/native -B "$BUILD_DIR" -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTS=ON
cmake --build "$BUILD_DIR" --config RelWithDebInfo
ctest --test-dir "$BUILD_DIR" --output-on-failure --config RelWithDebInfo

# L5 + L6 + L7: Python tests
echo ""
echo "--- Python Tests (pytest) ---"
python -m pytest src/python/ -v --tb=short 2>&1 | tail -5

echo ""
echo "=== All tests completed ==="
