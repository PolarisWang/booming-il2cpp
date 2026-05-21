$ErrorActionPreference = 'Stop'
$xmlFile = Get-ChildItem -Path TestResults -Recurse -Filter 'coverage.cobertura.xml' | Select-Object -First 1
if (-not $xmlFile) { Write-Host "No coverage file found"; exit 1 }
$xml = [xml](Get-Content $xmlFile.FullName)
$totalCovered = 0; $totalLines = 0
foreach($pkg in $xml.coverage.packages.package) {
    if($pkg.name -like '*Chaos.IL2CPP.Generator*' -and $pkg.name -notlike '*Tests*') {
        $pkgCovered = 0; $pkgLines = 0
        foreach($cls in $pkg.classes.class) {
            foreach($line in $cls.lines.line) {
                $pkgLines++
                if([int]$line.hits -gt 0) { $pkgCovered++ }
            }
        }
        if($pkgLines -gt 0) {
            $rate = [math]::Round($pkgCovered / $pkgLines * 100, 2)
            Write-Host "$($pkg.name): $rate% ($pkgCovered/$pkgLines)"
            $totalCovered += $pkgCovered; $totalLines += $pkgLines
        }
    }
}
if($totalLines -gt 0) {
    $totalRate = [math]::Round($totalCovered / $totalLines * 100, 2)
    Write-Host "---"
    Write-Host "CodeGen Total: $totalRate% ($totalCovered/$totalLines)"
}
