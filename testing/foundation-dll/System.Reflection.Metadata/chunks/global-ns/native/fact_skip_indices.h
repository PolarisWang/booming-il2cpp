static inline bool IsFactMethodSkipped(int methodIndex) {
    static const int kSkipped[] = {108, 109, 110, 143, 144};
    static const int kCount = sizeof(kSkipped) / sizeof(kSkipped[0]);
    for (int i = 0; i < kCount; i++) {
        if (methodIndex == kSkipped[i]) return true;
    }
    return false;
}
