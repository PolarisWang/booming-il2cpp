Add-Type -AssemblyName System.Xml.Linq
$doc = [System.Xml.Linq.XDocument]::Load('coverage.cobertura.xml')
$classes = $doc.Descendants('class') | Where-Object { $_.Attribute('filename').Value -like '*AssemblyFullClosureAuditEmitter*' }
foreach ($c in $classes) {
    $name = $c.Attribute('name').Value
    $rate = [double]($c.Attribute('line-rate').Value)
    Write-Host ("{0,-60} {1:P1}" -f $name, $rate)
}
