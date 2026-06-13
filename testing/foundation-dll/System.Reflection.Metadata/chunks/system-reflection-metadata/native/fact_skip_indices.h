static inline bool IsFactMethodSkipped(int methodIndex) {
    static const int kSkipped[] = {246, 247, 251, 252, 279, 280, 282, 283, 284, 285, 286, 414, 416, 418, 419, 420, 422, 423, 424};
    static const int kCount = sizeof(kSkipped) / sizeof(kSkipped[0]);
    for (int i = 0; i < kCount; i++) {
        if (methodIndex == kSkipped[i]) return true;
    }
    return false;
}
