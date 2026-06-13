static inline bool IsFactMethodSkipped(int methodIndex) {
    static const int kSkipped[] = {237, 239, 240};
    static const int kCount = sizeof(kSkipped) / sizeof(kSkipped[0]);
    for (int i = 0; i < kCount; i++) {
        if (methodIndex == kSkipped[i]) return true;
    }
    return false;
}
