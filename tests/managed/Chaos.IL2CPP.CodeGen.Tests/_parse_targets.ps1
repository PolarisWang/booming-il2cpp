$ErrorActionPreference = 'Stop'
$xmlFile = Get-ChildItem -Path TestResults -Recurse -Filter 'coverage.cobertura.xml' | Select-Object -First 1
$xml = [xml](Get-Content $xmlFile.FullName)
$results = @()
foreach($pkg in $xml.coverage.packages.package) {
    if($pkg.name -notlike '*Chaos.IL2CPP.CodeGen*') { continue }
    foreach($cls in $pkg.classes.class) {
        $total = 0; $covered = 0
        foreach($line in $cls.lines.line) {
            $total++
            if([int]$line.hits -gt 0) { $covered++ }
        }
        if($total -gt 0) {
            $rate = [math]::Round($covered / $total * 100, 2)
            $uncovered = $total - $covered
            $file = $cls.filename
            $shortName = Split-Path $file -Leaf
            # Show all non-ReferenceProof files sorted by potential gain (uncovered lines)
            if ($file -notlike '*ReferenceProof*' -and $uncovered -gt 0 -and $file -notlike '*g.cs' -and $file -notlike '*AssemblyInfo*') {
                $results += [PSCustomObject]@{ Rate = $rate; Covered = $covered; Total = $total; Uncovered = $uncovered; File = $shortName; FullPath = $file }
            }
        }
    }
}
# Focus on files where tests are easiest to add: Emission/ files and standalone utility files
Write-Host "=== Emission/ files sorted by uncovered lines ==="
$results | Where-Object { $_.FullPath -like '*Emission*' } | Sort-Object Uncovered -Descending | Format-Table Rate, Uncovered, Total, File -AutoSize

Write-Host "=== Non-Emission, non-ReferenceProof files with < 400 uncovered ==="
$results | Where-Object { $_.FullPath -notlike '*Emission*' -and $_.Uncovered -lt 400 } | Sort-Object Uncovered -Descending | Format-Table Rate, Uncovered, Total, File -AutoSize
