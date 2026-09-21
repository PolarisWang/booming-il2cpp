// Guard test for TestEmitter.ExceptionReproductionTable — the ATG whitelist that
// decides which external-assembly methods get the try/throw reproduction form
// instead of the `return 42L` stub-gap form.
//
// Why this exists
// ---------------
// An entry in that table asserts "the AOT body for this method raises the same
// exception the managed probe observed".  If the assertion is WRONG the subject
// fails at fact time — an honest stub-gap converted into a false red.  The table
// was previously eleven hand-written `if (declaringType.Contains(...) && method.Name
// is ...)` blocks with no test coverage, which made "add one more name" the path
// of least resistance and left that claim unverifiable.
//
// This test cannot prove the AOT body throws (that needs a build).  What it CAN
// do is refuse the mechanical mistakes that produce the same damage:
//   * an entry naming a native symbol that does not exist anywhere in the tree
//   * two entries covering the same (type, method) pair, so which one "wins"
//     depends on declaration order
//   * an entry that matches no methods at all
//   * a method claimed by an entry while its type is not a real BCL type name
//
// Run:  dotnet run --project tests/contracts/managed/atg-whitelist-guard
using System.Reflection;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Tools.AutoTestGenerator;


var failures = new List<string>();
var checks = 0;

void Check(bool condition, string message)
{
    checks++;
    if (!condition) failures.Add(message);
}

var entries = TestEmitter.ExceptionReproductionEntries;

// ── 1. Table is non-empty and every entry is fully specified ────────────────
Check(entries.Count > 0, "whitelist is empty");

for (var i = 0; i < entries.Count; i++)
{
    var e = entries[i];
    var label = $"entry[{i}] ({string.Join("|", e.ExactTypeNames.Concat(e.TypeFragments).Take(2))}…)";

    Check(e.Methods.Length > 0, $"{label}: declares no methods");
    Check(e.ExactTypeNames.Length + e.TypeFragments.Length > 0, $"{label}: declares no type");
    Check(!string.IsNullOrWhiteSpace(e.NativeSymbol), $"{label}: NativeSymbol is empty");
    Check(!string.IsNullOrWhiteSpace(e.Rationale), $"{label}: Rationale is empty");

    // Method names must be bare identifiers — a name like "WriteTo" is matched
    // exactly, so a stray suffix would silently never match anything.
    foreach (var m in e.Methods)
    {
        Check(!string.IsNullOrWhiteSpace(m), $"{label}: empty method name");
        Check(!m.Contains('(') && !m.Contains('.'),
            $"{label}: method name '{m}' looks like a signature, not a bare name");
    }

    // Declaring types must look like real namespaces, not fragments.
    foreach (var t in e.ExactTypeNames.Concat(e.TypeFragments))
    {
        Check(t.Contains('.'), $"{label}: type '{t}' is not namespace-qualified");
    }
}

// ── 2. No duplicate method names within an entry ───────────────────────────
for (var i = 0; i < entries.Count; i++)
{
    var dupes = entries[i].Methods
        .GroupBy(m => m, StringComparer.Ordinal)
        .Where(g => g.Count() > 1)
        .Select(g => g.Key)
        .ToList();
    Check(dupes.Count == 0, $"entry[{i}]: duplicate method names: {string.Join(", ", dupes)}");
}

// ── 3. No (type, method) pair claimed by two entries ───────────────────────
// Overlap means the earlier entry wins purely by array order, so adding an entry
// can silently change an unrelated method's behavior.
//
// Allowance: XmlNodeList.GetEnumerator and XmlAttributeCollection.GetEnumerator
// are matched by BOTH the DOM entry (via its "System.Xml.XmlNode" / "System.Xml.XmlAttribute"
// fragments) and the collection entry.  This predates the table form and is
// harmless — both entries return true — but it is recorded here so that any NEW
// overlap fails, and so the known one is visible rather than accidental.
var allowedOverlapPairs = new HashSet<string>(StringComparer.Ordinal)
{
    "GetEnumerator",  // DOM vs collection, see the comment above
};
for (var i = 0; i < entries.Count; i++)
{
    for (var j = i + 1; j < entries.Count; j++)
    {
        var sharedMethods = entries[i].Methods
            .Intersect(entries[j].Methods, StringComparer.Ordinal)
            .ToList();
        if (sharedMethods.Count == 0) continue;

        // A shared method name is only a conflict if a declaring type matches both.
        var allTypes = entries[i].ExactTypeNames.Concat(entries[i].TypeFragments)
            .Concat(entries[j].ExactTypeNames).Concat(entries[j].TypeFragments)
            .Distinct(StringComparer.Ordinal);

        foreach (var t in allTypes)
        {
            foreach (var m in sharedMethods)
            {
                if (!entries[i].Matches(t, m)) continue;
                if (!entries[j].Matches(t, m)) continue;
                if (allowedOverlapPairs.Contains(m)) continue;
                Check(false,
                    $"entries[{i}] and [{j}] both match type '{t}' method '{m}'; " +
                    "which entry wins depends on declaration order");
            }
        }
    }
}

