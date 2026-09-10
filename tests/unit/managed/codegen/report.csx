using System.Xml.Linq;

var path = args.Length > 0 ? args[0] : "coverage_report.xml";
var xml = XDocument.Load(path);
var ns = xml.Root!.GetDefaultNamespace();

var module = xml.Descendants(ns + "module")
    .FirstOrDefault(m => m.Attribute("name")?.Value == "Chaos.IL2CPP.Generator.dll");
if (module == null) { Console.Error.WriteLine("Module not found"); return; }

var srcMap = new Dictionary<string, string>();
foreach (var sf in module.Descendants(ns + "source_file"))
    srcMap[sf.Attribute("id")!.Value] = sf.Attribute("path")!.Value;

var stats = new Dictionary<string, (int covered, int partial, int total)>();
foreach (var func in module.Descendants(ns + "function"))
foreach (var range in func.Descendants(ns + "range"))
{
    var sid = range.Attribute("source_id")?.Value;
    if (sid == null || !srcMap.ContainsKey(sid)) continue;
    var path2 = srcMap[sid];
    if (!path2.Contains("CodeGen\\")) continue;
    var s = stats.GetValueOrDefault(path2);
    s.total++;
    var cov = range.Attribute("covered")?.Value;
    if (cov == "yes") s.covered++;
    else if (cov == "partial") s.partial++;
    stats[path2] = s;
}

var prefix = "D:\\agent\\booming-il2cpp\\src\\managed\\Chaos.IL2CPP.Generator\\";
foreach (var kv in stats.OrderBy(k => k.Key))
{
    var (covered, partial, total) = kv.Value;
    var pct = (covered + partial * 0.5) / total * 100;
    var shortName = kv.Key.StartsWith(prefix) ? kv.Key[prefix.Length..] : kv.Key;
    Console.WriteLine($"{pct,6:F1}%  {covered,5} yes + {partial,3} par / {total,5} blk  {shortName}");
}
