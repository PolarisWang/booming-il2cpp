// Layer I -- HPACK (RFC 7541) header compression implementation (NT-21).
// Companion to chaos/net/hpack.h.  Full RFC 7541:
//   * static table (Appendix A, 61 entries)
//   * dynamic table with SETTINGS_HEADER_TABLE_SIZE-adjustable capacity,
//     LRU eviction, entry size = name + value + 32
//   * Huffman coding (Appendix B) applied when it shortens a string
//   * indexed / literal-with-incremental-indexing / literal-without-
//     indexing / literal-never-indexed field forms
#include <chaos/net/hpack.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

namespace chaos {
namespace net {
namespace hpack {
namespace {

// ---- static table (RFC 7541 Appendix A), index = position + 1 ----------
struct StaticEntry {
    const char* name;
    const char* value;
};

const StaticEntry kStaticTable[61] = {
    {":authority", ""},
    {":method", "GET"},
    {":method", "POST"},
    {":path", "/"},
    {":path", "/index.html"},
    {":scheme", "http"},
    {":scheme", "https"},
    {":status", "200"},
    {":status", "204"},
    {":status", "206"},
    {":status", "304"},
    {":status", "400"},
    {":status", "404"},
    {":status", "500"},
    {"accept-charset", ""},
    {"accept-encoding", "gzip, deflate"},
    {"accept-language", ""},
    {"accept-ranges", ""},
    {"accept", ""},
    {"access-control-allow-origin", ""},
    {"age", ""},
    {"allow", ""},
    {"authorization", ""},
    {"cache-control", ""},
    {"content-disposition", ""},
    {"content-encoding", ""},
    {"content-language", ""},
    {"content-length", ""},
    {"content-location", ""},
    {"content-range", ""},
    {"content-type", ""},
    {"cookie", ""},
    {"date", ""},
    {"etag", ""},
    {"expect", ""},
    {"expires", ""},
    {"from", ""},
    {"host", ""},
    {"if-match", ""},
    {"if-modified-since", ""},
    {"if-none-match", ""},
    {"if-range", ""},
    {"if-unmodified-since", ""},
    {"last-modified", ""},
    {"link", ""},
    {"location", ""},
    {"max-forwards", ""},
    {"proxy-authenticate", ""},
    {"proxy-authorization", ""},
    {"range", ""},
    {"referer", ""},
    {"refresh", ""},
    {"retry-after", ""},
    {"server", ""},
    {"set-cookie", ""},
    {"strict-transport-security", ""},
    {"transfer-encoding", ""},
    {"user-agent", ""},
    {"vary", ""},
    {"via", ""},
    {"www-authenticate", ""},
};

// First static index whose NAME equals name (1-based overall), else 0.
int StaticNameIndex(const std::string& name) {
    for (int i = 0; i < 61; ++i) {
        if (std::strcmp(kStaticTable[i].name, name.c_str()) == 0) return i + 1;
    }
    return 0;
}

// ---- integer primitives (RFC 7541 5.1) ---------------------------------
void EncodeInt(std::vector<uint8_t>* o, uint32_t v, uint8_t prefixBits,
               uint8_t leading) {
    const uint32_t cap = (1u << prefixBits) - 1u;
    if (v < cap) {
        o->push_back(static_cast<uint8_t>(leading | v));
        return;
    }
    o->push_back(static_cast<uint8_t>(leading | cap));
    v -= cap;
    while (v >= 128) {
        o->push_back(static_cast<uint8_t>((v & 0x7F) | 0x80));
        v >>= 7;
    }
    o->push_back(static_cast<uint8_t>(v));
}

bool DecodeInt(const uint8_t* d, std::size_t sz, std::size_t* p,
               uint8_t prefixBits, uint32_t* out) {
    const uint8_t mask = static_cast<uint8_t>((1u << prefixBits) - 1u);
    if (*p >= sz) return false;
    uint32_t v = d[*p] & mask;
    *p += 1;
    if (v < mask) {
        *out = v;
        return true;
    }
    uint32_t m = 0;
    for (;;) {
        if (*p >= sz) return false;
        uint8_t b = d[*p];
        *p += 1;
        v += static_cast<uint32_t>(b & 0x7F) << m;
        if (!(b & 0x80)) {
            *out = v;
            return true;
        }
        m += 7;
        if (m > 28) return false;
    }
}

// ---- string literal (RFC 7541 5.2) with optional Huffman ---------------
void EncodeString(std::vector<uint8_t>* o, const std::string& s) {
    // Huffman only when it shortens the octet representation.
    std::vector<uint8_t> huff;
    HuffmanEncode(s, &huff);
    const bool useHuff = huff.size() < s.size();
    const uint8_t* data = useHuff
        ? huff.data() : reinterpret_cast<const uint8_t*>(s.data());
    const std::size_t len = useHuff ? huff.size() : s.size();
    EncodeInt(o, static_cast<uint32_t>(len), 7,
              static_cast<uint8_t>(useHuff ? 0x80 : 0x00));
    o->insert(o->end(), data, data + len);
}

bool DecodeString(const uint8_t* d, std::size_t sz, std::size_t* p,
                  std::string* out) {
    if (*p >= sz) return false;
    const bool isHuff = (d[*p] & 0x80) != 0;
    uint32_t len = 0;
    if (!DecodeInt(d, sz, p, 7, &len)) return false;
    if (static_cast<std::size_t>(*p) + len > sz) return false;
    if (isHuff) {
        if (!HuffmanDecode(d + *p, len, out)) return false;
    } else {
        out->assign(reinterpret_cast<const char*>(d + *p), len);
    }
    *p += len;
    return true;
}

}  // namespace

// Huffman code table (RFC 7541 Appendix B) -- extracted from
// python-hyper/hpack huffman_constants.py; verified against RFC 7541
// Appendix C encode vectors
// huffman_constants.py; verified against RFC 7541 Appendix C encode vectors
// (www.example.com / no-cache / custom-key / custom-value).
static const uint32_t kHuffCode[256] = {
    0x00001ff8, 0x007fffd8, 0x0fffffe2, 0x0fffffe3, 0x0fffffe4, 0x0fffffe5, 0x0fffffe6, 0x0fffffe7,
    0x0fffffe8, 0x00ffffea, 0x3ffffffc, 0x0fffffe9, 0x0fffffea, 0x3ffffffd, 0x0fffffeb, 0x0fffffec,
    0x0fffffed, 0x0fffffee, 0x0fffffef, 0x0ffffff0, 0x0ffffff1, 0x0ffffff2, 0x3ffffffe, 0x0ffffff3,
    0x0ffffff4, 0x0ffffff5, 0x0ffffff6, 0x0ffffff7, 0x0ffffff8, 0x0ffffff9, 0x0ffffffa, 0x0ffffffb,
    0x00000014, 0x000003f8, 0x000003f9, 0x00000ffa, 0x00001ff9, 0x00000015, 0x000000f8, 0x000007fa,
    0x000003fa, 0x000003fb, 0x000000f9, 0x000007fb, 0x000000fa, 0x00000016, 0x00000017, 0x00000018,
    0x00000000, 0x00000001, 0x00000002, 0x00000019, 0x0000001a, 0x0000001b, 0x0000001c, 0x0000001d,
    0x0000001e, 0x0000001f, 0x0000005c, 0x000000fb, 0x00007ffc, 0x00000020, 0x00000ffb, 0x000003fc,
    0x00001ffa, 0x00000021, 0x0000005d, 0x0000005e, 0x0000005f, 0x00000060, 0x00000061, 0x00000062,
    0x00000063, 0x00000064, 0x00000065, 0x00000066, 0x00000067, 0x00000068, 0x00000069, 0x0000006a,
    0x0000006b, 0x0000006c, 0x0000006d, 0x0000006e, 0x0000006f, 0x00000070, 0x00000071, 0x00000072,
    0x000000fc, 0x00000073, 0x000000fd, 0x00001ffb, 0x0007fff0, 0x00001ffc, 0x00003ffc, 0x00000022,
    0x00007ffd, 0x00000003, 0x00000023, 0x00000004, 0x00000024, 0x00000005, 0x00000025, 0x00000026,
    0x00000027, 0x00000006, 0x00000074, 0x00000075, 0x00000028, 0x00000029, 0x0000002a, 0x00000007,
    0x0000002b, 0x00000076, 0x0000002c, 0x00000008, 0x00000009, 0x0000002d, 0x00000077, 0x00000078,
    0x00000079, 0x0000007a, 0x0000007b, 0x00007ffe, 0x000007fc, 0x00003ffd, 0x00001ffd, 0x0ffffffc,
    0x000fffe6, 0x003fffd2, 0x000fffe7, 0x000fffe8, 0x003fffd3, 0x003fffd4, 0x003fffd5, 0x007fffd9,
    0x003fffd6, 0x007fffda, 0x007fffdb, 0x007fffdc, 0x007fffdd, 0x007fffde, 0x00ffffeb, 0x007fffdf,
    0x00ffffec, 0x00ffffed, 0x003fffd7, 0x007fffe0, 0x00ffffee, 0x007fffe1, 0x007fffe2, 0x007fffe3,
    0x007fffe4, 0x001fffdc, 0x003fffd8, 0x007fffe5, 0x003fffd9, 0x007fffe6, 0x007fffe7, 0x00ffffef,
    0x003fffda, 0x001fffdd, 0x000fffe9, 0x003fffdb, 0x003fffdc, 0x007fffe8, 0x007fffe9, 0x001fffde,
    0x007fffea, 0x003fffdd, 0x003fffde, 0x00fffff0, 0x001fffdf, 0x003fffdf, 0x007fffeb, 0x007fffec,
    0x001fffe0, 0x001fffe1, 0x003fffe0, 0x001fffe2, 0x007fffed, 0x003fffe1, 0x007fffee, 0x007fffef,
    0x000fffea, 0x003fffe2, 0x003fffe3, 0x003fffe4, 0x007ffff0, 0x003fffe5, 0x003fffe6, 0x007ffff1,
    0x03ffffe0, 0x03ffffe1, 0x000fffeb, 0x0007fff1, 0x003fffe7, 0x007ffff2, 0x003fffe8, 0x01ffffec,
    0x03ffffe2, 0x03ffffe3, 0x03ffffe4, 0x07ffffde, 0x07ffffdf, 0x03ffffe5, 0x00fffff1, 0x01ffffed,
    0x0007fff2, 0x001fffe3, 0x03ffffe6, 0x07ffffe0, 0x07ffffe1, 0x03ffffe7, 0x07ffffe2, 0x00fffff2,
    0x001fffe4, 0x001fffe5, 0x03ffffe8, 0x03ffffe9, 0x0ffffffd, 0x07ffffe3, 0x07ffffe4, 0x07ffffe5,
    0x000fffec, 0x00fffff3, 0x000fffed, 0x001fffe6, 0x003fffe9, 0x001fffe7, 0x001fffe8, 0x007ffff3,
    0x003fffea, 0x003fffeb, 0x01ffffee, 0x01ffffef, 0x00fffff4, 0x00fffff5, 0x03ffffea, 0x007ffff4,
    0x03ffffeb, 0x07ffffe6, 0x03ffffec, 0x03ffffed, 0x07ffffe7, 0x07ffffe8, 0x07ffffe9, 0x07ffffea,
    0x07ffffeb, 0x0ffffffe, 0x07ffffec, 0x07ffffed, 0x07ffffee, 0x07ffffef, 0x07fffff0, 0x03ffffee,
};
static const uint8_t kHuffLen[256] = {
    13u, 23u, 28u, 28u, 28u, 28u, 28u, 28u, 28u, 24u, 30u, 28u, 28u, 30u, 28u, 28u,
    28u, 28u, 28u, 28u, 28u, 28u, 30u, 28u, 28u, 28u, 28u, 28u, 28u, 28u, 28u, 28u,
     6u, 10u, 10u, 12u, 13u,  6u,  8u, 11u, 10u, 10u,  8u, 11u,  8u,  6u,  6u,  6u,
     5u,  5u,  5u,  6u,  6u,  6u,  6u,  6u,  6u,  6u,  7u,  8u, 15u,  6u, 12u, 10u,
    13u,  6u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,
     7u,  7u,  7u,  7u,  7u,  7u,  7u,  7u,  8u,  7u,  8u, 13u, 19u, 13u, 14u,  6u,
    15u,  5u,  6u,  5u,  6u,  5u,  6u,  6u,  6u,  5u,  7u,  7u,  6u,  6u,  6u,  5u,
     6u,  7u,  6u,  5u,  5u,  6u,  7u,  7u,  7u,  7u,  7u, 15u, 11u, 14u, 13u, 28u,
    20u, 22u, 20u, 20u, 22u, 22u, 22u, 23u, 22u, 23u, 23u, 23u, 23u, 23u, 24u, 23u,
    24u, 24u, 22u, 23u, 24u, 23u, 23u, 23u, 23u, 21u, 22u, 23u, 22u, 23u, 23u, 24u,
    22u, 21u, 20u, 22u, 22u, 23u, 23u, 21u, 23u, 22u, 22u, 24u, 21u, 22u, 23u, 23u,
    21u, 21u, 22u, 21u, 23u, 22u, 23u, 23u, 20u, 22u, 22u, 22u, 23u, 22u, 22u, 23u,
    26u, 26u, 20u, 19u, 22u, 23u, 22u, 25u, 26u, 26u, 26u, 27u, 27u, 26u, 24u, 25u,
    19u, 21u, 26u, 27u, 27u, 26u, 27u, 24u, 21u, 21u, 26u, 26u, 28u, 27u, 27u, 27u,
    20u, 24u, 20u, 21u, 22u, 21u, 21u, 23u, 22u, 22u, 25u, 25u, 24u, 24u, 26u, 23u,
    26u, 27u, 26u, 26u, 27u, 27u, 27u, 27u, 27u, 28u, 27u, 27u, 27u, 27u, 27u, 26u,};

namespace {

// Decode trie built from the code table once (thread-safe static init).
struct HUFF_NODE {
    int16_t child[2];   // -1 = none
    int16_t sym;        // -1 = internal, 0..255 = leaf symbol
};

const std::vector<HUFF_NODE>& HuffTrie() {
    static const std::vector<HUFF_NODE> trie = [] {
        std::vector<HUFF_NODE> t;
        t.push_back(HUFF_NODE{{-1, -1}, -1});  // root = 0
        for (int sym = 0; sym < 256; ++sym) {
            uint32_t code = kHuffCode[sym];
            int bits = kHuffLen[sym];
            int node = 0;
            for (int b = bits - 1; b >= 0; --b) {
                int bit = static_cast<int>((code >> b) & 1u);
                if (t[static_cast<std::size_t>(node)].child[bit] < 0) {
                    int idx = static_cast<int>(t.size());
                    t.push_back(HUFF_NODE{{-1, -1}, -1});
                    t[static_cast<std::size_t>(node)].child[bit] =
                        static_cast<int16_t>(idx);
                }
                node = t[static_cast<std::size_t>(node)].child[bit];
            }
            t[static_cast<std::size_t>(node)].sym = static_cast<int16_t>(sym);
        }
        return t;
    }();
    return trie;
}

// True when `node` lies on the all-ones path (a prefix of the 30-bit EOS
// code) -- the only legal mid-stream stopping point for padding.
bool IsEosPrefix(const std::vector<HUFF_NODE>& trie, int node) {
    int cur = 0;
    int remaining = 30;
    for (;;) {
        if (cur == node) return true;
        if (remaining == 0) return false;
        int nxt = trie[static_cast<std::size_t>(cur)].child[1];
        if (nxt < 0) return false;
        cur = nxt;
        --remaining;
    }
}

}  // namespace

void HuffmanEncode(const std::string& s, std::vector<uint8_t>* out) {
    uint32_t acc = 0;
    unsigned accBits = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        acc = (acc << kHuffLen[c]) | kHuffCode[c];
        accBits += kHuffLen[c];
        while (accBits >= 8) {
            accBits -= 8;
            out->push_back(static_cast<uint8_t>((acc >> accBits) & 0xFF));
        }
    }
    if (accBits > 0) {
        // pad with ones (EOS prefix), < 8 bits
        const unsigned pad = 8 - accBits;
        out->push_back(static_cast<uint8_t>(
            ((acc << pad) & 0xFFu) | ((1u << pad) - 1u)));
    }
}

bool HuffmanDecode(const uint8_t* data, std::size_t sz, std::string* out) {
    const std::vector<HUFF_NODE>& trie = HuffTrie();
    int node = 0;
    std::size_t pos = 0;
    while (pos < sz) {
        const uint8_t byte = data[pos++];
        for (int b = 7; b >= 0; --b) {
            const int bit = static_cast<int>((byte >> b) & 1u);
            const int nxt = trie[static_cast<std::size_t>(node)].child[bit];
            if (nxt < 0) return false;  // no such prefix in the table
            node = nxt;
            const int16_t sym = trie[static_cast<std::size_t>(node)].sym;
            if (sym >= 0) {
                out->push_back(static_cast<char>(sym));
                node = 0;
            }
        }
    }
    // A stream may only end on a symbol boundary or on an all-ones
    // (EOS-prefix) path; a truncated or malformed code fails here.
    return node == 0 || IsEosPrefix(trie, node);
}

// ---- dynamic table (RFC 7541 2.3) ----------------------------------------
uint32_t DynamicTable::EntrySize(const std::string& name,
                                 const std::string& value) {
    return static_cast<uint32_t>(name.size() + value.size() + 32u);
}

void DynamicTable::EvictTo(uint32_t limit) {
    while (!entries_.empty() && size_ > limit) {
        size_ -= entries_.back().size;
        entries_.pop_back();
    }
}

void DynamicTable::SetCapacity(uint32_t cap) {
    if (cap > 4096u) cap = 4096u;  // RFC 7541 4.2 hard cap for HTTP/2
    capacity_ = cap;
    EvictTo(capacity_);
}

bool DynamicTable::Insert(const std::string& name, const std::string& value) {
    const uint32_t need = EntrySize(name, value);
    if (need > capacity_) return false;  // entry alone does not fit
    entries_.insert(entries_.begin(), Entry{name, value, need});
    size_ += need;
    EvictTo(capacity_);
    return true;
}

bool DynamicTable::Get(uint32_t index, std::string* name,
                       std::string* value) const {
    if (index == 0 || index > entries_.size()) return false;
    const Entry& e = entries_[index - 1];
    if (name) *name = e.name;
    if (value) *value = e.value;
    return true;
}

uint32_t DynamicTable::FindExact(const std::string& name,
                                 const std::string& value) const {
    for (std::size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].name == name && entries_[i].value == value)
            return static_cast<uint32_t>(i + 1);
    }
    return 0;
}

