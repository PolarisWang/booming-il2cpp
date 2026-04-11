using Chaos.IL2CPP.HotUpdate;

internal interface IService
{
    int Process(int value);
}

internal static class Program
{
    private const string ServiceBridgeId = "service-process";
    private const string MathBridgeId = "math-max";
    private const string EngineBridgeId = "engine-add-five";
    private const string DelegateWrapperId = "delegate-process";
    private const string HotServiceSubjectId = "BridgeRoundtripProof/HotService::Process(System.Int32)";
    private const string AotMathMaxSubjectId = "System.Math/System.Math::Max(System.Int32,System.Int32)";
    private const string EngineAddFiveSubjectId = "BridgeRoundtripProof/Engine::AddFive(System.Int32)";
    private const string ExpectedAotToHotUpdateOutput = "bridge-aot-to-hot-update=42";
    private const string ExpectedHotUpdateToAotOutput = "bridge-hot-update-to-aot=2";
    private const string ExpectedHotUpdateToEngineOutput = "bridge-hot-update-to-engine=7";
    private const string ExpectedDelegateRoundtripOutput = "delegate-roundtrip=42";

    private static int Main(string[] args)
    {
        var bridgeGenerator = new BridgeGenerator();
        var bridgePlan = bridgeGenerator.Generate(new BridgeGenerationRequest
        {
            AotToHotUpdate =
            [
                new AotToHotUpdateBridgeSpec
                {
                    BridgeId = ServiceBridgeId,
                    HotUpdateSubjectId = HotServiceSubjectId,
                },
            ],
            HotUpdateToAot =
            [
                new HotUpdateToAotBridgeSpec
                {
                    BridgeId = MathBridgeId,
                    AotSubjectId = AotMathMaxSubjectId,
                },
            ],
            HotUpdateToEngine =
            [
                new HotUpdateToEngineBridgeSpec
                {
                    BridgeId = EngineBridgeId,
                    EngineSubjectId = EngineAddFiveSubjectId,
                },
            ],
            DelegateWrappers =
            [
                new DelegateWrapperSpec
                {
                    WrapperId = DelegateWrapperId,
                    HotUpdateSubjectId = HotServiceSubjectId,
                },
            ],
        });

        var dispatcher = new BridgeDispatcher();
        dispatcher.ApplyPlan(bridgePlan);
        dispatcher.RegisterHotUpdateInt32UnaryTarget(HotServiceSubjectId, static value => value * 2);
        dispatcher.RegisterAotInt32BinaryTarget(AotMathMaxSubjectId, Math.Max);
        dispatcher.RegisterEngineInt32UnaryTarget(EngineAddFiveSubjectId, static value => value + 5);

        IService service = new BridgeBackedService(dispatcher);

        Console.WriteLine("BridgeRoundtripProof entry reached.");
        Console.WriteLine($"args={args.Length}");
        Console.WriteLine($"bridge-aot-to-hot-update={service.Process(21)}");
        Console.WriteLine($"bridge-hot-update-to-aot={dispatcher.InvokeHotUpdateToAotInt32(MathBridgeId, 1, 2)}");
        Console.WriteLine($"bridge-hot-update-to-engine={dispatcher.InvokeHotUpdateToEngineInt32(EngineBridgeId, 2)}");
        Console.WriteLine($"delegate-roundtrip={dispatcher.CreateAotDelegateWrapper(DelegateWrapperId)(21)}");
        return 0;
    }

    private sealed class BridgeBackedService : IService
    {
        private readonly BridgeDispatcher _dispatcher;

        public BridgeBackedService(BridgeDispatcher dispatcher)
        {
            _dispatcher = dispatcher;
        }

        public int Process(int value)
        {
            return _dispatcher.InvokeAotToHotUpdateInt32(ServiceBridgeId, value);
        }
    }
}
