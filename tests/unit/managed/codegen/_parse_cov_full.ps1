$ErrorActionPreference = 'Stop'
$xmlFile = Get-ChildItem -Path TestResults -Recurse -Filter 'coverage.cobertura.xml' | Select-Object -First 1
$xml = [xml](Get-Content $xmlFile.FullName)
$results = @()
foreach($pkg in $xml.coverage.packages.package) {
    if($pkg.name -notlike '*Chaos.IL2CPP.Generator*') { continue }
    foreach($cls in $pkg.classes.class) {
        $total = 0; $covered = 0
        foreach($line in $cls.lines.line) {
            $total++
            if([int]$line.hits -gt 0) { $covered++ }
        }
        if($total -gt 0) {
            $rate = [math]::Round($covered / $total * 100, 2)
            $uncovered = $total - $covered
            $results += [PSCustomObject]@{ Rate = $rate; Covered = $covered; Total = $total; Uncovered = $uncovered; File = $cls.filename; Name = $cls.name }
        }
    }
}
Write-Host "=== Non-ReferenceProof files with < 50% coverage, sorted by uncovered lines (highest first) ==="
$results | Where-Object { $_.File -notlike '*ReferenceProof*' -and $_.Rate -lt 50 } | Sort-Object Uncovered -Descending | Select-Object -First 25 | Format-Table Rate, Uncovered, Total, @{N='File';E={Split-Path $_.File -Leaf}} -AutoSize