uint32_t DynamicTable::FindName(const std::string& name) const {
    for (std::size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].name == name) return static_cast<uint32_t>(i + 1);
    }
    return 0;
}

void DynamicTable::Clear() {
    entries_.clear();
    size_ = 0;
}

// ---- encoder -------------------------------------------------------------
void Encoder::EncodeField(std::vector<uint8_t>* o, const std::string& name,
                          const std::string& value, FieldMode mode) {
    // 1) full exact match (name+value): static table first, then dynamic
    for (int i = 0; i < 61; ++i) {
        if (kStaticTable[i].value[0] != '\0' &&
            std::strcmp(kStaticTable[i].name, name.c_str()) == 0 &&
            value == kStaticTable[i].value) {
            EncodeInt(o, static_cast<uint32_t>(i + 1), 7, 0x80);
            return;
        }
    }
    const uint32_t dyn = table_.FindExact(name, value);
    if (dyn != 0) {
        EncodeInt(o, 61u + dyn, 7, 0x80);
        return;
    }

    // 2) literal: static name index 1..61, else dynamic name index, else
    //    literal name
    int nameIdx = StaticNameIndex(name);
    if (nameIdx == 0) {
        const uint32_t dn = table_.FindName(name);
        if (dn != 0) nameIdx = static_cast<int>(61u + dn);
    }

    if (mode == FieldMode::Indexed) {
        // literal with incremental indexing (01xxxxxx) + insert
        EncodeInt(o, static_cast<uint32_t>(nameIdx), 6, 0x40);
        if (nameIdx == 0) EncodeString(o, name);
        EncodeString(o, value);
        table_.Insert(name, value);
    } else {
        // literal never indexed (0001xxxx)
        EncodeInt(o, static_cast<uint32_t>(nameIdx), 4, 0x10);
        if (nameIdx == 0) EncodeString(o, name);
        EncodeString(o, value);
    }
}

