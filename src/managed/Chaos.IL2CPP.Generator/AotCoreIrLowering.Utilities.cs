using Chaos.IL2CPP.Contracts;
namespace Chaos.IL2CPP.Generator;

public sealed partial class AotCoreIrLowering
{

    private static bool SequenceEqual(
        IReadOnlyList<string>? left,
        IReadOnlyList<string>? right)
    {
        if (ReferenceEquals(left, right))
        {
            return true;
        }

        if (left is null || right is null || left.Count != right.Count)
        {
            return false;
        }

        for (var index = 0; index < left.Count; index++)
        {
            if (!string.Equals(left[index], right[index], StringComparison.Ordinal))
            {
                return false;
            }
        }

        return true;
    }

    private static AotCoreIrTypeShapeKind ResolveTypeShape(
        ManagedTypeModel? managedType,
        string? fallbackTypeIdentityOrSubjectId = null)
    {
        if (managedType is null)
        {
            return IsKnownValueTypeIdentity(fallbackTypeIdentityOrSubjectId)
                ? AotCoreIrTypeShapeKind.ValueType
                : AotCoreIrTypeShapeKind.ReferenceType;
        }

        if (managedType.IsInterface)
        {
            return AotCoreIrTypeShapeKind.InterfaceType;
        }

        return managedType.IsValueType
            ? AotCoreIrTypeShapeKind.ValueType
            : AotCoreIrTypeShapeKind.ReferenceType;
    }

    private static string? ResolveArrayElementSubjectId(string subjectId)
    {
        return subjectId.EndsWith("[]", StringComparison.Ordinal)
            ? subjectId[..^2]
            : null;
    }

    private static AotCoreIrTypeShapeKind ResolveArrayElementTypeShape(
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        string? arrayElementSubjectId)
    {
        if (string.IsNullOrWhiteSpace(arrayElementSubjectId))
        {
            return default;
        }

        return managedTypes.TryGetValue(arrayElementSubjectId, out var arrayElementType)
            ? ResolveTypeShape(arrayElementType)
            : ResolveTypeShape(null, arrayElementSubjectId);
    }

    private static bool IsKnownValueTypeIdentity(string? typeIdentityOrSubjectId)
    {
        return typeIdentityOrSubjectId switch
        {
            "System.Boolean" => true,
            "System.Byte" => true,
            "System.Char" => true,
            "System.Double" => true,
            "System.Int16" => true,
            "System.Int32" => true,
            "System.Int64" => true,
            "System.IntPtr" => true,
            "System.RuntimeArgumentHandle" => true,
            "System.RuntimeFieldHandle" => true,
            "System.RuntimeMethodHandle" => true,
            "System.RuntimeTypeHandle" => true,
            "System.SByte" => true,
            "System.Single" => true,
            "System.TypedReference" => true,
            "System.UInt16" => true,
            "System.UInt32" => true,
            "System.UInt64" => true,
            "System.UIntPtr" => true,
            _ => false,
        };
    }

    private static string GetDeclaringTypeSubjectId(string fieldSubjectId)
    {
        var separatorIndex = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException(
                $"field subject '{fieldSubjectId}' is missing declaring type information during AotCoreIr lowering.");
        }

