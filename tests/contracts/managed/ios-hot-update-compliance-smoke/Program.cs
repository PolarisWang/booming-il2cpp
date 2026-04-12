using System.Security.Cryptography;
using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

if (args.Length != 1)
{
    Console.Error.WriteLine("usage: IosHotUpdateComplianceSmoke <compliant|native-code|restricted-api|oversized>");
    return 2;
}

var scenario = args[0];
var packageRoot = Directory.CreateTempSubdirectory("ios-hot-update-compliance-smoke").FullName;

try
{
    CreatePackage(packageRoot, scenario);
    var package = PackageReader.ReadFromDirectory(packageRoot);
    PackageValidator.ValidateCompatibleTargetAotVersion(package, "1.0.0");
    PackageValidator.ValidateIosDistributionCompliance(
        package,
        new IosComplianceValidationOptions
        {
            MaxPackageBytes = scenario == "oversized" ? 32 : 2048,
        });

    if (scenario == "compliant")
    {
        Console.WriteLine("ios-compliance=pass");
        return 0;
    }

    Console.WriteLine("ios-compliance=unexpected-pass");
    return 3;
}
catch (InvalidOperationException exception)
{
    if (scenario == "compliant")
    {
        Console.Error.WriteLine(exception.Message);
        return 4;
    }

    Console.WriteLine("ios-compliance=reject");
    Console.WriteLine(exception.Message);
    return 0;
}
finally
{
    if (Directory.Exists(packageRoot))
    {
        Directory.Delete(packageRoot, recursive: true);
    }
}

static void CreatePackage(string packageRoot, string scenario)
{
    var assemblyBytes = scenario switch
    {
        "restricted-api" => "managed payload dlopen token",
        "oversized" => new string('A', 512),
        _ => "managed payload",
    };

    var assemblyPath = Path.Combine(packageRoot, "Patch.dll");
    var metadataPath = Path.Combine(packageRoot, "supplemental-metadata.json");

    File.WriteAllText(assemblyPath, assemblyBytes);
    File.WriteAllText(metadataPath, "{\"schemaVersion\":\"v0\"}");

    if (scenario == "native-code")
    {
        File.WriteAllText(Path.Combine(packageRoot, "payload.dylib"), "native payload");
    }

    var assemblyFile = new FileInfo(assemblyPath);
    var metadataFile = new FileInfo(metadataPath);

    var manifest = new
    {
        FormatVersion = "v0",
        PackageId = "ios-compliance-smoke",
        TargetAotVersion = "1.0.0",
        Assemblies = new[]
        {
            new
            {
                Name = assemblyFile.Name,
                Hash = ComputeHash(File.ReadAllBytes(assemblyPath)),
                Size = (int)assemblyFile.Length,
                EntryPoint = "Smoke/Entry::Run()",
            },
        },
        SupplementalMetadata = metadataFile.Name,
        Signature = "sig",
    };

    File.WriteAllText(
        Path.Combine(packageRoot, PackageReader.ManifestFileName),
        JsonSerializer.Serialize(manifest));
}

static string ComputeHash(byte[] bytes)
{
    return $"sha256:{Convert.ToHexString(SHA256.HashData(bytes)).ToLowerInvariant()}";
}
