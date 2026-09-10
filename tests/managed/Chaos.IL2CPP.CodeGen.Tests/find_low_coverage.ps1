Add-Type -AssemblyName System.Xml.Linq
$doc = [System.Xml.Linq.XDocument]::Load('coverage.cobertura.xml')

# Aggregate by file
$fileStats = @{}
foreach ($c in $doc.Descendants('class')) {
    $filename = $c.Attribute('filename').Value
    if ($filename -notlike '*CodeGen*' -or $filename -like '*ReferenceProof*' -or $filename -like '*PatchData*' -or $filename -like '*obj/*' -or $filename -like '*Generated*') {
        continue
    }
    $lines = $c.Descendants('line')
    $covered = 0
    $total = 0
    foreach ($l in $lines) {
        $hits = [int]($l.Attribute('hits').Value)
        $total++
        if ($hits -gt 0) { $covered++ }
    }
    if (-not $fileStats.ContainsKey($filename)) {
        $fileStats[$filename] = @{ Covered = 0; Total = 0 }
    }
    $fileStats[$filename].Covered += $covered
    $fileStats[$filename].Total += $total
}

$items = @()
foreach ($f in $fileStats.Keys) {
    $s = $fileStats[$f]
    $rate = if ($s.Total -gt 0) { [double]$s.Covered / $s.Total } else { 1.0 }
    $uncovered = $s.Total - $s.Covered
    if ($uncovered -gt 0) {
        $shortName = $f -replace '.*\\', ''
        $items += [PSCustomObject]@{ File = $shortName; Covered = $s.Covered; Total = $s.Total; Uncovered = $uncovered; Rate = $rate }
    }
}

# Sort by most uncovered lines first
Write-Host "=== Top files by uncovered lines ==="
$items | Sort-Object Uncovered -Descending | Select-Object -First 15 | Format-Table -AutoSize -Property File, Covered, Total, Uncovered, @{N='Rate';E={'{0:P1}' -f $_.Rate}}

# Also show files that can be pushed to 90% quickly (100-400 total lines, under 50% coverage, not massive files)
Write-Host "`n=== Medium files (100-500 lines) under 50% coverage ==="
$items | Where-Object { $_.Total -ge 100 -and $_.Total -le 500 -and $_.Rate -lt 0.50 } | Sort-Object Rate | Format-Table -AutoSize -Property File, Covered, Total, Uncovered, @{N='Rate';E={'{0:P1}' -f $_.Rate}}
