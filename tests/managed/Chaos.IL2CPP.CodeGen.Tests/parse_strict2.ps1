Add-Type -AssemblyName System.Xml.Linq
$doc = [System.Xml.Linq.XDocument]::Load('strict_coverage.xml')
$sources = @{}
$doc.Descendants('source') | ForEach-Object { $sources[$_.Attribute('id').Value] = $_.Attribute('path').Value }
$mod = $doc.Descendants('module') | Where-Object { $_.Attribute('name').Value -eq 'Chaos.IL2CPP.Generator.dll' } | Select-Object -First 1
$fileStats = @{}
foreach ($fn in $mod.Descendants('function')) {
    foreach ($r in $fn.Descendants('range')) {
        $sid = $r.Attribute('source_id').Value
        $path = $sources[$sid]
        if ($path -notlike '*CodeGen*') { continue }
        if (-not $fileStats.ContainsKey($path)) { $fileStats[$path] = @{covered=0; partial=0; uncovered=0} }
        $cov = $r.Attribute('covered').Value
        if ($cov -eq 'yes') { $fileStats[$path].covered++ }
        elseif ($cov -eq 'partially') { $fileStats[$path].partial++ }
        else { $fileStats[$path].uncovered++ }
    }
}
$sorted = $fileStats.GetEnumerator() | Sort-Object { $_.Value.uncovered } -Descending | Select-Object -First 40
foreach ($e in $sorted) {
    $v = $e.Value; $total = $v.covered + $v.partial + $v.uncovered
    $rate = if ($total -gt 0) { ($v.covered/$total).ToString('P2') } else { 'N/A' }
    Write-Host ("$($v.uncovered.ToString('D6')) uncovered, $($total.ToString('D6')) total, $rate  $($e.Name)")
}
