using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;

namespace AutoBridgeProof;

internal static class Program
{
    private static int Main(string[] args)
    {
        var aotMethods = new[]
        {
            CreateMethod("AutoBridgeProof/Aot::Ping()", "Ping", "System.Void"),
            CreateMethod("AutoBridgeProof/Aot::AddOne(System.Int32)", "AddOne", "System.Int32", "System.Int32"),
            CreateMethod("AutoBridgeProof/Aot::Add(System.Int32,System.Int32)", "Add", "System.Int32", "System.Int32", "System.Int32"),
            CreateMethod("AutoBridgeProof/Aot::Prefix(System.String)", "Prefix", "System.String", "System.String"),
            CreateMethod("AutoBridgeProof/Aot::Echo(System.Object)", "Echo", "System.Object", "System.Object"),
        };
        var hotUpdateMethods = new[]
        {
            CreateMethod("AutoBridgeProof/Hot::Ping()", "Ping", "System.Void"),
            CreateMethod("AutoBridgeProof/Hot::AddOne(System.Int32)", "AddOne", "System.Int32", "System.Int32"),
            CreateMethod("AutoBridgeProof/Hot::Add(System.Int32,System.Int32)", "Add", "System.Int32", "System.Int32", "System.Int32"),
            CreateMethod("AutoBridgeProof/Hot::Prefix(System.String)", "Prefix", "System.String", "System.String"),
            CreateMethod("AutoBridgeProof/Hot::Echo(System.Object)", "Echo", "System.Object", "System.Object"),
        };

        var generator = new AutoBridgeGenerator();
        var plan = generator.Generate(aotMethods, hotUpdateMethods);
        var dispatcher = new BridgeDispatcher();
        dispatcher.ApplyPlan(plan);

        var voidInvoked = false;
        dispatcher.RegisterManagedTarget(hotUpdateMethods[0].SubjectId, _ =>
        {
            voidInvoked = true;
            return null;
        });
        dispatcher.RegisterManagedTarget(hotUpdateMethods[1].SubjectId, args => Convert.ToInt32(args[0]) + 1);
        dispatcher.RegisterManagedTarget(hotUpdateMethods[2].SubjectId, args => Convert.ToInt32(args[0]) + Convert.ToInt32(args[1]));
        dispatcher.RegisterManagedTarget(hotUpdateMethods[3].SubjectId, args => $"bridge-{args[0]}");
        dispatcher.RegisterManagedTarget(hotUpdateMethods[4].SubjectId, args => args[0]);

        _ = dispatcher.Dispatch(hotUpdateMethods[0].SubjectId, Array.Empty<object?>());
        Console.WriteLine($"auto-bridge-void={(voidInvoked ? "ok" : "unexpected")}");
        Console.WriteLine($"auto-bridge-int-unary={dispatcher.Dispatch(hotUpdateMethods[1].SubjectId, [41])}");
        Console.WriteLine($"auto-bridge-int-binary={dispatcher.Dispatch(hotUpdateMethods[2].SubjectId, [20, 22])}");
        Console.WriteLine($"auto-bridge-string-unary={dispatcher.Dispatch(hotUpdateMethods[3].SubjectId, ["chaos"])}");
        Console.WriteLine($"auto-bridge-object-unary={dispatcher.Dispatch(hotUpdateMethods[4].SubjectId, [new Payload("payload")])}");

        try
        {
            _ = generator.Generate(
                aotMethods,
                [CreateMethod("AutoBridgeProof/Hot::Bad(System.Int32,System.String)", "Bad", "System.Int32", "System.Int32", "System.String")]);
            Console.WriteLine("auto-bridge-unsupported=unexpected");
            return 1;
        }
        catch (NotSupportedException exception)
        {
            Console.WriteLine($"auto-bridge-unsupported={exception.Message}");
        }

        return 0;
    }

    private static ManagedMethodModel CreateMethod(
        string subjectId,
        string methodName,
        string returnType,
        params string[] parameterTypes)
    {
        var parameters = parameterTypes
            .Select((parameterType, index) => new ManagedParameterModel
            {
                Name = $"arg{index}",
                Type = parameterType,
            })
            .ToList();

        return new ManagedMethodModel
        {
            AssemblyName = "AutoBridgeProof",
            DeclaringTypeSubjectId = "AutoBridgeProof/Program",
            DeclaringTypeDisplayName = "Program",
            Name = methodName,
            ReturnType = returnType,
            SubjectId = subjectId,
            DefinitionSubjectId = subjectId,
            Signature = $"{returnType} Program::{methodName}({string.Join(",", parameterTypes)})",
            IsStatic = true,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = subjectId.GetHashCode(StringComparison.Ordinal),
            Parameters = parameters,
            Body = new ManagedMethodBodyModel
            {
                Blocks = [],
                ExceptionRegions = [],
            },
        };
    }

    private sealed record Payload(string Name)
    {
        public override string ToString()
        {
            return Name;
        }
    }
}
