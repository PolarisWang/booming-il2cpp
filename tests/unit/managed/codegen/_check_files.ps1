$ErrorActionPreference = 'Stop'
$xmlFile = Get-ChildItem -Path TestResults -Recurse -Filter 'coverage.cobertura.xml' | Select-Object -First 1
$xml = [xml](Get-Content $xmlFile.FullName)
foreach($pkg in $xml.coverage.packages.package) {
    if($pkg.name -notlike '*Chaos.IL2CPP.Generator*') { continue }
    foreach($cls in $pkg.classes.class) {
        if($cls.filename -like '*ManagedNaming*') {
            Write-Host "File: $($cls.filename)"
            Write-Host "Name: $($cls.name)"
            $total = 0; $covered = 0
            foreach($line in $cls.lines.line) {
                $total++
                if([int]$line.hits -gt 0) { $covered++ }
            }
            $rate = [math]::Round($covered / $total * 100, 2)
            Write-Host "Coverage: $rate% ($covered/$total)"
        }
        if($cls.filename -like '*CodeGenStage*') {
            Write-Host "File: $($cls.filename)"
            Write-Host "Name: $($cls.name)"
            $total = 0; $covered = 0
            foreach($line in $cls.lines.line) {
                $total++
                if([int]$line.hits -gt 0) { $covered++ }
            }
            $rate = [math]::Round($covered / $total * 100, 2)
            Write-Host "Coverage: $rate% ($covered/$total)"
        }
    }
}