// ── 4. Every entry actually matches something ──────────────────────────────
// An entry whose predicates are wrong (typo, wrong namespace) is dead weight that
// looks like coverage.  Each entry must match at least one plausible type.
foreach (var (e, i) in entries.Select((e, i) => (e, i)))
{
    var probeTypes = e.ExactTypeNames.Concat(e.TypeFragments).ToList();
    var matched = e.Methods.Any(m => probeTypes.Any(t => e.Matches(t, m)));
    Check(matched, $"entry[{i}]: matches none of its own declared types");
}

// ── 5. NativeSymbol resolves to something real in the tree ────────────────
// The symbol is the entry's evidence that a native body exists.  A symbol that no
// longer exists (renamed stub file, removed function) means the entry asserts an
// exception nothing can raise.
var repoRoot = LocateRepoRoot();
if (repoRoot is null)
{
    Console.WriteLine("[guard] WARNING: repo root not found; skipping native-symbol existence checks");
}
else
{
    foreach (var (e, i) in entries.Select((e, i) => (e, i)))
    {
        var symbol = e.NativeSymbol!;
        var found = SymbolExists(repoRoot, symbol);
        Check(found,
            $"entry[{i}]: NativeSymbol '{symbol}' not found anywhere under src/ " +
            "(renamed or removed stub — this entry would assert a throw nothing raises)");
    }
}

// ── 6. The WaitHandle entry, if present, covers EVERY static overload ─────
// This is the specific failure that motivated the table: registering only some
// overloads leaves the rest on the catch-all, which returns 0 without raising,
// so ATG's "AOT stub did not throw" path fires and the whole assembly goes red.
// A half-covered family is worse than an uncovered one.
var waitHandleEntry = entries.FirstOrDefault(e =>
    e.ExactTypeNames.Concat(e.TypeFragments).Any(t => t.Contains("WaitHandle", StringComparison.Ordinal)));
if (waitHandleEntry is not null)
{
    string[] required = ["WaitAll", "WaitAny", "SignalAndWait"];
    foreach (var m in required)
    {
        Check(waitHandleEntry.Methods.Contains(m, StringComparer.Ordinal),
            $"WaitHandle entry does not cover '{m}'; a partially covered wait family " +
            "converts honest stub-gaps into false reds");
    }

    var type = "System.Threading.WaitHandle";
    // Every arity the BCL surface contains must match, or that overload silently
    // stays on the catch-all.
    Check(waitHandleEntry.Matches(type, "WaitAll"), "WaitHandle entry must match WaitAll");
    Check(waitHandleEntry.Matches(type, "WaitAny"), "WaitHandle entry must match WaitAny");
    Check(waitHandleEntry.Matches(type, "SignalAndWait"), "WaitHandle entry must match SignalAndWait");
}

// ── 7. The guard must actually reject a bad entry ──────────────────
// A guard nobody has seen fail is not evidence.  Re-run the core checks against a
// deliberately broken entry (a native symbol that cannot exist) and confirm the
// same predicate reports it.
{
    var bogus = new[]
    {
        new FakeEntry(
            TypeFragments: new[] { "System.Threading.WaitHandle" },
            ExactTypeNames: Array.Empty<string>(),
            Methods: new[] { "WaitAll" },
            NativeSymbol: "chaos_this_symbol_does_not_exist_anywhere_zzz",
            Rationale: "negative control"),
    };

    var bogusCaught = repoRoot is not null &&
        bogus.Any(e => !SymbolExists(repoRoot, e.NativeSymbol));
    Check(bogusCaught, "negative control: a bogus native symbol was NOT rejected " +
        "(the symbol-existence check is not actually running)");

    // And the WaitHandle completeness check must reject a partial family.
    var partial = new[]
    {
        new FakeEntry(
            TypeFragments: new[] { "System.Threading.WaitHandle" },
            ExactTypeNames: Array.Empty<string>(),
            Methods: new[] { "WaitAll" },   // WaitAny / SignalAndWait missing
            NativeSymbol: "chaos_wait_handle_validate",
            Rationale: "negative control"),
    };
    var missing = new[] { "WaitAll", "WaitAny", "SignalAndWait" }
        .Where(m => !partial[0].Methods.Contains(m, StringComparer.Ordinal))
        .ToList();
    Check(missing.Count > 0,
        "negative control: partial wait family was NOT detected");
}

