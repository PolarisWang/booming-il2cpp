static inline bool IsFactMethodSkipped(int methodIndex) {
    static const int kSkipped[] = {104, 105, 106, 275, 276, 277, 278, 279};
    static const int kCount = sizeof(kSkipped) / sizeof(kSkipped[0]);
    for (int i = 0; i < kCount; i++) {
        if (methodIndex == kSkipped[i]) return true;
    }
    return false;
}
