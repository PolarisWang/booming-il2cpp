static inline bool IsFactMethodSkipped(int methodIndex) {
    static const int kSkipped[] = {840, 841, 842, 1683, 1684, 1702, 1703, 1704, 1705, 1706, 1717, 1718, 1719, 1720, 1721, 1722, 1723, 1724, 1725, 1729, 1730, 1731, 1732, 1733, 1734, 1735, 1736, 1737, 1738, 1739, 1740, 1741, 1742, 1743};
    static const int kCount = sizeof(kSkipped) / sizeof(kSkipped[0]);
    for (int i = 0; i < kCount; i++) {
        if (methodIndex == kSkipped[i]) return true;
    }
    return false;
}
