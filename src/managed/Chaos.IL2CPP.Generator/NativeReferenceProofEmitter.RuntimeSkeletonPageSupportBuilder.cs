// Auto-generated partial: NativeReferenceProofEmitter.RuntimeSkeletonPageSupportBuilder
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
    private sealed class RuntimeSkeletonPageSupportBuilder
    {
        private readonly Dictionary<string, int> fieldBindingDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldBindingDescriptors = [];
        private readonly Dictionary<string, int> typeDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> typeDescriptors = [];
        private readonly Dictionary<string, int> constructorFieldSetterDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> constructorFieldSetterDescriptors = [];
        private readonly Dictionary<string, int> fieldGetterStringReturnDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldGetterStringReturnDescriptors = [];
        private readonly Dictionary<string, int> fieldBackedStringReturnDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldBackedStringReturnDescriptors = [];
        private readonly Dictionary<string, int> fieldArgumentStringReturnDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldArgumentStringReturnDescriptors = [];
        private readonly Dictionary<string, int> staticCallCtorGetterDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> staticCallCtorGetterDescriptors = [];
        private readonly Dictionary<string, int> staticStringLiteralAppendDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> staticStringLiteralAppendDescriptors = [];
        private readonly Dictionary<string, int> constructorThenInstanceCallDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> constructorThenInstanceCallDescriptors = [];
        private readonly Dictionary<string, int> delegateClosedTargetRelayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> delegateClosedTargetRelayDescriptors = [];
        private readonly Dictionary<string, int> arrayBoxingReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayBoxingReferenceArrayDescriptors = [];
        private readonly Dictionary<string, int> arrayReverseReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayReverseReferenceArrayDescriptors = [];
        private readonly Dictionary<string, int> arrayClearReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayClearReferenceArrayDescriptors = [];
        private readonly Dictionary<string, int> arrayCopyReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayCopyReferenceArrayDescriptors = [];

        public int GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            string assemblyNameLiteral,
            string referenceTypeToken,
            string capturedFieldToken)
        {
            var key = string.Join("|", assemblyNameLiteral, referenceTypeToken, capturedFieldToken);
            if (fieldBindingDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldBindingDescriptors.Count;
            fieldBindingDescriptorIndices.Add(key, descriptorIndex);
            fieldBindingDescriptors.Add($"    {{ {assemblyNameLiteral}, {referenceTypeToken}, {capturedFieldToken} }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonTypeDescriptor(
            string assemblyNameLiteral,
            string typeToken)
        {
            var key = string.Join("|", assemblyNameLiteral, typeToken);
            if (typeDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = typeDescriptors.Count;
            typeDescriptorIndices.Add(key, descriptorIndex);
            typeDescriptors.Add($"    {{ {assemblyNameLiteral}, {typeToken} }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonConstructorFieldSetterDescriptor(int fieldBindingIndex)
        {
            var key = fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture);
            if (constructorFieldSetterDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = constructorFieldSetterDescriptors.Count;
            constructorFieldSetterDescriptorIndices.Add(key, descriptorIndex);
            constructorFieldSetterDescriptors.Add($"    {{ {fieldBindingIndex}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonFieldGetterStringReturnDescriptor(int fieldBindingIndex)
        {
            var key = fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture);
            if (fieldGetterStringReturnDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldGetterStringReturnDescriptors.Count;
            fieldGetterStringReturnDescriptorIndices.Add(key, descriptorIndex);
            fieldGetterStringReturnDescriptors.Add($"    {{ {fieldBindingIndex}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonFieldBackedStringReturnDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (fieldBackedStringReturnDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldBackedStringReturnDescriptors.Count;
            fieldBackedStringReturnDescriptorIndices.Add(key, descriptorIndex);
            fieldBackedStringReturnDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonFieldArgumentStringReturnDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (fieldArgumentStringReturnDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldArgumentStringReturnDescriptors.Count;
            fieldArgumentStringReturnDescriptorIndices.Add(key, descriptorIndex);
            fieldArgumentStringReturnDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonStaticCallCtorGetterDescriptor(
            int fieldBindingIndex,
            string consoleWriteLineStringIcallLiteral,
            string echoLiteral,
            int echoLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                consoleWriteLineStringIcallLiteral,
                echoLiteral,
                echoLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (staticCallCtorGetterDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = staticCallCtorGetterDescriptors.Count;
            staticCallCtorGetterDescriptorIndices.Add(key, descriptorIndex);
            staticCallCtorGetterDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {consoleWriteLineStringIcallLiteral}, {echoLiteral}, {echoLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonStaticStringLiteralAppendDescriptor(
            string concatIcallLiteral,
            string appendedLiteral,
            int appendedLiteralByteCount)
        {
            var key = string.Join(
                "|",
                concatIcallLiteral,
                appendedLiteral,
                appendedLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (staticStringLiteralAppendDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = staticStringLiteralAppendDescriptors.Count;
            staticStringLiteralAppendDescriptorIndices.Add(key, descriptorIndex);
            staticStringLiteralAppendDescriptors.Add(
                $"    {{ {concatIcallLiteral}, {appendedLiteral}, {appendedLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonConstructorThenInstanceCallDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (constructorThenInstanceCallDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = constructorThenInstanceCallDescriptors.Count;
            constructorThenInstanceCallDescriptorIndices.Add(key, descriptorIndex);
            constructorThenInstanceCallDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonDelegateClosedTargetRelayDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount,
            string trailingLiteral,
            int trailingLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                trailingLiteral,
                trailingLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (delegateClosedTargetRelayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = delegateClosedTargetRelayDescriptors.Count;
            delegateClosedTargetRelayDescriptorIndices.Add(key, descriptorIndex);
            delegateClosedTargetRelayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u, {trailingLiteral}, {trailingLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayBoxingReferenceArrayDescriptor(
            int fieldBindingIndex,
            int boxedValueTypeIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int boxedInt32Value,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                boxedValueTypeIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                boxedInt32Value.ToString(System.Globalization.CultureInfo.InvariantCulture),
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayBoxingReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayBoxingReferenceArrayDescriptors.Count;
            arrayBoxingReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayBoxingReferenceArrayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {boxedValueTypeIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {boxedInt32Value}, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int sourceArrayLength,
            int targetArrayLength,
            int sourceStoreIndex,
            int sourceArrayIndex,
            int targetArrayIndex,
            int targetReadIndex,
            int copyLength,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                sourceArrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                targetArrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                sourceStoreIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                sourceArrayIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                targetArrayIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                targetReadIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                copyLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayCopyReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayCopyReferenceArrayDescriptors.Count;
            arrayCopyReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayCopyReferenceArrayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {sourceArrayLength}u, {targetArrayLength}u, {sourceStoreIndex}u, {sourceArrayIndex}u, {targetArrayIndex}u, {targetReadIndex}u, {copyLength}u, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(
            int stringTypeIndex,
            string concatIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int arrayLength,
            int arrayStoreIndex,
            int clearStartIndex,
            int clearLength,
            int arrayReadIndex,
            string storedLiteral,
            int storedLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                stringTypeIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                arrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayStoreIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                clearStartIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                clearLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayReadIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                storedLiteral,
                storedLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayClearReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayClearReferenceArrayDescriptors.Count;
            arrayClearReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayClearReferenceArrayDescriptors.Add(
                $"    {{ {stringTypeIndex}u, {concatIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {arrayLength}u, {arrayStoreIndex}u, {clearStartIndex}u, {clearLength}u, {arrayReadIndex}u, {storedLiteral}, {storedLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int arrayLength,
            int arrayStoreIndex,
            int reverseStartIndex,
            int reverseLength,
            int arrayReadIndex,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                arrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayStoreIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                reverseStartIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                reverseLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayReadIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayReverseReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayReverseReferenceArrayDescriptors.Count;
            arrayReverseReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayReverseReferenceArrayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {arrayLength}u, {arrayStoreIndex}u, {reverseStartIndex}u, {reverseLength}u, {arrayReadIndex}u, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public string[] GetFieldBindingDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldBindingDescriptors,
                "    { nullptr, 0u, 0u },");
        }

        public string[] GetTypeDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                typeDescriptors,
                "    { nullptr, 0u },");
        }

        public string[] GetConstructorFieldSetterDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                constructorFieldSetterDescriptors,
                "    { 0u },");
        }

        public string[] GetFieldGetterStringReturnDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldGetterStringReturnDescriptors,
                "    { 0u },");
        }

        public string[] GetFieldBackedStringReturnDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldBackedStringReturnDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetFieldArgumentStringReturnDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldArgumentStringReturnDescriptors,
                "    { 0u, nullptr, nullptr, 0u },");
        }

        public string[] GetStaticCallCtorGetterDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                staticCallCtorGetterDescriptors,
                "    { 0u, nullptr, nullptr, 0u },");
        }

        public string[] GetStaticStringLiteralAppendDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                staticStringLiteralAppendDescriptors,
                "    { nullptr, nullptr, 0u },");
        }

        public string[] GetConstructorThenInstanceCallDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                constructorThenInstanceCallDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetDelegateClosedTargetRelayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                delegateClosedTargetRelayDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayBoxingReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayBoxingReferenceArrayDescriptors,
                "    { 0u, 0u, nullptr, nullptr, nullptr, 0, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayCopyReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayCopyReferenceArrayDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, 0u, 0u, 0u, 0u, 0u, 0u, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayClearReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayClearReferenceArrayDescriptors,
                "    { 0u, nullptr, nullptr, 0u, 0u, 0u, 0u, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayReverseReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayReverseReferenceArrayDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, 0u, 0u, 0u, 0u, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public int FieldBindingDescriptorCount => fieldBindingDescriptors.Count;

        public int TypeDescriptorCount => typeDescriptors.Count;

        public int ConstructorFieldSetterDescriptorCount => constructorFieldSetterDescriptors.Count;

        public int FieldGetterStringReturnDescriptorCount => fieldGetterStringReturnDescriptors.Count;

        public int FieldBackedStringReturnDescriptorCount => fieldBackedStringReturnDescriptors.Count;

        public int FieldArgumentStringReturnDescriptorCount => fieldArgumentStringReturnDescriptors.Count;

        public int StaticCallCtorGetterDescriptorCount => staticCallCtorGetterDescriptors.Count;

        public int StaticStringLiteralAppendDescriptorCount => staticStringLiteralAppendDescriptors.Count;

        public int ConstructorThenInstanceCallDescriptorCount => constructorThenInstanceCallDescriptors.Count;

        public int DelegateClosedTargetRelayDescriptorCount => delegateClosedTargetRelayDescriptors.Count;

        public int ArrayBoxingReferenceArrayDescriptorCount => arrayBoxingReferenceArrayDescriptors.Count;

        public int ArrayReverseReferenceArrayDescriptorCount => arrayReverseReferenceArrayDescriptors.Count;

        public int ArrayClearReferenceArrayDescriptorCount => arrayClearReferenceArrayDescriptors.Count;

        public int ArrayCopyReferenceArrayDescriptorCount => arrayCopyReferenceArrayDescriptors.Count;

        private static string[] GetDescriptorArrayOrPlaceholder(List<string> descriptors, string placeholder)
        {
            return descriptors.Count > 0
                ? [.. descriptors]
                : [placeholder];
        }
    }

    private sealed record AssemblyFullClosureRuntimeSkeletonEmission(
        IReadOnlyList<NativeReferenceGeneratedSource> GeneratedSources,
        IReadOnlyList<NativeReferenceGeneratedArtifactRef> GeneratedArtifacts,
        IReadOnlyList<AuditTranslationUnitPageArtifact> TranslationUnitPages,
        int TranslationUnitMethodCount,
        string? PreferredAssemblyDispatchSubjectId);

}
