using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Interlocked::Increment
        /// </summary>
        private static void RegisterInterlockedIncrement(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "Increment", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedIncrementInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // Int32 / UInt32 / UInt64 overloads.  Registering only the Int64
            // form left these callees matching no shape, so they lowered to the
            // zero-argument external-runtime catch-all and returned 0 —
            // `Assert.AreEqual(1, Interlocked.Increment(ref x))` then failed
            // with a wrong value.  The callee shape is the same `T(T&)` the
            // Int64 entry uses; only the carrier width differs.
            RegisterInterlockedUnaryArithmetic(registry, "Increment",
                "System.Int32&", "ChaosInterlockedIncrementInt32", AotCoreIrAbiCarrierKind.Int32);
            RegisterInterlockedUnaryArithmetic(registry, "Increment",
                "System.UInt32&", "ChaosInterlockedIncrementUInt32", AotCoreIrAbiCarrierKind.UInt32);
            RegisterInterlockedUnaryArithmetic(registry, "Increment",
                "System.UInt64&", "ChaosInterlockedIncrementUInt64", AotCoreIrAbiCarrierKind.UInt64);
        }

        /// <summary>
        /// Interlocked::Decrement
        /// </summary>
        private static void RegisterInterlockedDecrement(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "Decrement", ["System.Int64&"],
                ShapeKind.SimpleForward, "ChaosInterlockedDecrementInt64",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

            // See RegisterInterlockedIncrement — same int-width gap, Decrement.
            RegisterInterlockedUnaryArithmetic(registry, "Decrement",
                "System.Int32&", "ChaosInterlockedDecrementInt32", AotCoreIrAbiCarrierKind.Int32);
            RegisterInterlockedUnaryArithmetic(registry, "Decrement",
                "System.UInt32&", "ChaosInterlockedDecrementUInt32", AotCoreIrAbiCarrierKind.UInt32);
            RegisterInterlockedUnaryArithmetic(registry, "Decrement",
                "System.UInt64&", "ChaosInterlockedDecrementUInt64", AotCoreIrAbiCarrierKind.UInt64);
        }

        /// <summary>
        /// `Interlocked.&lt;op&gt;(ref T)` for a non-Int64 carrier — one by-ref
        /// parameter in, one value out, both the same width.
        ///
        /// Kept as a helper rather than six hand-written blocks because the
        /// registered entries must stay shape-identical apart from the carrier:
        /// a copy-paste slip here silently re-creates the very bug this fixes
        /// (a callee that matches no shape and falls to the catch-all), and
        /// that failure is invisible in the registry — it only shows up as a
        /// wrong answer at runtime.
        /// </summary>
        private static void RegisterInterlockedUnaryArithmetic(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string paramType,
            string nativeSymbol,
            AotCoreIrAbiCarrierKind carrier)
        {
            registry.Register("Interlocked", methodName, [paramType],
                ShapeKind.SimpleForward, nativeSymbol,
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = carrier,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// Interlocked::Exchange
        /// </summary>
        private static void RegisterInterlockedExchange(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "Exchange", ["System.Int64&", "System.Int64"],
                ShapeKind.SimpleForward, "ChaosInterlockedExchangeInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Interlocked::CompareExchange (Int32)
        /// </summary>
        private static void RegisterInterlockedCompareExchange(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "CompareExchange", ["System.Int32&", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosInterlockedCompareExchangeInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Interlocked::CompareExchange (Int64)
        /// </summary>
        private static void RegisterInterlockedCompareExchange_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Interlocked", "CompareExchange", ["System.Int64&", "System.Int64", "System.Int64"],
                ShapeKind.SimpleForward, "ChaosInterlockedCompareExchangeInt64",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                },
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// `Interlocked.CompareExchange&lt;T&gt;(ref T, T, T)` — explicit generic
        /// instantiations.
        /// </summary>
        /// <remarks>
        /// The exact-shape registrations above cover the NON-generic spelling
        /// (`Interlocked::CompareExchange:(System.Int32&,System.Int32,System.Int32)`).
        /// An explicitly instantiated call lowers with the type argument baked
        /// into the method name —
        /// `Interlocked::CompareExchange&lt;System.Int32&gt;:System.Int32(...)` —
        /// which `TryMatchShape` keys on verbatim, so it never matched the
        /// non-generic entry and fell through to the zero-argument
        /// external-runtime catch-all (returning 0).  `TryMatchGenericShape`
        /// handles the angle-bracket spelling and is consulted first, so the fix
        /// is a descriptor here rather than a change to the matching layer.
        ///
        /// The type argument is what selects the native: dispatching on
        /// `typeArgs[0]` keeps `&lt;System.Int32&gt;` and `&lt;System.Int64&gt;` on
        /// their own carriers.  Matching on the method name alone would collapse
        /// them onto one symbol.
        /// </remarks>
        private static void RegisterInterlockedCompareExchangeGeneric(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "Interlocked",
                MethodName: "CompareExchange",
                Resolver: (planner, callee, typeArgs) =>
                {
                    // Only the 3-argument `(ref T, T, T)` form is routable: the
                    // native helpers below take (location, value, comparand).
                    // A different arity would need its own symbol, and guessing
                    // here would pass the wrong argument as the comparand.
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 3) return null;

                    var typeArg = typeArgs is { Count: > 0 } ? typeArgs[0].Trim() : null;
                    var (nativeSymbol, carrier) = typeArg switch
                    {
                        "System.Int32" => ("ChaosInterlockedCompareExchangeInt32", AotCoreIrAbiCarrierKind.Int32),
                        "System.Int64" => ("ChaosInterlockedCompareExchangeInt64", AotCoreIrAbiCarrierKind.Int64),
                        _ => (null, AotCoreIrAbiCarrierKind.Void),
                    };
                    if (nativeSymbol == null) return null;

                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    return new GenericShapeResolution("", symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new AotCoreIrAbiSlotArtifact
                            {
                                CarrierKindCode = carrier,
                                TypeShape = AotCoreIrTypeShapeKind.ValueType
                            },
                            new AotCoreIrAbiSlotArtifact
                            {
                                CarrierKindCode = carrier,
                                TypeShape = AotCoreIrTypeShapeKind.ValueType
                            },
                        }),
                        new AotCoreIrAbiSlotArtifact
                        {
                            CarrierKindCode = carrier,
                            TypeShape = AotCoreIrTypeShapeKind.ValueType
                        },
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: nativeSymbol);
                }));
        }

    }
}
