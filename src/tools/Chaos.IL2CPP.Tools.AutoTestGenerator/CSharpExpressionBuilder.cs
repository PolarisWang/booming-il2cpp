using System.Reflection;
using System.Text;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Builds C# expressions for constructing instances of types.
/// Used to generate instance method calls with proper constructor initialization.
/// </summary>
public sealed class CSharpExpressionBuilder
{
    /// <summary>
    /// C# keyword aliases that cannot follow global:: (e.g. global::string is invalid).
    /// When ToCSharpTypeName/StripAssemblyQualification produces one of these, emit the
    /// bare keyword directly instead of global::&lt;keyword&gt;.
    /// </summary>
    internal static bool IsCSharpKeyword(string name) => name switch
    {
        "bool" or "byte" or "sbyte" or "short" or "ushort" or
        "int" or "uint" or "long" or "ulong" or
        "float" or "double" or "decimal" or
        "char" or "string" or "object" or "void" => true,
        _ => false
    };

    private readonly CSharpSerializer _serializer;

    // Types with well-known static factory instances (abstract or no default ctor)
    private static readonly Dictionary<string, string> KnownInstances = new(StringComparer.Ordinal)
    {
        ["System.Text.Encoding"] = "Encoding.UTF8",
        ["System.String"] = "string.Empty",
        // System.Array is abstract — default(Array) is null and causes NRE
        // in AOT-generated code.  Use Array.Empty<int>() for a valid instance.
        ["System.Array"] = "System.Array.Empty<int>()",
        // Crypto abstract type factories — .Create() returns concrete instances
        ["System.Security.Cryptography.HashAlgorithm"] = "System.Security.Cryptography.SHA256.Create()",
        ["System.Security.Cryptography.SymmetricAlgorithm"] = "System.Security.Cryptography.Aes.Create()",
        ["System.Security.Cryptography.AsymmetricAlgorithm"] = "System.Security.Cryptography.RSA.Create()",
        ["System.Security.Cryptography.RSA"] = "System.Security.Cryptography.RSA.Create()",
        ["System.Security.Cryptography.SHA256"] = "System.Security.Cryptography.SHA256.Create()",
        // XML abstract type factories
        ["System.Xml.XmlReader"] = "System.Xml.XmlReader.Create(new System.IO.StringReader(\"<root/>\"))",
        ["System.Xml.XmlWriter"] = "System.Xml.XmlWriter.Create(System.IO.Stream.Null)",
        // IO stream/text factories
        ["System.IO.Stream"] = "System.IO.Stream.Null",
        ["System.IO.TextReader"] = "new System.IO.StringReader(\"\")",
        ["System.IO.TextWriter"] = "new System.IO.StringWriter()",
        // Dataflow block 类型 — new + lambda，避免 default(null) 抛 NRE
        ["System.Threading.Tasks.Dataflow.ActionBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.ActionBlock<System.Int32>(_ => { })",
        ["System.Threading.Tasks.Dataflow.BufferBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BufferBlock<System.Int32>()",
        ["System.Threading.Tasks.Dataflow.BroadcastBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BroadcastBlock<System.Int32>(_ => _)",
        ["System.Threading.Tasks.Dataflow.TransformBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.TransformBlock<System.Int32, System.Int32>(x => x)",
        ["System.Threading.Tasks.Dataflow.TransformManyBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.TransformManyBlock<System.Int32, System.Int32>(x => System.Array.Empty<System.Int32>())",
        ["System.Threading.Tasks.Dataflow.BatchBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BatchBlock<System.Int32>(1)",
        ["System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32>(1)",
        ["System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.BatchedJoinBlock<System.Int32, System.Int32, System.Int32>(1)",
        ["System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32>()",
        ["System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32, System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.JoinBlock<System.Int32, System.Int32, System.Int32>()",
        ["System.Threading.Tasks.Dataflow.WriteOnceBlock<System.Int32>"] =
            "new System.Threading.Tasks.Dataflow.WriteOnceBlock<System.Int32>(_ => _)",
        // IO.Pipelines
        ["System.IO.Pipelines.Pipe"] = "new System.IO.Pipelines.Pipe()",
        ["System.IO.Pipelines.PipeReader"] = "System.IO.Pipelines.PipeReader.Create(System.IO.Stream.Null)",
        ["System.IO.Pipelines.PipeWriter"] = "System.IO.Pipelines.PipeWriter.Create(System.IO.Stream.Null)",
        ["System.IO.Pipelines.PipeScheduler"] = "System.IO.Pipelines.PipeScheduler.Inline",
        // System.Formats.Tar
        ["System.Formats.Tar.TarReader"] = "new System.Formats.Tar.TarReader(System.IO.Stream.Null)",
        ["System.Formats.Tar.TarWriter"] = "new System.Formats.Tar.TarWriter(System.IO.Stream.Null)",
        // System.Net.Http
        ["System.Net.Http.HttpClient"] = "new System.Net.Http.HttpClient()",
        ["System.Net.Http.Headers.AuthenticationHeaderValue"] =
            "new System.Net.Http.Headers.AuthenticationHeaderValue(\"Bearer\", \"token\")",
        ["System.Net.Http.Headers.MediaTypeHeaderValue"] =
            "new System.Net.Http.Headers.MediaTypeHeaderValue(\"application/json\")",
        ["System.Net.Http.Headers.MediaTypeWithQualityHeaderValue"] =
            "new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue(\"application/json\")",
        ["System.Net.Http.Headers.ProductHeaderValue"] =
            "new System.Net.Http.Headers.ProductHeaderValue(\"test\", \"1.0\")",
        ["System.Net.Http.Headers.ProductInfoHeaderValue"] =
            "new System.Net.Http.Headers.ProductInfoHeaderValue(\"test\", \"1.0\")",
        ["System.Net.Http.Headers.CacheControlHeaderValue"] =
            "new System.Net.Http.Headers.CacheControlHeaderValue()",
        ["System.Net.Http.Headers.ContentDispositionHeaderValue"] =
            "new System.Net.Http.Headers.ContentDispositionHeaderValue(\"attachment\")",
        ["System.Net.Http.Headers.ContentRangeHeaderValue"] =
            "new System.Net.Http.Headers.ContentRangeHeaderValue(0, 99, 100)",
        ["System.Net.Http.Headers.EntityTagHeaderValue"] =
            "new System.Net.Http.Headers.EntityTagHeaderValue(\"\\\"tag\\\"\")",
        ["System.Net.Http.Headers.NameValueHeaderValue"] =
            "new System.Net.Http.Headers.NameValueHeaderValue(\"name\", \"value\")",
        ["System.Net.Http.Headers.NameValueWithParametersHeaderValue"] =
            "new System.Net.Http.Headers.NameValueWithParametersHeaderValue(\"name\", \"value\")",
        ["System.Net.Http.Headers.RangeConditionHeaderValue"] =
            "new System.Net.Http.Headers.RangeConditionHeaderValue(new System.DateTime(2024, 1, 1))",
        ["System.Net.Http.Headers.RangeHeaderValue"] =
            "new System.Net.Http.Headers.RangeHeaderValue(0, 100)",
        ["System.Net.Http.Headers.RetryConditionHeaderValue"] =
            "new System.Net.Http.Headers.RetryConditionHeaderValue(new System.DateTime(2024, 1, 1))",
        ["System.Net.Http.Headers.StringWithQualityHeaderValue"] =
            "new System.Net.Http.Headers.StringWithQualityHeaderValue(\"en\", 0.5)",
        ["System.Net.Http.Headers.TransferCodingHeaderValue"] =
            "new System.Net.Http.Headers.TransferCodingHeaderValue(\"chunked\")",
        ["System.Net.Http.Headers.TransferCodingWithQualityHeaderValue"] =
            "new System.Net.Http.Headers.TransferCodingWithQualityHeaderValue(\"chunked\", 0.5)",
        ["System.Net.Http.Headers.ViaHeaderValue"] =
            "new System.Net.Http.Headers.ViaHeaderValue(\"1.1\", \"host\")",
        ["System.Net.Http.Headers.WarningHeaderValue"] =
            "new System.Net.Http.Headers.WarningHeaderValue(299, \"-\", \"\\\"warning\\\"\")",
        // System.Text.Json
        ["System.Text.Json.JsonSerializerOptions"] = "new System.Text.Json.JsonSerializerOptions()",
        // System.Diagnostics.DiagnosticSource
        ["System.Diagnostics.ActivitySource"] = "new System.Diagnostics.ActivitySource(\"test\")",
        ["System.Diagnostics.ActivityListener"] = "new System.Diagnostics.ActivityListener()",
        ["System.Diagnostics.DiagnosticListener"] = "new System.Diagnostics.DiagnosticListener(\"test\")",
        ["System.Diagnostics.ActivityTagsCollection"] = "new System.Diagnostics.ActivityTagsCollection()",
        ["System.Diagnostics.Metrics.Meter"] = "new System.Diagnostics.Metrics.Meter(\"test\")",
        ["System.Diagnostics.Metrics.MeterListener"] = "new System.Diagnostics.Metrics.MeterListener()",
        // System.Security.AccessControl
        ["System.Security.AccessControl.RawSecurityDescriptor"] =
            "new System.Security.AccessControl.RawSecurityDescriptor(\"\")",
        ["System.Security.AccessControl.CommonSecurityDescriptor"] =
            "new System.Security.AccessControl.CommonSecurityDescriptor(false, false, \"\")",
        ["System.Security.Policy.Evidence"] = "new System.Security.Policy.Evidence()",
        ["System.Security.Policy.EvidenceBase"] = "new System.Security.Policy.Evidence()",
        // Globalization types — abstract/default(Calendar)! causes NRE
        ["System.Globalization.Calendar"] = "new System.Globalization.GregorianCalendar()",
        ["System.Globalization.CompareInfo"] = "System.Globalization.CultureInfo.InvariantCulture.CompareInfo",
        ["System.Globalization.DateTimeFormatInfo"] = "System.Globalization.CultureInfo.InvariantCulture.DateTimeFormat",
        ["System.Globalization.CultureInfo"] = "System.Globalization.CultureInfo.InvariantCulture",
        ["System.Globalization.EastAsianLunisolarCalendar"] = "new System.Globalization.ChineseLunisolarCalendar()",
        ["System.Globalization.ChineseLunisolarCalendar"] = "new System.Globalization.ChineseLunisolarCalendar()",
        // Reflection API types — provide valid instances instead of default(T)!
        // default(FieldInfo)! passes a zero handle to the AOT native stub,
        // causing NullReferenceException in all test subjects. Using real
        // metadata instances produces valid handles and meaningful assertions.
        ["System.Reflection.FieldInfo"] =
            "typeof(int).GetFields(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static)[0]",
        ["System.Reflection.PropertyInfo"] =
            "typeof(string).GetProperty(\"Length\")!",
        ["System.Reflection.MethodInfo"] =
            "typeof(string).GetMethod(\"ToString\", System.Type.EmptyTypes)!",
        ["System.Reflection.ConstructorInfo"] =
            "typeof(string).GetConstructor(System.Type.EmptyTypes)!",
        ["System.Reflection.MemberInfo"] =
            "typeof(object).GetMembers(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Instance)[0]",
        ["System.Reflection.Module"] =
            "typeof(int).Module",
        ["System.Reflection.ParameterInfo"] =
            "typeof(string).GetMethod(\"IndexOf\", new[] { typeof(char) })!.GetParameters()[0]",
        ["System.Reflection.EventInfo"] =
            "typeof(System.ComponentModel.PropertyChangedEventArgs).GetEvents()[0]",
        ["System.Reflection.Assembly"] =
            "typeof(int).Assembly",
        // System.Net.Sockets — constructible instance types
        ["System.Net.Sockets.Socket"] =
            "new System.Net.Sockets.Socket(System.Net.Sockets.SocketType.Stream, System.Net.Sockets.ProtocolType.Tcp)",
        ["System.Net.Sockets.SocketAsyncEventArgs"] =
            "new System.Net.Sockets.SocketAsyncEventArgs()",
        ["System.Net.Sockets.TcpClient"] =
            "new System.Net.Sockets.TcpClient()",
        ["System.Net.Sockets.TcpListener"] =
            "new System.Net.Sockets.TcpListener(System.Net.IPAddress.Loopback, 0)",
        ["System.Net.Sockets.UdpClient"] =
            "new System.Net.Sockets.UdpClient()",
        ["System.Net.Sockets.UnixDomainSocketEndPoint"] =
            "new System.Net.Sockets.UnixDomainSocketEndPoint(\"/tmp/test\")",
        // System.Net.ServerSentEvents
        ["System.Net.ServerSentEvents.SseParser"] =
            "System.Net.ServerSentEvents.SseParser.Create<int>(System.IO.Stream.Null)",
        // System.IO.Compression — a valid, empty in-memory zip (22-byte EOCD-only)
        // so ZipFileExtensions instance methods get a real archive instead of a
        // bare GetUninitializedObject (null backing store → ArgumentNullException).
        ["System.IO.Compression.ZipArchive"] =
            "new System.IO.Compression.ZipArchive(new System.IO.MemoryStream(System.Convert.FromBase64String(\"UEsFBgAAAAAAAAAAAAAAAAAAAAAAAA==\")), System.IO.Compression.ZipArchiveMode.Read)",
        // System.Runtime.Serialization — formatter types with parameterless ctors.
        // A bare GetUninitializedObject has null internal state, so every instance
        // method throws (ObjectIDGenerator.GetId, BinaryFormatter.Serialize, …).
        // These are real constructible types; `new T()` gives working instances.
        ["System.Runtime.Serialization.ObjectIDGenerator"] =
            "new System.Runtime.Serialization.ObjectIDGenerator()",
        ["System.Runtime.Serialization.FormatterConverter"] =
            "new System.Runtime.Serialization.FormatterConverter()",
        ["System.Runtime.Serialization.SurrogateSelector"] =
            "new System.Runtime.Serialization.SurrogateSelector()",
        ["System.Runtime.Serialization.Formatters.Binary.BinaryFormatter"] =
            "new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter()",
        // System.Runtime.Serialization — abstract subclasses constructed
        // NOTE: Formatter deliberately excluded — ~20 abstract members
        // (WriteDouble, WriteChar, …) vary by framework version.
        ["System.Runtime.Serialization.SerializationBinder"] = "new TestSerializationBinder()",
        // System.Xml — XsltContext and XPathExpression are abstract with too
        // many abstract members whose set varies by framework version, so
        // no stable synthesized subclass is possible.
    };

    // Types with a static `Shared` property that returns a valid instance.
    // Using Shared instead of default(Type)! avoids NullReferenceException
    // when calling instance methods (e.g. ArrayPool<Int32>.Shared.Rent(0)).
    // The key is matched as a prefix (before `<` for generic types).
    private static readonly Dictionary<string, string> SharedInstanceTypes = new(StringComparer.Ordinal)
    {
        ["System.Buffers.ArrayPool"] = ".Shared",
        // Frozen collections — .Empty returns a real non-bare instance
        ["System.Collections.Frozen.FrozenDictionary"] = ".Empty",
        ["System.Collections.Frozen.FrozenSet"] = ".Empty",
    };

    /// <summary>
    /// Abstract types that ATG cannot instantiate directly, mapped to a concrete
    /// subclass it can emit into the generated test class.
    ///
    /// Background: for an abstract type, GetInstanceExpression falls back to
    /// SubjectInstanceFactory.Create&lt;T&gt;() which uses GetUninitializedObject —
    /// a bare object with null internal state.  Every instance method then throws
    /// (KeyedCollection.Contains → InvalidOperationException because the key
    /// extractor was never supplied), so the whole subject degrades to smoke-42.
    ///
    /// Fix: emit a minimal concrete subclass inline (see
    /// <see cref="SynthesizedSubclasses"/>) and construct *that* instead.  The
    /// subclass only needs to satisfy the abstract members; behaviour comes from
    /// the base type's real implementation.
    ///
    /// Key = the abstract type's full name as it appears in a subject id
    /// (namespace-qualified, generic args preserved e.g.
    /// "System.Collections.ObjectModel.KeyedCollection&lt;System.Int32,System.Int32&gt;").
    /// Prefix matching (before any '&lt;') is used so one entry covers every
    /// instantiation.
    /// </summary>
    private static readonly Dictionary<string, string> AbstractSubclassMap = new(StringComparer.Ordinal)
    {
        // KeyedCollection<TKey,TItem> is abstract: it requires GetKeyForItem.
        // For the generic-instantiated form ATG actually sees, key on the prefix
        // and synthesise a subclass with an identity key extractor — correct for
        // any TItem where the item is its own key (true for int/string/int-like).
        ["System.Collections.ObjectModel.KeyedCollection"] = "TestKeyedCollection",
        // System.Runtime.Serialization — both are abstract with a single
        // abstract member that has a trivial passthrough implementation.
        ["System.Runtime.Serialization.SerializationBinder"] = "TestSerializationBinder",
    };

    /// <summary>
    /// Source for each synthesized concrete subclass, keyed by the class name in
    /// <see cref="AbstractSubclassMap"/>.  Emitted once per generated file by
    /// <see cref="EmitSynthesizedSubclasses"/>.  Only the abstract members are
    /// implemented; everything else is inherited from the real base type.
    /// </summary>
    internal static readonly Dictionary<string, string> SynthesizedSubclasses = new(StringComparer.Ordinal)
    {
        ["TestKeyedCollection"] = """
            // Synthesized by ATG: concrete KeyedCollection so instance methods
            // (Contains/TryGetValue/Remove/…) run on a real backing dictionary
            // instead of a GetUninitializedObject bare instance.
            internal sealed class TestKeyedCollection<TKey, TItem> : System.Collections.ObjectModel.KeyedCollection<TKey, TItem>
                where TKey : notnull
            {
                protected override TKey GetKeyForItem(TItem item) => (TKey)(object)item!;
            }
            """,
        ["TestSerializationBinder"] = """
            // Synthesized by ATG: concrete SerializationBinder.  BindToType passes
            // the type name through Assembly.GetType, which is the documented
            // default behaviour; BindToName writes the assembly-qualified name.
            internal sealed class TestSerializationBinder : System.Runtime.Serialization.SerializationBinder
            {
                public override System.Type? BindToType(string assemblyName, string typeName)
                    => System.Type.GetType(typeName, throwOnError: false);

                public override void BindToName(System.Type serializedType, out string? assemblyName, out string? typeName)
                {
                    assemblyName = serializedType.Assembly.FullName;
                    typeName = serializedType.FullName;
                }
            }
            """,
        ["TestXPathExpression"] = """
            internal sealed class TestXPathExpression : System.Xml.XPath.XPathExpression
            {
                public override string Expression => "/";
                public override System.Xml.XPath.XPathResultType ReturnType => System.Xml.XPath.XPathResultType.NodeSet;
                public override void SetContext(System.Xml.XmlNamespaceManager nsManager) { }
                public override void SetContext(System.Xml.IXmlNamespaceResolver nsResolver) { }
                public override object Evaluate(System.Xml.XPath.XPathNodeIterator nodeIterator)
                    => throw new System.NotSupportedException("TestXPathExpression is a construction fixture");
            }
            """,
        ["TestXsltContext"] = """
            internal sealed class TestXsltContext : System.Xml.Xsl.XsltContext
            {
                public override System.Xml.Xsl.IXsltContextFunction ResolveFunction(string prefix, string name, System.Xml.XPath.XPathResultType[] argTypes)
                    => throw new System.NotSupportedException("TestXsltContext is a construction fixture");
                public override System.Xml.Xsl.IXsltContextVariable ResolveVariable(string prefix, string name)
                    => throw new System.NotSupportedException("TestXsltContext is a construction fixture");
                public override int CompareDocument(string baseUriA, string baseUriB) => 0;
                public override bool PreserveWhitespace(System.Xml.XPath.XPathNavigator node) => true;
                public override bool Whitespace => true;
            }
            """,
    };

    /// <summary>
    /// Concrete subclass expression for an abstract type, or null when the type
    /// is not (or not yet) covered.  Uses prefix matching so a single entry
    /// covers every generic instantiation.
    /// </summary>
    public static string? TryGetAbstractSubclassExpression(string typeFullName, string csType)
    {
        foreach (var (prefix, className) in AbstractSubclassMap)
        {
            if (!typeFullName.StartsWith(prefix, StringComparison.Ordinal))
                continue;
            // Preserve the generic argument list (if any) so the synthesized
            // generic subclass gets the same type arguments.
            var gaStart = csType.IndexOf('<');
            var ga = gaStart >= 0 ? csType[gaStart..] : "";
            return $"new {className}{ga}()";
        }
        return null;
    }

    /// <summary>
    /// Emit every synthesized subclass into the generated file (once).
    /// </summary>
    public static void EmitSynthesizedSubclasses(System.Text.StringBuilder sb, string indent, IEnumerable<string> usedNames)
    {
        var used = new HashSet<string>(usedNames, StringComparer.Ordinal);
        foreach (var (name, source) in SynthesizedSubclasses)
        {
            if (!used.Contains(name))
                continue;
            foreach (var line in source.Split('\n'))
                sb.AppendLine(indent + line.TrimEnd('\r'));
            sb.AppendLine();
        }
    }

    /// <summary>
    /// Names of the synthesized subclasses that are actually referenced, so the
    /// emitter only writes the ones a given file uses.
    /// </summary>
    internal static IEnumerable<string> UsedSubclassNames(IEnumerable<string> instanceExpressions)
    {
        var used = new HashSet<string>(StringComparer.Ordinal);
        foreach (var expr in instanceExpressions)
            foreach (var name in SynthesizedSubclasses.Keys)
                if (expr.Contains("new " + name, StringComparison.Ordinal))
                    used.Add(name);
        return used;
    }

    /// <summary>
    /// Describes how to construct an instance of a known type.
    /// </summary>
    private enum FactoryKind { Collection, Dictionary, EnumerableCtor, CustomExpr }

    private sealed record TypeFactory(
        FactoryKind Kind,
        int Arity,                          // number of generic type parameters
        string? CustomExpr = null);         // literal expression (for CustomExpr kind)

    private static readonly Dictionary<string, TypeFactory> KnownTypeFactories = new(StringComparer.Ordinal)
    {
        // Collections with Add(T) — collection initializer works
        ["System.Collections.Generic.List"] =        new(FactoryKind.Collection, 1),
        ["System.Collections.Generic.HashSet"] =      new(FactoryKind.Collection, 1),
        ["System.Collections.Generic.Collection"] =    new(FactoryKind.Collection, 1),

        // Dictionary with Add(K,V) — dictionary initializer works
        ["System.Collections.Generic.Dictionary"] =   new(FactoryKind.Dictionary, 2),

        // Collections with IEnumerable<T> constructor (no Add method)
        ["System.Collections.Generic.LinkedList"] =   new(FactoryKind.EnumerableCtor, 1),
        ["System.Collections.Generic.Stack"] =         new(FactoryKind.EnumerableCtor, 1),
        ["System.Collections.Generic.Queue"] =         new(FactoryKind.EnumerableCtor, 1),

        // ObservableCollection<T> — a bare GetUninitializedObject instance has a null
        // backing list, so Move/Remove throw ArgumentOutOfRangeException/NullReference.
        // Construct with seeded items via the IEnumerable<T> ctor so index-based
        // operations (Move(0,1)) succeed with deterministic semantics.
        ["System.Collections.ObjectModel.ObservableCollection"] = new(FactoryKind.EnumerableCtor, 1),

        // Special constructor expressions
        ["System.IO.MemoryStream"] =                   new(FactoryKind.CustomExpr, 0,
            "new MemoryStream(new byte[] { 1, 2, 3 })"),

        // ── Reflection subjects ─────────────────────────────────────────
        // Reflection member types are *views* over metadata: a bare
        // GetUninitializedObject instance has no metadata behind it, so every
        // accessor returns null/0 and the probe records either a bogus
        // exception or a value that verifies nothing. Each factory below
        // obtains a real, metadata-backed instance from a local sample type,
        // which is what gives the resulting assertions something to check.
        //
        // The seed type (`ReflectionSubjectSample`) is emitted by ProbeEmitter
        // into the probe project alongside these expressions.
        ["System.Reflection.Assembly"] =               new(FactoryKind.CustomExpr, 0,
            "global::System.Reflection.Assembly.GetExecutingAssembly()"),
        ["System.Reflection.Module"] =                 new(FactoryKind.CustomExpr, 0,
            "global::System.Reflection.Assembly.GetExecutingAssembly().ManifestModule"),
        ["System.Reflection.AssemblyName"] =           new(FactoryKind.CustomExpr, 0,
            "global::System.Reflection.Assembly.GetExecutingAssembly().GetName()"),
        ["System.Type"] =                              new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample)"),
        ["System.Reflection.MemberInfo"] =             new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample)"),
        ["System.Reflection.MethodBase"] =             new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetMethod(\"SampleMethod\")!"),
        ["System.Reflection.MethodInfo"] =             new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetMethod(\"SampleMethod\")!"),
        ["System.Reflection.ConstructorInfo"] =        new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetConstructor(global::System.Type.EmptyTypes)!"),
        ["System.Reflection.FieldInfo"] =              new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetField(\"SampleField\")!"),
        ["System.Reflection.PropertyInfo"] =           new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetProperty(\"SampleProperty\")!"),
        ["System.Reflection.EventInfo"] =              new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetEvent(\"SampleEvent\")!"),
        ["System.Reflection.ParameterInfo"] =          new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetMethod(\"SampleMethod\")!.GetParameters()[0]"),
        ["System.Reflection.CustomAttributeData"] =    new(FactoryKind.CustomExpr, 0,
            "global::System.Reflection.CustomAttributeData.GetCustomAttributes(typeof(global::Chaos.Probe.ReflectionSubjectSample))[0]"),
        ["System.Reflection.CustomAttributeNamedArgument"] = new(FactoryKind.CustomExpr, 0,
            "default(global::System.Reflection.CustomAttributeNamedArgument)"),
        ["System.Reflection.CustomAttributeTypedArgument"] = new(FactoryKind.CustomExpr, 0,
            "default(global::System.Reflection.CustomAttributeTypedArgument)"),
        ["System.Reflection.InterfaceMapping"] =       new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample).GetInterfaceMap(typeof(global::Chaos.Probe.IReflectionSubject))"),
        ["System.Reflection.ManifestResourceInfo"] =   new(FactoryKind.CustomExpr, 0,
            "default(global::System.Reflection.ManifestResourceInfo)!"),
        ["System.Reflection.TypeInfo"] =               new(FactoryKind.CustomExpr, 0,
            "typeof(global::Chaos.Probe.ReflectionSubjectSample)"),
    };

    public CSharpExpressionBuilder(CSharpSerializer serializer)
    {
        _serializer = serializer;
    }

    /// <summary>
    /// Get the best instance expression for a type.
    /// Static types → type name (static call prefix).
    /// Known constructible types → "new T(...)" or collection-initializer expression.
    /// Known factory instances → e.g. "Encoding.UTF8".
    /// Fallback → "default(TypeName)!".
    /// </summary>
    public string GetInstanceExpression(string typeFullName, bool isStatic)
    {
        if (isStatic)
            return $"global::{CSharpSerializer.StripAssemblyQualification(typeFullName)}";

        var csType = CSharpSerializer.MapToCSharpType(typeFullName);

        // Check known factory instances (Encoding.UTF8, string.Empty)
        // Check known factory instances (Encoding.UTF8, string.Empty).
        // Try the full namespace-qualified name first, then the C# short name:
        // MapToCSharpType strips the namespace (System.Net.Sockets.Socket → Socket)
        // but KnownInstances keys are fully qualified, so both must be probed.
        if (KnownInstances.TryGetValue(typeFullName, out var knownExprFull))
            return knownExprFull;
        if (KnownInstances.TryGetValue(csType, out var knownExpr))
            return knownExpr;

        // Check known type factories (collections, special constructors).
        // Pass typeFullName so the factory table (which is keyed on the fully
        // qualified name) can match.
        var factoryResult = TryBuildFactoryExpression(typeFullName, csType);
        if (factoryResult is not null)
            return factoryResult;

        // Check the abstract-subclass map before falling back to
        // SubjectInstanceFactory.  Abstract types (KeyedCollection, etc.)
        // cannot be instantiated via GetUninitializedObject — all instance
        // methods throw because abstract members are missing.  For known
        // types we emit a real synthesized subclass.
        var subclassExpr = TryGetAbstractSubclassExpression(typeFullName, csType);
        if (subclassExpr is not null)
            return subclassExpr;

        // For types with namespace qualification, use global:: prefix.
        var qualified = CSharpSerializer.StripAssemblyQualification(typeFullName);
        if (qualified.Contains('.'))
        {
            // Check SharedInstanceTypes before falling back to default.
            // Include generic type arguments so e.g. ArrayPool<System.Int32>.Shared works.
            var baseName = qualified.Contains('<') ? qualified[..qualified.IndexOf('<')] : qualified;
            if (SharedInstanceTypes.TryGetValue(baseName, out var sharedSuffix))
            {
                var gaPart = qualified.Contains('<') ? qualified[qualified.IndexOf('<')..] : "";
                return $"global::{baseName}{gaPart}{sharedSuffix}";
            }
                        // Use SubjectInstanceFactory for valid instances. Ref structs and
            // unresolvable types fall back to default(T)! (ref structs can't be
            // generic type params; unresolved types may be in unreferenced assemblies).
            try {
                var t = Type.GetType(typeFullName, false);
                var qualifiedType = CSharpSerializer.ToQualifiedCSharpType(typeFullName);
                // Ref struct types can't be used as generic type arguments — use default(T) for them.
                if (t != null && t.IsValueType && t.IsByRefLike)
                    return $"default({qualifiedType})!";
                // For types where Type.GetType returns null (BCL types not loaded in ATG process),
                // fall through to SubjectInstanceFactory.Create<T>() which uses GetUninitializedObject
                // to return a non-null instance. This fixes the 908-case "default(global::T)! → NRE"
                // pattern that caused 42-sentinel false passes.
                // NOTE: ToQualifiedCSharpType keeps the full namespace path for non-keyword types
                // (e.g. System.Globalization.CultureInfo), and collapses primitives to keywords
                // (string, int, bool).  This avoids two invalid patterns:
                //   global::string   — global:: cannot precede a C# keyword (CS1525)
                //   global::CultureInfo — bare type not globally resolvable (CS0400)
                // When t is null (unresolvable) we still emit Create<T> since the caller
                // expects a non-null instance; if the type turns out to be a ref struct
                // the compilation will fail with CS9244 — but that's better than silently
                // returning null from default(T) and producing false positives.
                return $"SubjectInstanceFactory.Create<{qualifiedType}>()";
            } catch { /* fall through to default(T) fallback */ }
            return $"default({CSharpSerializer.ToQualifiedCSharpType(typeFullName)})!";
        }

        // Try to find a parameterless constructor via runtime reflection
        try
        {
            var type = Type.GetType(typeFullName, throwOnError: false);
            if (type is not null && !type.IsAbstract && !type.IsInterface)
            {
                var ctors = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance);
                if (ctors.Any(c => c.GetParameters().Length == 0))
                    return $"new {csType}()";
            }
        }
        catch
        {
            // Best-effort — fall through to default
        }

        return _serializer.DefaultExpression(csType);
    }

    /// <summary>
    /// Try to build a factory expression from the KnownTypeFactories table.
    /// Handles generic type argument extraction and seed value generation.
    /// </summary>
    private static string? TryBuildFactoryExpression(string typeFullName, string csType)
    {
        // Extract base name (before <) to look up in KnownTypeFactories
        var gaStart = typeFullName.IndexOf('<');
        var baseName = gaStart >= 0 ? typeFullName[..gaStart] : typeFullName;

        // Strip assembly qualifier + namespace prefix for lookup
        // But KnownTypeFactories uses full namespace.QualifiedName format
        if (!KnownTypeFactories.TryGetValue(baseName, out var factory))
            return null;

        // Extract type arguments from the C#-style type name
        var csGaStart = csType.IndexOf('<');
        string[] csTypeArgs;
        if (csGaStart >= 0 && factory.Arity > 0)
        {
            var inner = csType[(csGaStart + 1)..^1]; // strip <>
            csTypeArgs = SplitTopLevelArgs(inner, ',');
        }
        else
        {
            csTypeArgs = Array.Empty<string>();
        }

        // Custom expression (e.g. MemoryStream)
        if (factory.Kind == FactoryKind.CustomExpr && factory.CustomExpr is not null)
            return factory.CustomExpr;

        // Enumerable constructor: new Stack<int>(new[] { 1, 2, 3 })
        if (factory.Kind == FactoryKind.EnumerableCtor && csTypeArgs.Length >= 1)
        {
            var seeds = GenerateSeedLiterals(csTypeArgs[0], 3);
            return $"new {csType}(new[] {{ {string.Join(", ", seeds)} }})";
        }

        // Collection initializer: new List<int> { 1, 2, 3 }
        if (factory.Kind == FactoryKind.Collection && csTypeArgs.Length >= 1)
        {
            var seeds = GenerateSeedLiterals(csTypeArgs[0], 3);
            return $"new {csType} {{ {string.Join(", ", seeds)} }}";
        }

        // Dictionary initializer: new Dictionary<int,int> { { 1, 2 }, { 3, 4 } }
        if (factory.Kind == FactoryKind.Dictionary && csTypeArgs.Length >= 2)
        {
            var keySeeds = GenerateSeedLiterals(csTypeArgs[0], 3);
            var valSeeds = GenerateSeedLiterals(csTypeArgs[1], 3);
            var entries = new List<string>();
            for (int i = 0; i < 3 && i < keySeeds.Length && i < valSeeds.Length; i++)
                entries.Add($"{{ {keySeeds[i]}, {valSeeds[i]} }}");
            return $"new {csType} {{ {string.Join(", ", entries)} }}";
        }

        return null;
    }

    /// <summary>
    /// Generate seed literal values for a given C# type name.
    /// Produces 'count' distinct literals suitable for collection seeding.
    /// </summary>
    private static string[] GenerateSeedLiterals(string csTypeName, int count)
    {
        return csTypeName switch
        {
            "int" or "Int32" => Enumerable.Range(1, count).Select(i => i.ToString()).ToArray(),
            "uint" or "UInt32" => Enumerable.Range(1, count).Select(i => $"{i}u").ToArray(),
            "long" or "Int64" => Enumerable.Range(1, count).Select(i => $"{i}L").ToArray(),
            "ulong" or "UInt64" => Enumerable.Range(1, count).Select(i => $"{i}UL").ToArray(),
            "short" or "Int16" => Enumerable.Range(1, count).Select(i => $"(short){i}").ToArray(),
            "ushort" or "UInt16" => Enumerable.Range(1, count).Select(i => $"(ushort){i}").ToArray(),
            "byte" or "Byte" => Enumerable.Range(1, count).Select(i => $"(byte){i}").ToArray(),
            "sbyte" or "SByte" => Enumerable.Range(1, count).Select(i => $"(sbyte){i}").ToArray(),
            "float" or "Single" => Enumerable.Range(1, count).Select(i => $"{i}f").ToArray(),
            "double" or "Double" => Enumerable.Range(1, count).Select(i => $"{i}.0").ToArray(),
            "decimal" or "Decimal" => Enumerable.Range(1, count).Select(i => $"{i}m").ToArray(),
            "bool" or "Boolean" => new[] { "true", "false", "true" },
            "char" or "Char" => new[] { "'A'", "'B'", "'C'" },
            "string" or "String" => Enumerable.Range(0, count).Select(i => $"\"seed{i}\"").ToArray(),
            _ => Enumerable.Range(0, count).Select(i => $"default({csTypeName})!").ToArray(),
        };
    }

    /// <summary>
    /// Split top-level generic arguments separated by a delimiter.
    /// Respects nested angle brackets: "int, List<int>" → ["int", "List<int>"].
    /// </summary>
    private static string[] SplitTopLevelArgs(string s, char delimiter)
    {
        var result = new List<string>();
        int depth = 0, start = 0;
        for (int i = 0; i < s.Length; i++)
        {
            if (s[i] == '<' || s[i] == '[' || s[i] == '{')
                depth++;
            else if (s[i] == '>' || s[i] == ']' || s[i] == '}')
                depth--;
            else if (s[i] == delimiter && depth == 0)
            {
                result.Add(s[start..i].Trim());
                start = i + 1;
            }
        }
        if (start < s.Length)
            result.Add(s[start..].Trim());
        return result.ToArray();
    }
}
