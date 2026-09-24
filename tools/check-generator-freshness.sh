#!/bin/bash
# Report whether the Generator that the chunk pipeline will actually run
# contains a given symbol, and compare it with the freshly-built Generator.
#
# WHY THIS EXISTS
#   The chunk pipeline does not run `dotnet .../Chaos.IL2CPP.Generator.dll`.
#   It runs the TPG, which carries its OWN COPY of the real Generator next to
#   it (copied via the Driver -> Generator ProjectReference). Editing Generator
#   source and rebuilding Generator alone therefore leaves the pipeline running
#   the OLD code, silently: the build proceeds, artifacts are written, and the
#   only symptom is that the change appears to have had no effect at all.
#
#   That has cost real time here more than once. This check is cheap; noticing
#   it after a 20-minute build is not. Run it BEFORE any build whose result
#   depends on a Generator change.
#
# USAGE
#   bash tools/check-generator-freshness.sh <SymbolName>
#   bash tools/check-generator-freshness.sh DefaultCharsPerPart
#
# Exit status: 0 = fresh, 1 = stale (rebuild TPG), 2 = cannot tell.
#
# WHY REFLECTION RATHER THAN `strings`
#   `strings` on a .NET assembly does not find method or parameter names — those
#   live in the metadata tables, not in the string heap. An earlier version of
#   this check used `strings` and reported a false negative for a symbol that
#   was demonstrably present.

set -u

if [ $# -lt 1 ]; then
    echo "usage: $0 <SymbolName>" >&2
    exit 2
fi

SYMBOL="$1"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DLL="$REPO_ROOT/src/managed/Chaos.IL2CPP.Generator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll"
TPG_DLL="$REPO_ROOT/src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll"

for f in "$SRC_DLL" "$TPG_DLL"; do
    if [ ! -f "$f" ]; then
        echo "MISSING: $f" >&2
        exit 2
    fi
done

PROBE_DIR="$(mktemp -d)"
PROBE_OUT="$PROBE_DIR/out"
trap 'rm -rf "$PROBE_DIR"' EXIT

cat > "$PROBE_DIR/p.csproj" <<'PROJ'
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <Nullable>disable</Nullable>
    <AssemblyName>probe</AssemblyName>
  </PropertyGroup>
</Project>
PROJ

cat > "$PROBE_DIR/Program.cs" <<'PROG'
using System;
using System.Linq;
using System.Reflection;

class Probe
{
    static int Main(string[] args)
    {
        var asm = Assembly.LoadFrom(args[0]);
        string symbol = args[1];
        const BindingFlags All = BindingFlags.Public | BindingFlags.NonPublic
                               | BindingFlags.Static | BindingFlags.Instance;

        foreach (var t in asm.GetTypes())
        {
            // A change can arrive as a type, a member, or a parameter name.
            if (t.Name == symbol || t.FullName == symbol)
            {
                Console.WriteLine("FOUND(type) " + t.FullName);
                return 0;
            }
            foreach (var m in t.GetMembers(All))
            {
                if (m.Name == symbol)
                {
                    Console.WriteLine("FOUND(member) " + t.FullName + "." + m.Name);
                    return 0;
                }
            }
            foreach (var m in t.GetMethods(All))
            {
                if (m.GetParameters().Any(p => p.Name == symbol))
                {
                    Console.WriteLine("FOUND(param) " + t.FullName + "." + m.Name);
                    return 0;
                }
            }
        }
        Console.WriteLine("ABSENT");
        return 1;
    }
}
PROG

dotnet build "$PROBE_DIR" -c Release -o "$PROBE_OUT" >/dev/null 2>&1

PROBE_DLL="$PROBE_OUT/probe.dll"
if [ ! -f "$PROBE_DLL" ]; then
    echo "probe failed to build; cannot determine freshness" >&2
    exit 2
fi

# One probe subprocess per DLL. Loading two assemblies that share a simple name
# in the SAME process makes the second LoadFrom fail (identity conflict), which
# showed up as an empty result and was then mis-read as "stale" — a false
# positive that would have sent someone chasing a non-existent problem.
probe() {
    dotnet "$PROBE_DLL" "$1" "$SYMBOL" 2>/dev/null | grep -E '^(FOUND|ABSENT)' | tail -1
}

src_result="$(probe "$SRC_DLL")"
tpg_result="$(probe "$TPG_DLL")"

echo "symbol : $SYMBOL"
echo "src    : $(stat -c %y "$SRC_DLL" | cut -d. -f1)  -> ${src_result:-<probe failed>}"
echo "tpg    : $(stat -c %y "$TPG_DLL" | cut -d. -f1)  -> ${tpg_result:-<probe failed>}"

# No usable answer from the BUNDLED copy means we cannot conclude anything —
# reporting STALE there would be a guess.
if [ -z "$tpg_result" ]; then
    echo "VERDICT: cannot tell — the probe produced no result for the bundled copy." >&2
    exit 2
fi

if [ "$tpg_result" = "ABSENT" ]; then
    echo "VERDICT: STALE — the pipeline will NOT run your change." >&2
    echo "         Rebuild the TPG:" >&2
    echo "           dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Chaos.IL2CPP.Tools.TestProjectGenerator.csproj -c Debug" >&2
    exit 1
fi

echo "VERDICT: fresh — the pipeline will run your change."
exit 0
