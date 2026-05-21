using System.Xml.Linq;

var path = args.Length > 0 ? args[0] : "coverage_analysis.xml";
var xml = XDocument.Load(path);

var module = xml.Descendants("module")
    .FirstOrDefault(m => m.Attribute("name")?.Value == "Chaos.IL2CPP.Generator.dll");
if (module == null) { Console.Error.WriteLine("Module not found"); return; }

var srcMap = new Dictionary<string, string>();
foreach (var sf in module.Descendants("source_file"))
    srcMap[sf.Attribute("id")!.Value] = sf.Attribute("path")!.Value;

var stats = new Dictionary<string, CovStat>();
foreach (var func in module.Descendants("function"))
foreach (var range in func.Descendants("range"))
{
    var sid = range.Attribute("source_id")?.Value;
    if (sid == null || !srcMap.ContainsKey(sid)) continue;
    var p = srcMap[sid];
    if (!p.Contains("CodeGen\\") || p.Contains("obj\\")) continue;

    if (!stats.ContainsKey(p)) stats[p] = new CovStat();
    var s = stats[p];
    s.Total++;
    var cov = range.Attribute("covered")?.Value;
    if (cov == "yes") s.Covered++;
    else if (cov == "partial") s.Partial++;
}

var prefix = "D:\\agent\\booming-il2cpp\\src\\managed\\Chaos.IL2CPP.Generator\\";
int grandTotal = 0, grandCovered = 0;
double grandPartial = 0;
int refProofTotal = 0, refProofCovered = 0;
double refProofPartial = 0;
int coreTotal = 0, coreCovered = 0;
double corePartial = 0;

var refProofPrefixes = new[] { "ReferenceProof", "NativeReferenceProof", "RuntimeSkeleton",
    "GenericCapabilityMatrix", "NativeAotEmitter", "PatchData", "Pluginization",
    "BuildSystem", "CodeGenRuntimeProviderCatalog", "NativeReferenceLoweringPlanner" };

bool IsRefProof(string p)
{
    var name = Path.GetFileNameWithoutExtension(p);
    foreach (var x in refProofPrefixes)
        if (name.StartsWith(x) || p.Contains("\\" + x + "\\") || p.Contains("/" + x + "/"))
            return true;
    return false;
}

Console.WriteLine("=== FILES BELOW 90% ===");
foreach (var kv in stats.OrderBy(k => k.Key))
{
    var pct = (kv.Value.Covered + kv.Value.Partial * 0.5) / kv.Value.Total * 100;
    var shortName = kv.Key.StartsWith(prefix) ? kv.Key[prefix.Length..] : kv.Key;
    var marker = IsRefProof(kv.Key) ? " [RP]" : "";
    if (pct < 90)
        Console.WriteLine($"{pct,6:F1}%  {kv.Value.Covered,5} yes + {kv.Value.Partial,3} par / {kv.Value.Total,5} blk{marker}  {shortName}");

    grandTotal += kv.Value.Total;
    grandCovered += kv.Value.Covered;
    grandPartial += kv.Value.Partial * 0.5;
    if (IsRefProof(kv.Key))
    {
        refProofTotal += kv.Value.Total;
        refProofCovered += kv.Value.Covered;
        refProofPartial += kv.Value.Partial * 0.5;
    }
    else
    {
        coreTotal += kv.Value.Total;
        coreCovered += kv.Value.Covered;
        corePartial += kv.Value.Partial * 0.5;
    }
}

Console.WriteLine($"\n=== CORE (excl. ReferenceProof) ===");
var corePct = coreTotal > 0 ? (coreCovered + corePartial) / coreTotal * 100 : 0;
Console.WriteLine($"  Total: {coreCovered + corePartial:F0} / {coreTotal} blocks = {corePct:F1}%");
Console.WriteLine($"  Need {(90 * coreTotal / 100.0 - coreCovered - corePartial):F0} more blocks for 90%");

Console.WriteLine($"\n=== REFERENCE PROOF ===");
var rpPct = refProofTotal > 0 ? (refProofCovered + refProofPartial) / refProofTotal * 100 : 0;
Console.WriteLine($"  Total: {refProofCovered + refProofPartial:F0} / {refProofTotal} blocks = {rpPct:F1}%");

Console.WriteLine($"\n=== GRAND TOTAL ===");
var grandPct = grandTotal > 0 ? (grandCovered + grandPartial) / grandTotal * 100 : 0;
Console.WriteLine($"  Total: {grandCovered + grandPartial:F0} / {grandTotal} blocks = {grandPct:F1}%");

record CovStat { public int Covered; public int Partial; public int Total; }