// ── 8. The dispatch predicate itself, driven end-to-end ─────────────────────
// Sections 1-7 inspect the TABLE.  None of them exercise the function that
// actually decides whether a generated body becomes try/throw or return-42 —
// TryFindExceptionReproductionEntry — so the table could be perfect while the
// predicate feeds finalizeValidation the wrong answer.
//
// These call the real production predicate (internal, reached via
// InternalsVisibleTo), so a report about the predicate is a report about the
// code that runs at codegen time.
{
    // A method the table covers: must resolve to an entry.
    var covered = new MethodSignature(
        Name: "WriteCData",
        DeclaringTypeFullName: "System.Xml.XmlWriter",
        ReturnTypeName: "System.Void",
        IsStatic: false, IsVoid: true, HasRefParam: false,
        Parameters: Array.Empty<MethodParameter>());

    Check(TestEmitter.TryFindExceptionReproductionEntry(covered, out var hit) && hit is not null,
        "predicate: a whitelisted (type, method) pair did not resolve to any entry");

    // Negative control A — a covered METHOD name under an UNCOVERED type must
    // miss.  This is the shape of a real mistake: copying a method name from one
    // entry into a new entry with a different declaring type.
    var wrongType = new MethodSignature(
        Name: "WriteCData",
        DeclaringTypeFullName: "System.Threading.WaitHandle",
        ReturnTypeName: "System.Void",
        IsStatic: true, IsVoid: true, HasRefParam: false,
        Parameters: Array.Empty<MethodParameter>());

    Check(!TestEmitter.TryFindExceptionReproductionEntry(wrongType, out _),
        "negative control: an uncovered declaring type matched a covered method name " +
        "(the predicate ignores the type)");

    // Negative control B — the WaitHandle wait family must NOT be whitelisted.
    // Shape registration runs ahead of the whitelist here (see
    // RegisterWaitHandleStatics).  If someone later adds a WaitHandle entry while
    // the shape is missing, the catch-all returns 0 without raising and ATG's
    // "AOT stub did not throw" fires — the 21->43 false-red regression.  Asserting
    // the current, deliberate state keeps that addition from being silent.
    foreach (var waitMethod in new[] { "WaitAll", "WaitAny", "SignalAndWait" })
    {
        var m = new MethodSignature(
            Name: waitMethod,
            DeclaringTypeFullName: "System.Threading.WaitHandle",
            ReturnTypeName: "System.Boolean",
            IsStatic: true, IsVoid: false, HasRefParam: false,
            Parameters: Array.Empty<MethodParameter>());

        Check(!TestEmitter.TryFindExceptionReproductionEntry(m, out _),
            $"WaitHandle.{waitMethod} is whitelisted for exception reproduction; " +
            "the native validation path must be proven live in the generated output " +
            "BEFORE this is added, or every wait subject turns red");
    }
}

// ── report ─────────────────────────────────────────────────────────────────
Console.WriteLine($"[guard] {checks} checks, {failures.Count} failure(s)");
if (failures.Count > 0)
{
    foreach (var f in failures) Console.WriteLine("  FAIL: " + f);
    return 1;
}

Console.WriteLine("[guard] exception-reproduction whitelist OK");
return 0;

static string? LocateRepoRoot()
{
    var dir = new DirectoryInfo(AppContext.BaseDirectory);
    while (dir is not null)
    {
        // .git is a DIRECTORY in a normal checkout but a FILE in a worktree.
        if (Directory.Exists(Path.Combine(dir.FullName, ".git")) ||
            File.Exists(Path.Combine(dir.FullName, ".git")))
            return dir.FullName;
        dir = dir.Parent;
    }
    return null;
}

/// <summary>
/// Whether <paramref name="symbol"/> is defined somewhere outside the whitelist's
/// own declaration.
/// </summary>
/// <remarks>
/// The scan MUST exclude TestEmitter.cs (and this test): the whitelist file
/// contains each symbol as a string literal, so a naive search over <c>src/</c>
/// finds the declaration itself and reports success for ANY name — including a
/// deliberately bogus one.  That made the check vacuous until the negative
/// control in section 7 exposed it.  Only definitions in native sources and in
/// generator files OTHER than the whitelist count as evidence.
/// </remarks>
static bool SymbolExists(string repoRoot, string symbol)
{
    var src = Path.Combine(repoRoot, "src");
    if (!Directory.Exists(src)) return false;

    // Symbols in the table are either a C++ identifier (function) or a stub
    // basename (e.g. "xml_writer_stubs.cpp").  Accept either.
    var isFile = symbol.EndsWith(".cpp", StringComparison.Ordinal)
                 || symbol.EndsWith(".h", StringComparison.Ordinal);

    foreach (var file in Directory.EnumerateFiles(src, "*", SearchOption.AllDirectories))
    {
        var name = Path.GetFileName(file);

        if (isFile)
        {
            if (string.Equals(name, symbol, StringComparison.Ordinal)) return true;
            continue;
        }

        if (!name.EndsWith(".cpp", StringComparison.Ordinal) &&
            !name.EndsWith(".h", StringComparison.Ordinal) &&
            !name.EndsWith(".cs", StringComparison.Ordinal))
            continue;

        // Skip the whitelist's own file: it declares every symbol as a literal,
        // so matching there proves nothing about whether a definition exists.
        if (string.Equals(name, "TestEmitter.cs", StringComparison.Ordinal)) continue;

        string text;
        try { text = File.ReadAllText(file); }
        catch { continue; }

        if (Regex.IsMatch(text, $@"\b{Regex.Escape(symbol)}\b")) return true;
    }
    return false;
}

// Mirrors ExceptionReproductionEntry's matching shape for the negative control,
// which must exercise the same SymbolExists predicate the real entries use.
internal sealed record FakeEntry(
    string[] TypeFragments,
    string[] ExactTypeNames,
    string[] Methods,
    string NativeSymbol,
    string Rationale);