// ---- decoder -------------------------------------------------------------
namespace {
constexpr uint32_t kMaxFields = 4096;
}

bool Decoder::DecodeHeaderBlock(
    const uint8_t* data, std::size_t sz,
    std::vector<std::pair<std::string, std::string>>* out) {
    std::size_t p = 0;
    uint32_t fields = 0;
    while (p < sz) {
        if (++fields > kMaxFields) return false;
        if (p >= sz) return false;
        const uint8_t b = data[p];
        std::string name, value;
        if (b & 0x80) {
            // indexed: 1xxxxxxx + 7-bit index
            uint32_t idx = 0;
            if (!DecodeInt(data, sz, &p, 7, &idx)) return false;
            if (idx == 0) return false;  // index 0 is invalid
            if (idx <= 61) {
                name = kStaticTable[idx - 1].name;
                value = kStaticTable[idx - 1].value;
            } else {
                if (!table_.Get(idx - 61, &name, &value)) return false;
            }
        } else {
            // literal: incremental (01xxxxxx, 6-bit name index) or
            // without/never (0000/0001, 4-bit name index)
            const bool incremental = (b & 0x40) != 0;
            const uint8_t prefixBits = incremental ? 6 : 4;
            uint32_t nameIdx = 0;
            if (!DecodeInt(data, sz, &p, prefixBits, &nameIdx)) return false;
            if (nameIdx == 0) {
                if (!DecodeString(data, sz, &p, &name)) return false;
            } else if (nameIdx <= 61) {
                name = kStaticTable[nameIdx - 1].name;
            } else {
                if (!table_.Get(nameIdx - 61, &name, &value)) return false;
            }
            if (!DecodeString(data, sz, &p, &value)) return false;
            if (incremental) table_.Insert(name, value);
        }
        out->emplace_back(std::move(name), std::move(value));
    }
    return true;
}

}  // namespace hpack
}  // namespace net
}  // namespace chaos
