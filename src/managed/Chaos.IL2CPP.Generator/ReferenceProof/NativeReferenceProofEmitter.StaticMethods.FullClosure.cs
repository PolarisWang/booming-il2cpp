using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{



    private static void WriteGeneratedSource(
        string outputRootPath,
        NativeReferenceGeneratedSource generatedSource)
    {
        var targetPath = Path.Combine(outputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
        Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
        File.WriteAllText(targetPath, generatedSource.Contents);
    }





    private static int GetRuntimeSkeletonPageParallelism(int pageCount)
    {
        if (pageCount <= 1)
        {
            return 1;
        }

        var recommendedParallelism = Math.Max(1, Environment.ProcessorCount - 1);
        return Math.Min(pageCount, recommendedParallelism);
    }

}
