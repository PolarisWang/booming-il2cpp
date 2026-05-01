#ifndef CHAOS_IL2CPP_CHAR_CLASSIFICATION_H_
#define CHAOS_IL2CPP_CHAR_CLASSIFICATION_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

bool CharIsAscii(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiDigit(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiHexDigit(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiHexDigitLower(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiHexDigitUpper(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetter(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetterLower(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetterOrDigit(CHAOS_IL2CPP_UINT16 value);
bool CharIsAsciiLetterUpper(CHAOS_IL2CPP_UINT16 value);
bool CharIsBetween(CHAOS_IL2CPP_UINT16 value, CHAOS_IL2CPP_UINT16 lower_bound, CHAOS_IL2CPP_UINT16 upper_bound);
bool CharIsHighSurrogate(CHAOS_IL2CPP_UINT16 value);
bool CharIsLatin1(CHAOS_IL2CPP_UINT16 value);
bool CharIsLowSurrogate(CHAOS_IL2CPP_UINT16 value);
bool CharIsSeparatorLatin1(CHAOS_IL2CPP_UINT16 value);
bool CharIsSurrogate(CHAOS_IL2CPP_UINT16 value);
bool CharIsSurrogatePair(CHAOS_IL2CPP_UINT16 high_surrogate, CHAOS_IL2CPP_UINT16 low_surrogate);
bool CharIsWhiteSpaceLatin1(CHAOS_IL2CPP_UINT16 value);
CHAOS_IL2CPP_INT32 CharCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);
bool CharEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_CHAR_CLASSIFICATION_H_
