namespace chaos::il2cpp::runtime_core {
namespace {

static bool CharIsAsciiGeneric(CHAOS_IL2CPP_UINT16 value) { return value <= 0x7Fu; }
static bool CharIsAsciiIntrinsicImpl(CHAOS_IL2CPP_UINT16 value) { return (value & 0xFF80u) == 0u; }

bool CharIsAscii(CHAOS_IL2CPP_UINT16 value) {
    return kCharClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? CharIsAsciiIntrinsicImpl(value) : CharIsAsciiGeneric(value);
}

bool CharIsAsciiDigit(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('0')) <= 9u;
}

bool CharIsAsciiHexDigitLower(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('a')) <= 5u;
}

bool CharIsAsciiHexDigitUpper(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('A')) <= 5u;
}

bool CharIsAsciiHexDigit(CHAOS_IL2CPP_UINT16 value) {
    return CharIsAsciiDigit(value) || CharIsAsciiHexDigitLower(value) || CharIsAsciiHexDigitUpper(value);
}

bool CharIsAsciiLetterLower(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('a')) <= 25u;
}

bool CharIsAsciiLetterUpper(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('A')) <= 25u;
}

bool CharIsAsciiLetter(CHAOS_IL2CPP_UINT16 value) {
    return CharIsAsciiLetterLower(value) || CharIsAsciiLetterUpper(value);
}

bool CharIsAsciiLetterOrDigit(CHAOS_IL2CPP_UINT16 value) {
    return CharIsAsciiLetter(value) || CharIsAsciiDigit(value);
}

bool CharIsBetween(CHAOS_IL2CPP_UINT16 value, CHAOS_IL2CPP_UINT16 lower_bound, CHAOS_IL2CPP_UINT16 upper_bound) {
    return value >= lower_bound && value <= upper_bound;
}

static bool CharIsLatin1Generic(CHAOS_IL2CPP_UINT16 value) { return value <= 0xFFu; }
static bool CharIsLatin1IntrinsicImpl(CHAOS_IL2CPP_UINT16 value) { return (value & 0xFF00u) == 0u; }

bool CharIsLatin1(CHAOS_IL2CPP_UINT16 value) {
    return kCharClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? CharIsLatin1IntrinsicImpl(value) : CharIsLatin1Generic(value);
}

bool CharIsHighSurrogate(CHAOS_IL2CPP_UINT16 value) { return CharIsBetween(value, 0xD800u, 0xDBFFu); }
bool CharIsLowSurrogate(CHAOS_IL2CPP_UINT16 value) { return CharIsBetween(value, 0xDC00u, 0xDFFFu); }

bool CharIsSeparatorLatin1(CHAOS_IL2CPP_UINT16 value) {
    return value == 0x20u || value == 0xA0u;
}

bool CharIsSurrogate(CHAOS_IL2CPP_UINT16 value) { return CharIsBetween(value, 0xD800u, 0xDFFFu); }

bool CharIsSurrogatePair(CHAOS_IL2CPP_UINT16 high_surrogate, CHAOS_IL2CPP_UINT16 low_surrogate) {
    return CharIsHighSurrogate(high_surrogate) && CharIsLowSurrogate(low_surrogate);
}

bool CharIsWhiteSpaceLatin1(CHAOS_IL2CPP_UINT16 value) {
    return value == 0x20u ||
           static_cast<CHAOS_IL2CPP_UINT16>(value - 0x09u) <= static_cast<CHAOS_IL2CPP_UINT16>(0x0Du - 0x09u) ||
           value == 0x85u || value == 0xA0u;
}

CHAOS_IL2CPP_INT32 CharCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    return 0;
}

bool CharEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return left_value == right_value;
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core