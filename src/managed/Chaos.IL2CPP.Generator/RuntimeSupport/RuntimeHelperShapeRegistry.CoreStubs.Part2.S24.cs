// T6 — array enumeration interfaces.
//
// Assert.AreEqual<T> compares two IEnumerable operands by walking
// GetEnumerator() / MoveNext() / get_Current().  When an operand is a managed
// array those three calls land in the AOT external-runtime table.  Before this
// registration they had no native body, so each fell to the catch-all and
// returned 0 — the enumeration loop terminated after the first MoveNext and
// every collection comparison reported a mismatch, including empty-vs-empty.
//
// These entries are *instance* methods: the receiver (the array, then the
// enumerator) is ABI slot 0.  The receiver injection for this interface pair is
// driven by _ReceiverInjectedTypes in
// NativeAotLoweringPlanner.ExternalRuntimeHelpers; the two must stay in step —
// registering here without the matching allowlist entry (or vice versa) leaves
// the helper signature and the call site disagreeing.

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// IEnumerable.GetEnumerator / IEnumerator.MoveNext / get_Current over a
        /// managed 1-D array.  Implemented in
        /// src/native/runtime-core/runtime_stubs/array_enumerator_stubs.cpp.
        /// </summary>
        private static void RegisterArrayEnumeratorStubs(RuntimeHelperShapeRegistry registry)
        {
            // IEnumerable.GetEnumerator() → IEnumerator
            registry.Register("System.Collections.IEnumerable", "GetEnumerator", [],
                ShapeKind.SimpleForward, "ChaosArrayGetEnumerator",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Collections.IEnumerable", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // IEnumerator.MoveNext() → bool
            registry.Register("System.Collections.IEnumerator", "MoveNext", [],
                ShapeKind.SimpleForward, "ChaosArrayEnumeratorMoveNext",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Collections.IEnumerator", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // IEnumerator.get_Current() → object
            registry.Register("System.Collections.IEnumerator", "get_Current", [],
                ShapeKind.SimpleForward, "ChaosArrayEnumeratorGetCurrent",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Collections.IEnumerator", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
        }
    }
}