        return fieldSubjectId[..separatorIndex];
    }

    private static string GetMemberDeclaringTypeSubjectId(string memberSubjectId)
    {
        var separatorIndex = memberSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException(
                $"member subject '{memberSubjectId}' is missing declaring type information during AotCoreIr lowering.");
        }

        return memberSubjectId[..separatorIndex];
    }

    private static bool IsPInvokeStringType(string typeIdentity)
    {
        return string.Equals(typeIdentity, "System.String", StringComparison.Ordinal);
    }

    /// <summary>
    /// Checks if a type derives from <see cref="System.Runtime.InteropServices.SafeHandle"/>
    /// by walking the <see cref="ManagedTypeModel.BaseTypeSubjectId"/> chain.
    /// </summary>
    private static bool IsSafeHandleDerivedType(string typeIdentity, IReadOnlyDictionary<string, ManagedTypeModel> managedTypes)
    {
        const string safeHandleSubjectId = "System.Runtime.InteropServices.SafeHandle";
        if (!managedTypes.TryGetValue(typeIdentity, out var typeModel))
            return false;

        var current = typeModel;
        while (current != null)
        {
            if (string.Equals(current.SubjectId, safeHandleSubjectId, StringComparison.Ordinal))
                return true;

            if (current.BaseTypeSubjectId == null)
                break;

            if (!managedTypes.TryGetValue(current.BaseTypeSubjectId, out current))
                break;
        }

        return false;
    }

    /// <summary>
    /// Checks if a type identity refers to a blittable primitive that has the same
    /// managed and native representation (no conversion needed).
    /// </summary>
    private static bool IsBlittablePrimitiveType(string typeIdentity)
    {
        return typeIdentity switch
        {
            "System.Boolean" => true,
            "System.Byte" => true,
            "System.SByte" => true,
            "System.Int16" => true,
            "System.UInt16" => true,
            "System.Int32" => true,
            "System.UInt32" => true,
            "System.Int64" => true,
            "System.UInt64" => true,
            "System.IntPtr" => true,
            "System.UIntPtr" => true,
            "System.Single" => true,
            "System.Double" => true,
            "System.Char" => true,
            _ => false,
        };
    }

    private enum StructFieldClassification { NonValueType, Empty, Blittable, HasStringFields, Complex }

    /// <summary>
    /// Classifies the instance fields of a value type for P/Invoke marshalling decisions.
    /// When <paramref name="stringFieldSubjectIds"/> is provided, populates it with the
    /// SubjectId of each string field found.
    /// </summary>
    private static StructFieldClassification ClassifyValueTypeFields(
        string typeIdentity,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        List<string>? stringFieldSubjectIds = null)
    {
        if (!managedTypes.TryGetValue(typeIdentity, out var typeModel))
            return StructFieldClassification.NonValueType;

        if (!typeModel.IsValueType)
            return StructFieldClassification.NonValueType;

        var fields = managedFields.Values
            .Where(f => string.Equals(f.DeclaringTypeSubjectId, typeIdentity, StringComparison.Ordinal)
                        && !f.IsStatic)
            .ToList();

        if (fields.Count == 0)
            return StructFieldClassification.Empty;

        bool hasString = false;
        foreach (var field in fields)
        {
            if (IsBlittablePrimitiveType(field.FieldType))
                continue;

            if (field.FieldType == "System.String")
            {
                hasString = true;
                stringFieldSubjectIds?.Add(field.SubjectId);
                continue;
            }

            // Nested value type — recurse
            var nestedClass = ClassifyValueTypeFields(field.FieldType, managedTypes, managedFields, stringFieldSubjectIds);
            if (nestedClass == StructFieldClassification.Blittable)
                continue;
            if (nestedClass == StructFieldClassification.HasStringFields)
            {
                hasString = true;
                continue;
            }

            return StructFieldClassification.Complex;
        }

        if (hasString)
            return StructFieldClassification.HasStringFields;

        return StructFieldClassification.Blittable;
    }

    /// <summary>
    /// Returns true when the given type identity names a value type whose fields
    /// are all blittable primitives (or nested blittable value types).
    /// </summary>
    private static bool IsBlittableStructType(
        string typeIdentity,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        if (string.IsNullOrEmpty(typeIdentity)) return false;
        return ClassifyValueTypeFields(typeIdentity, managedTypes, managedFields) == StructFieldClassification.Blittable;
    }

    /// <summary>
    /// For a P/Invoke method, detect which parameter indices are blittable value types.
    /// </summary>
    private static IReadOnlyList<int>? DetectBlittableStructParameters(
        IReadOnlyList<ManagedParameterModel> parameters,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        var indices = new List<int>();
        for (int i = 0; i < parameters.Count; i++)
        {
            if (ClassifyValueTypeFields(parameters[i].Type, managedTypes, managedFields) == StructFieldClassification.Blittable)
            {
                indices.Add(i);
            }
        }
        return indices.Count > 0 ? indices : null;
    }

    /// <summary>
    /// For a P/Invoke method, detect which parameter indices are value types containing
    /// string fields (simple non-blittable structs). Collects the SubjectIds of string
    /// fields into <paramref name="stringFieldSubjectIds"/> grouped per parameter.
    /// </summary>
    private static IReadOnlyList<int>? DetectSimpleNonBlittableStructParameters(
        IReadOnlyList<ManagedParameterModel> parameters,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        out IReadOnlyList<IReadOnlyList<string>>? stringFieldSubjectIds)
    {
        var indices = new List<int>();
        var allStringFields = new List<IReadOnlyList<string>>();
        for (int i = 0; i < parameters.Count; i++)
        {
            var fieldSubjectIds = new List<string>();
            var classification = ClassifyValueTypeFields(parameters[i].Type, managedTypes, managedFields, fieldSubjectIds);
            if (classification == StructFieldClassification.HasStringFields)
            {
                indices.Add(i);
                allStringFields.Add(fieldSubjectIds);
            }
        }
        if (indices.Count > 0)
        {
            stringFieldSubjectIds = allStringFields;
            return indices;
        }
        stringFieldSubjectIds = null;
        return null;
    }

    /// <summary>
    /// For a P/Invoke method, detect which parameter indices are complex non-blittable
    /// value types (structs containing fields beyond blittable primitives and simple strings).
    /// These require descriptor-driven marshalling via StructMarshallingDescriptorV1.
    /// </summary>
    private static IReadOnlyList<int>? DetectComplexStructParameters(
        IReadOnlyList<ManagedParameterModel> parameters,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        var indices = new List<int>();
        for (int i = 0; i < parameters.Count; i++)
        {
            var classification = ClassifyValueTypeFields(parameters[i].Type, managedTypes, managedFields);
            if (classification == StructFieldClassification.Complex)
            {
                indices.Add(i);
            }
        }
        return indices.Count > 0 ? indices : null;
    }

    /// <summary>
    /// Determine the native-size equivalent for a managed primitive type.
    /// Returns the native byte size, or null if the type is not a recognized primitive.
    /// </summary>
    private static int? GetNativePrimitiveSize(string fieldType)
    {
        return fieldType switch
        {
            "System.Boolean" => 4,   // Win32 BOOL
            "System.Byte" => 1,
            "System.SByte" => 1,
            "System.Char" => 2,      // UNICODE char16_t
            "System.Int16" => 2,
            "System.UInt16" => 2,
            "System.Int32" => 4,
            "System.UInt32" => 4,
            "System.Int64" => 8,
            "System.UInt64" => 8,
            "System.Single" => 4,
            "System.Double" => 8,
            "System.IntPtr" => 8,
            "System.UIntPtr" => 8,
            _ => null,
        };
    }

    /// <summary>
    /// Build a marshalling descriptor tree for a value type, classifying each field
    /// and computing its native offset/size. Uses the AOT model layout where managed
    /// fields are sequential IntPtr-sized slots; native offsets match for V1.
    /// </summary>
    private static StructMarshallingDescriptorArtifact? BuildStructMarshallingDescriptor(
        string typeIdentity,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        if (!managedTypes.TryGetValue(typeIdentity, out var typeModel) || !typeModel.IsValueType)
            return null;

        var fields = managedFields.Values
            .Where(f => string.Equals(f.DeclaringTypeSubjectId, typeIdentity, StringComparison.Ordinal)
                        && !f.IsStatic)
            .OrderBy(f => f.Name, StringComparer.Ordinal)
            .ToList();

        if (fields.Count == 0)
            return null;

        const int ptrSize = 8; // sizeof(CHAOS_IL2CPP_INTPTR)
        var fieldDescriptors = new List<StructFieldDescriptorArtifact>();
        int currentOffset = 0;

        foreach (var field in fields)
        {
            var (kind, size, arrayCount, elementType, nestedTypeId) = ClassifyFieldForMarshalling(
                field, managedTypes, managedFields);

            fieldDescriptors.Add(new StructFieldDescriptorArtifact
            {
                Kind = kind,
                Offset = currentOffset,
                Size = size,
                ArrayCount = arrayCount,
                ElementType = elementType,
                NestedTypeSubjectId = nestedTypeId,
                Name = field.Name,
            });

            currentOffset += ptrSize; // managed offset stride
        }

        return new StructMarshallingDescriptorArtifact
        {
            TypeSubjectId = typeIdentity,
            TotalSize = currentOffset,
            Fields = fieldDescriptors,
        };
    }

    /// <summary>
    /// Classify a single field for marshalling and determine its native parameters.
    /// Returns (kind, size, arrayCount, elementType, nestedTypeSubjectId).
    /// </summary>
    private static (string Kind, int Size, int ArrayCount, string? ElementType, string? NestedTypeSubjectId)
        ClassifyFieldForMarshalling(
            ManagedFieldModel field,
            IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
            IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        var ft = field.FieldType;

        // String fields
        if (ft == "System.String")
            return ("StringField", 8, 0, null, null);

        // Boolean → Win32 BOOL
        if (ft == "System.Boolean")
            return ("BoolField", 4, 0, null, null);

        // Decimal (16 bytes, COM DECIMAL compatible layout)
        if (ft == "System.Decimal")
            return ("DecimalField", 16, 0, null, null);

        // DateTime (8 bytes, FILETIME compatible)
        if (ft == "System.DateTime")
            return ("DateTimeField", 8, 0, null, null);

        // Guid (16 bytes, blittable)
        if (ft == "System.Guid")
            return ("GuidField", 16, 0, null, null);

        // object → ObjectField
        if (ft == "System.Object")
            return ("ObjectField", 8, 0, null, null);

        // Blittable primitive
        var nativeSize = GetNativePrimitiveSize(ft);
        if (nativeSize.HasValue)
            return ("Blittable", nativeSize.Value, 0, null, null);

        // Nested value type — recurse
        if (managedTypes.TryGetValue(ft, out var nestedType) && nestedType.IsValueType)
        {
            // Check if the nested type is at least partially supported
            var nestedClass = ClassifyValueTypeFields(ft, managedTypes, managedFields);
            if (nestedClass != StructFieldClassification.NonValueType)
            {
                return ("NestedStruct", 8, 0, null, ft);
            }
        }

        // Default: treat as blittable pointer-sized (managed reference or opaque)
        return ("Blittable", 8, 0, null, null);
    }

    /// <summary>
    /// Compute the COM vtable slot index for a method call on a ComImport interface.
    /// Slot = 3 (IUnknown reserved) + method ordinal within the interface's method list.
    /// The method ordinal is determined by MetadataToken order within the declaring type.
    /// </summary>
    private static int? ComputeComVtableSlot(
        TypedIlInstructionArtifact typedInstruction,
        IReadOnlyDictionary<string, ManagedMethodModel> managedMethods,
        AotCoreIrReferenceArtifact targetReference)
    {
        var declaringTypeId = targetReference.DeclaringTypeSubjectId;
        if (string.IsNullOrEmpty(declaringTypeId))
            return null;

        var calleeSubjectId = typedInstruction.Callee;
        if (string.IsNullOrEmpty(calleeSubjectId))
            return null;

        // Filter all methods belonging to the declaring interface, ordered by metadata token.
        var interfaceMethods = managedMethods.Values
            .Where(m => string.Equals(m.DeclaringTypeSubjectId, declaringTypeId, StringComparison.Ordinal))
            .OrderBy(m => m.MetadataToken)
            .ToList();

        if (interfaceMethods.Count == 0)
            return null;

        // Find the index of the target method in the ordered list.
        for (var i = 0; i < interfaceMethods.Count; i++)
        {
            if (string.Equals(interfaceMethods[i].SubjectId, calleeSubjectId, StringComparison.Ordinal))
            {
                return 3 + i; // 3 IUnknown reserved slots + method ordinal
            }
        }

        return null;
    }

    /// <summary>
    /// When a callvirt is preceded by the constrained. IL prefix on a value type,
    /// returns the value type's own override SubjectId for the virtual method slot.
    /// Returns null when no override exists (the original Callee should be kept).
    /// </summary>
    private static string? ResolveConstrainedValueTypeOverride(
        TypedIlInstructionArtifact typedInstruction)
    {
        var constrainedTypeId = typedInstruction.ConstrainedTypeSubjectId;
        if (string.IsNullOrWhiteSpace(constrainedTypeId))
            return null;

        // Extract the signature suffix (everything after "::") from the slot method.
        var callee = typedInstruction.Callee;
        if (string.IsNullOrWhiteSpace(callee))
            return null;

        var sepIdx = callee.IndexOf("::", StringComparison.Ordinal);
        if (sepIdx < 0)
            return null;
        var slotSig = callee.Substring(sepIdx + 2);

        // Constrained. callvirt on a value type should dispatch to the value type's
        // own override of the virtual method (e.g. Guid::GetHashCode instead of
        // Object::GetHashCode). We construct the expected SubjectId directly —
        // no need to verify IsValueType since the C# compiler only emits constrained.
        // for value types; for reference types it emits plain callvirt.

        // Reject generic parameter markers (!0, !!0) from F# or other sources.
        // A generic parameter is not a concrete type and has no override to dispatch to.
        if (constrainedTypeId.Contains('!'))
            return null;

        return constrainedTypeId + "::" + slotSig;
    }

}
