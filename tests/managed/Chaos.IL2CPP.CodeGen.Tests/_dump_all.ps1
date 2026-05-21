$ErrorActionPreference = 'Stop'
$xmlFile = Get-ChildItem -Path TestResults -Recurse -Filter 'coverage.cobertura.xml' | Select-Object -First 1
$xml = [xml](Get-Content $xmlFile.FullName)
foreach($pkg in $xml.coverage.packages.package) {
    if($pkg.name -notlike '*Chaos.IL2CPP.CodeGen*') { continue }
    Write-Host "=== $($pkg.name) ==="
    foreach($cls in $pkg.classes.class) {
        $file = $cls.filename
        $short = Split-Path $file -Leaf
        $total = 0; $covered = 0
        foreach($line in $cls.lines.line) { $total++; if([int]$line.hits -gt 0) { $covered++ } }
        if($total -gt 0) {
            $rate = [math]::Round($covered / $total * 100, 2)
        } else { $rate = "N/A" }
        Write-Host "  $rate%  $short  ($file)"
    }
}
