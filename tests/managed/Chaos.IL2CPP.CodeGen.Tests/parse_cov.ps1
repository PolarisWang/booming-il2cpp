$ErrorActionPreference = 'Stop'
$xml = [xml](Get-Content "$PSScriptRoot\TestResults\coverlet_coverage.cobertura.xml")
$results = @()
foreach($pkg in $xml.coverage.packages.package) {
    foreach($cls in $pkg.classes.class) {
        $total = 0; $covered = 0
        foreach($line in $cls.lines.line) {
            $total++
            if([int]$line.hits -gt 0) { $covered++ }
        }
        if($total -gt 0) {
            $rate = [math]::Round($covered / $total * 100, 2)
            $results += [PSCustomObject]@{ Rate = $rate; Covered = $covered; Total = $total; File = $cls.filename; Name = $cls.name }
        }
    }
}
Write-Host "=== Non-ReferenceProof files with < 50% coverage, total >= 200 ==="
$results | Where-Object { $_.Total -ge 200 -and $_.File -notlike '*ReferenceProof*' } | Sort-Object Rate | Select-Object -First 30 | Format-Table Rate, Covered, Total, File -AutoSize
Write-Host "=== Non-ReferenceProof files with coverage >= 90%, total >= 200 ==="
$results | Where-Object { $_.Total -ge 200 -and $_.Rate -ge 90 -and $_.File -notlike '*ReferenceProof*' } | Sort-Object Rate | Format-Table Rate, Covered, Total, File -AutoSize
