using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("Chaos.IL2CPP.Generator.Tests")]
[assembly: InternalsVisibleTo("Chaos.IL2CPP.CodeGen.Tests")]
// The ATG must know which BCL methods the AOT shape registry actually backs, so it
// can stop emitting the AOT-STUB-GAP `return 42L` body for methods that DO have a
// real native implementation.  That set is defined by RuntimeHelperShapeRegistry
// (internal) — reading it here is strictly more reliable than the hand-maintained
// Classifier.KnownNativeImpls list, which had drifted to zero entries for json/xml.
[assembly: InternalsVisibleTo("Chaos.IL2CPP.Tools.AutoTestGenerator")]
