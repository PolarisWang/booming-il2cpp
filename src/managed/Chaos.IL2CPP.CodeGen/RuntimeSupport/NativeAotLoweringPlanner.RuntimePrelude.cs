using System.Text;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{

    private static void EmitRuntimePrelude(
        StringBuilder builder,
        IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers,
        StaticFieldDataSupportModel staticFieldDataSupport)
    {
        builder.AppendLine("static_assert(sizeof(std::intptr_t) == sizeof(std::uint64_t), \"native-aot ABI lowering requires 64-bit intptr_t\");");
        builder.AppendLine();
        builder.AppendLine("constexpr std::intptr_t chaos_managed_pointer_local_slot_tag = 1;");
        builder.AppendLine("constexpr std::intptr_t chaos_raw_int32_pointer_tag = 2;");
        builder.AppendLine();
        builder.AppendLine("struct chaos_managed_exception");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t object_value = 0;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("template <typename TAction>");
        builder.AppendLine("struct chaos_finally_scope_guard");
        builder.AppendLine("{");
        builder.AppendLine("    explicit chaos_finally_scope_guard(TAction init_action)");
        builder.AppendLine("        : action(init_action)");
        builder.AppendLine("    {");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    chaos_finally_scope_guard(const chaos_finally_scope_guard&) = delete;");
        builder.AppendLine("    chaos_finally_scope_guard& operator=(const chaos_finally_scope_guard&) = delete;");
        builder.AppendLine();
        builder.AppendLine("    ~chaos_finally_scope_guard()");
        builder.AppendLine("    {");
        builder.AppendLine("        if (active)");
        builder.AppendLine("        {");
        builder.AppendLine("            action();");
        builder.AppendLine("        }");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    TAction action;");
        builder.AppendLine("    bool active = true;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("template <typename TAction>");
        builder.AppendLine("chaos_finally_scope_guard<TAction> chaos_make_finally_scope_guard(TAction action)");
        builder.AppendLine("{");
        builder.AppendLine("    return chaos_finally_scope_guard<TAction>(action);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_float32(float value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint32_t bits = 0;");
        builder.AppendLine("    std::memcpy(&bits, &value, sizeof(value));");
        builder.AppendLine("    return static_cast<std::intptr_t>(static_cast<std::uint64_t>(bits));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("float chaos_load_float32(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto bits = static_cast<std::uint32_t>(static_cast<std::uint64_t>(value));");
        builder.AppendLine("    float result = 0.0f;");
        builder.AppendLine("    std::memcpy(&result, &bits, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_float64(double value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint64_t bits = 0;");
        builder.AppendLine("    std::memcpy(&bits, &value, sizeof(value));");
        builder.AppendLine("    std::intptr_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &bits, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("double chaos_load_float64(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint64_t bits = 0;");
        builder.AppendLine("    std::memcpy(&bits, &value, sizeof(bits));");
        builder.AppendLine("    double result = 0.0;");
        builder.AppendLine("    std::memcpy(&result, &bits, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_int64(std::int64_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int64_t chaos_load_int64(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::int64_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_store_uint64(std::uint64_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::uint64_t chaos_load_uint64(std::intptr_t value) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    std::uint64_t result = 0;");
        builder.AppendLine("    std::memcpy(&result, &value, sizeof(result));");
        builder.AppendLine("    return result;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_wrap_add(std::int32_t left, std::int32_t right) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    return static_cast<std::int32_t>(");
        builder.AppendLine("        static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_wrap_sub(std::int32_t left, std::int32_t right) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    return static_cast<std::int32_t>(");
        builder.AppendLine("        static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_wrap_mul(std::int32_t left, std::int32_t right) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    return static_cast<std::int32_t>(");
        builder.AppendLine("        static_cast<std::uint32_t>(left) * static_cast<std::uint32_t>(right));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_div(std::int32_t left, std::int32_t right)");
        builder.AppendLine("{");
        builder.AppendLine("    if (right == 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (left == std::numeric_limits<std::int32_t>::min() && right == -1)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::int32_t>(left / right);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_rem(std::int32_t left, std::int32_t right)");
        builder.AppendLine("{");
        builder.AppendLine("    if (right == 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    if (left == std::numeric_limits<std::int32_t>::min() && right == -1)");
        builder.AppendLine("    {");
        builder.AppendLine("        return 0;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::int32_t>(left % right);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_shift_left_int32(std::int32_t value, std::int32_t amount) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto shift = static_cast<std::uint32_t>(amount) & 31U;");
        builder.AppendLine("    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) << shift);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_shift_right_int32(std::int32_t value, std::int32_t amount) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto shift = static_cast<std::uint32_t>(amount) & 31U;");
        builder.AppendLine("    if (shift == 0U)");
        builder.AppendLine("    {");
        builder.AppendLine("        return value;");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto bits = static_cast<std::uint32_t>(value);");
        builder.AppendLine("    if (value >= 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        return static_cast<std::int32_t>(bits >> shift);");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    const auto fill = std::numeric_limits<std::uint32_t>::max() << (32U - shift);");
        builder.AppendLine("    return static_cast<std::int32_t>((bits >> shift) | fill);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_shift_right_un_int32(std::int32_t value, std::int32_t amount) noexcept");
        builder.AppendLine("{");
        builder.AppendLine("    const auto shift = static_cast<std::uint32_t>(amount) & 31U;");
        builder.AppendLine("    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) >> shift);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_checked_conv_ovf_i1(std::intptr_t value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (value < static_cast<std::intptr_t>(std::numeric_limits<std::int8_t>::min()) ||");
        builder.AppendLine("        value > static_cast<std::intptr_t>(std::numeric_limits<std::int8_t>::max()))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::intptr_t>(static_cast<std::int8_t>(value));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_checked_conv_ovf_u1(std::intptr_t value)");
        builder.AppendLine("{");
        builder.AppendLine("    if (value < static_cast<std::intptr_t>(0) ||");
        builder.AppendLine("        value > static_cast<std::intptr_t>(std::numeric_limits<std::uint8_t>::max()))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return static_cast<std::intptr_t>(static_cast<std::uint8_t>(value));");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t* chaos_resolve_native_int_slot(std::intptr_t chaos_address)");
        builder.AppendLine("{");
        builder.AppendLine("    if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
        builder.AppendLine("    {");
        builder.AppendLine(
            "        return reinterpret_cast<std::intptr_t*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
        builder.AppendLine("    }");
        builder.AppendLine();
        builder.AppendLine("    return reinterpret_cast<std::intptr_t*>(chaos_address);");
        builder.AppendLine("}");
        builder.AppendLine();

        if (externalRuntimeHelpers.Any(helper => IsSpanRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitSpanRuntimePrelude(builder, staticFieldDataSupport);
        }

        if (externalRuntimeHelpers.Any(helper => IsCollectionRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitCollectionRuntimePrelude(builder);
        }

        if (externalRuntimeHelpers.Any(helper => IsMonitorRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitMonitorRuntimePrelude(builder);
        }

        if (externalRuntimeHelpers.Any(helper => IsThreadRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitThreadRuntimePrelude(builder);
        }

        if (externalRuntimeHelpers.Any(helper => IsAsyncRuntimeHelperSubjectId(helper.SubjectId)))
        {
            EmitAsyncRuntimePrelude(builder);
        }
    }

    private static void EmitSpanRuntimePrelude(
        StringBuilder builder,
        StaticFieldDataSupportModel staticFieldDataSupport)
    {
        foreach (var entry in staticFieldDataSupport.EntriesBySubjectId.Values.OrderBy(value => value.FieldSubjectId, StringComparer.Ordinal))
        {
            builder.AppendLine($"const std::uint8_t chaos_static_field_data_{SanitizeSubjectId(entry.FieldSubjectId)}[] =");
            builder.AppendLine("{");
            if (entry.Bytes.Count == 0)
            {
                builder.AppendLine("    0,");
            }
            else
            {
                foreach (var chunk in entry.Bytes.Chunk(16))
                {
                    builder.Append("    ");
                    builder.Append(string.Join(", ", chunk.Select(value => $"0x{value:X2}")));
                    builder.AppendLine(",");
                }
            }

            builder.AppendLine("};");
            builder.AppendLine();
        }

        builder.AppendLine("bool chaos_try_get_static_field_data(");
        builder.AppendLine("    std::intptr_t chaos_field_handle,");
        builder.AppendLine("    const std::uint8_t*& chaos_data,");
        builder.AppendLine("    std::size_t& chaos_size)");
        builder.AppendLine("{");
        foreach (var entry in staticFieldDataSupport.EntriesBySubjectId.Values.OrderBy(value => value.FieldSubjectId, StringComparer.Ordinal))
        {
            builder.AppendLine($"    if (chaos_field_handle == {GetFieldHandleLiteral(entry.FieldSubjectId)})");
            builder.AppendLine("    {");
            builder.AppendLine($"        chaos_data = chaos_static_field_data_{SanitizeSubjectId(entry.FieldSubjectId)};");
            builder.AppendLine($"        chaos_size = sizeof(chaos_static_field_data_{SanitizeSubjectId(entry.FieldSubjectId)});");
            builder.AppendLine("        return true;");
            builder.AppendLine("    }");
        }

        builder.AppendLine("    chaos_data = nullptr;");
        builder.AppendLine("    chaos_size = 0;");
        builder.AppendLine("    return false;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("void* chaos_resolve_block_copy_address(std::intptr_t chaos_address)");
        builder.AppendLine("{");
        builder.AppendLine("    if ((chaos_address & chaos_raw_int32_pointer_tag) != 0)");
        builder.AppendLine("    {");
        builder.AppendLine(
            "        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_raw_int32_pointer_tag));");
        builder.AppendLine("    }");
        builder.AppendLine("    if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
        builder.AppendLine("    {");
        builder.AppendLine(
            "        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
        builder.AppendLine("    }");
        builder.AppendLine("    return reinterpret_cast<void*>(chaos_address);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("enum class chaos_span_storage_kind : std::uint8_t");
        builder.AppendLine("{");
        builder.AppendLine("    array = 1,");
        builder.AppendLine("    rawInt32 = 2,");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("struct chaos_span_runtime_entry");
        builder.AppendLine("{");
        builder.AppendLine("    chaos_span_storage_kind storage_kind = chaos_span_storage_kind::array;");
        builder.AppendLine("    std::intptr_t owner_handle = 0;");
        builder.AppendLine("    std::intptr_t data_handle = 0;");
        builder.AppendLine("    std::int32_t start = 0;");
        builder.AppendLine("    std::int32_t length = 0;");
        builder.AppendLine("    bool read_only = false;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("struct chaos_memory_runtime_entry");
        builder.AppendLine("{");
        builder.AppendLine("    std::intptr_t array_handle = 0;");
        builder.AppendLine("    std::int32_t start = 0;");
        builder.AppendLine("    std::int32_t length = 0;");
        builder.AppendLine("};");
        builder.AppendLine();
        builder.AppendLine("chaos_span_runtime_entry* chaos_require_span_runtime_entry(std::intptr_t chaos_handle)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_handle == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    return reinterpret_cast<chaos_span_runtime_entry*>(chaos_handle);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("chaos_memory_runtime_entry* chaos_require_memory_runtime_entry(std::intptr_t chaos_handle)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_handle == static_cast<std::intptr_t>(0))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    return reinterpret_cast<chaos_memory_runtime_entry*>(chaos_handle);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_create_array_span_int32(std::intptr_t chaos_array_value, std::int32_t chaos_start, std::int32_t chaos_length)");
        builder.AppendLine("{");
        builder.AppendLine("    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);");
        builder.AppendLine("    if (chaos_array == nullptr ||");
        builder.AppendLine("        chaos_array->element_type_shape != chaos_type_shape_value ||");
        builder.AppendLine("        chaos_start < 0 ||");
        builder.AppendLine("        chaos_length < 0 ||");
        builder.AppendLine("        chaos_start > static_cast<std::int32_t>(chaos_array->length) ||");
        builder.AppendLine("        chaos_length > static_cast<std::int32_t>(chaos_array->length) - chaos_start)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    auto* chaos_entry = new chaos_span_runtime_entry{};");
        builder.AppendLine("    chaos_entry->storage_kind = chaos_span_storage_kind::array;");
        builder.AppendLine("    chaos_entry->owner_handle = chaos_array_value;");
        builder.AppendLine("    chaos_entry->start = chaos_start;");
        builder.AppendLine("    chaos_entry->length = chaos_length;");
        builder.AppendLine("    return reinterpret_cast<std::intptr_t>(chaos_entry);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_create_raw_span_int32(void* chaos_data, std::int32_t chaos_length, bool chaos_read_only)");
        builder.AppendLine("{");
        builder.AppendLine("    if (chaos_length < 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    auto* chaos_entry = new chaos_span_runtime_entry{};");
        builder.AppendLine("    chaos_entry->storage_kind = chaos_span_storage_kind::rawInt32;");
        builder.AppendLine("    chaos_entry->data_handle = reinterpret_cast<std::intptr_t>(chaos_data);");
        builder.AppendLine("    chaos_entry->length = chaos_length;");
        builder.AppendLine("    chaos_entry->read_only = chaos_read_only;");
        builder.AppendLine("    return reinterpret_cast<std::intptr_t>(chaos_entry);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_create_memory_int32(std::intptr_t chaos_array_value)");
        builder.AppendLine("{");
        builder.AppendLine("    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);");
        builder.AppendLine("    if (chaos_array == nullptr || chaos_array->element_type_shape != chaos_type_shape_value)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    auto* chaos_entry = new chaos_memory_runtime_entry{};");
        builder.AppendLine("    chaos_entry->array_handle = chaos_array_value;");
        builder.AppendLine("    chaos_entry->length = static_cast<std::int32_t>(chaos_array->length);");
        builder.AppendLine("    return reinterpret_cast<std::intptr_t>(chaos_entry);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_memory_int32_get_span(std::intptr_t chaos_memory_handle)");
        builder.AppendLine("{");
        builder.AppendLine("    auto* chaos_memory = chaos_require_memory_runtime_entry(chaos_memory_handle);");
        builder.AppendLine("    return chaos_create_array_span_int32(chaos_memory->array_handle, chaos_memory->start, chaos_memory->length);");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::int32_t chaos_span_int32_get_length(std::intptr_t chaos_span_handle)");
        builder.AppendLine("{");
        builder.AppendLine("    return chaos_require_span_runtime_entry(chaos_span_handle)->length;");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_span_int32_get_item_address(std::intptr_t chaos_span_handle, std::int32_t chaos_index)");
        builder.AppendLine("{");
        builder.AppendLine("    auto* chaos_span = chaos_require_span_runtime_entry(chaos_span_handle);");
        builder.AppendLine("    if (chaos_index < 0 || chaos_index >= chaos_span->length)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    switch (chaos_span->storage_kind)");
        builder.AppendLine("    {");
        builder.AppendLine("        case chaos_span_storage_kind::array:");
        builder.AppendLine("        {");
        builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_span->owner_handle);");
        builder.AppendLine("            return reinterpret_cast<std::intptr_t>(&chaos_array->elements[static_cast<std::size_t>(chaos_span->start + chaos_index)]);");
        builder.AppendLine("        }");
        builder.AppendLine("        case chaos_span_storage_kind::rawInt32:");
        builder.AppendLine("        {");
        builder.AppendLine("            auto* chaos_item = reinterpret_cast<std::int32_t*>(chaos_span->data_handle) + chaos_index;");
        builder.AppendLine("            return reinterpret_cast<std::intptr_t>(chaos_item) | chaos_raw_int32_pointer_tag;");
        builder.AppendLine("        }");
        builder.AppendLine("        default:");
        builder.AppendLine("            std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("void chaos_initialize_array_from_field_data_int32(std::intptr_t chaos_array_value, std::intptr_t chaos_field_handle)");
        builder.AppendLine("{");
        builder.AppendLine("    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);");
        builder.AppendLine("    if (chaos_array == nullptr || chaos_array->element_type_shape != chaos_type_shape_value)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    const std::uint8_t* chaos_bytes = nullptr;");
        builder.AppendLine("    std::size_t chaos_size = 0;");
        builder.AppendLine("    if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size) ||");
        builder.AppendLine("        (chaos_size % sizeof(std::int32_t)) != 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    const auto chaos_element_count = static_cast<std::int32_t>(chaos_size / sizeof(std::int32_t));");
        builder.AppendLine("    if (chaos_element_count > static_cast<std::int32_t>(chaos_array->length))");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    for (std::int32_t chaos_index = 0; chaos_index < chaos_element_count; chaos_index++)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::int32_t chaos_value = 0;");
        builder.AppendLine("        std::memcpy(&chaos_value, chaos_bytes + (static_cast<std::size_t>(chaos_index) * sizeof(std::int32_t)), sizeof(chaos_value));");
        builder.AppendLine("        chaos_array->elements[static_cast<std::size_t>(chaos_index)] = static_cast<std::intptr_t>(chaos_value);");
        builder.AppendLine("    }");
        builder.AppendLine("}");
        builder.AppendLine();
        builder.AppendLine("std::intptr_t chaos_create_field_data_span_int32(std::intptr_t chaos_field_handle)");
        builder.AppendLine("{");
        builder.AppendLine("    const std::uint8_t* chaos_bytes = nullptr;");
        builder.AppendLine("    std::size_t chaos_size = 0;");
        builder.AppendLine("    if (!chaos_try_get_static_field_data(chaos_field_handle, chaos_bytes, chaos_size) ||");
        builder.AppendLine("        (chaos_size % sizeof(std::int32_t)) != 0)");
        builder.AppendLine("    {");
        builder.AppendLine("        std::abort();");
        builder.AppendLine("    }");
        builder.AppendLine("    return chaos_create_raw_span_int32(const_cast<std::uint8_t*>(chaos_bytes), static_cast<std::int32_t>(chaos_size / sizeof(std::int32_t)), true);");
        builder.AppendLine("}");
        builder.AppendLine();
    }
}

