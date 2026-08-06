$ErrorActionPreference = 'Stop'
$xmlFile = Get-ChildItem -Path TestResults -Recurse -Filter 'coverage.cobertura.xml' | Select-Object -First 1
$xml = [xml](Get-Content $xmlFile.FullName)
foreach($pkg in $xml.coverage.packages.package) {
    foreach($cls in $pkg.classes.class) {
        if($cls.filename -like '*ManagedNaming*' -or $cls.name -like '*ManagedNaming*') {
            $total = 0; $covered = 0
            foreach($line in $cls.lines.line) { $total++; if([int]$line.hits -gt 0) { $covered++ } }
            $rate = [math]::Round($covered / $total * 100, 2)
            Write-Host "$($pkg.name) / $($cls.name): $rate% ($covered/$total)"
            Write-Host "  File: $($cls.filename)"
        }
    }
}
Write-Host "---"
Write-Host "Also looking for Naming files..."
foreach($pkg in $xml.coverage.packages.package) {
    foreach($cls in $pkg.classes.class) {
        if($cls.filename -like '*Naming*') {
            $total = 0; $covered = 0
            foreach($line in $cls.lines.line) { $total++; if([int]$line.hits -gt 0) { $covered++ } }
            $rate = [math]::Round($covered / $total * 100, 2)
            Write-Host "$($pkg.name) / $($cls.name): $rate% ($covered/$total)"
            Write-Host "  File: $($cls.filename)"
        }
    }
}
