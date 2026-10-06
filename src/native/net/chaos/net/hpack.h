// Layer I -- HPACK (RFC 7541) header compression for HTTP/2 (NT-21).
//
// Scope: full RFC 7541 --
//   * static table (Appendix A, 61 entries)
//   * dynamic table (capacity set via SETTINGS_HEADER_TABLE_SIZE; LRU
//     eviction when over capacity; entry size = name+value+32)
//   * Huffman coding (Appendix B) used whenever it shortens the string
//   * indexed / literal-with-incremental-indexing / literal-without-
//     indexing / literal-never-indexed field forms
//
// Each direction of each HTTP/2 connection owns one Encoder and one
// Decoder.  All functions are noexcept-lean (vectors/strings only).
#ifndef CHAOS_NET_HPACK_H
#define CHAOS_NET_HPACK_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace chaos {
namespace net {
namespace hpack {

// Huffman (Appendix B).  HuffmanEncode never fails; HuffmanDecode rejects
// EOS-only paths, truncated input and padding longer than 7 bits.
void HuffmanEncode(const std::string& s, std::vector<uint8_t>* out);
bool HuffmanDecode(const uint8_t* data, std::size_t sz, std::string* out);

enum class FieldMode {
    Indexed,       // literal-with-incremental-indexing + table insert
    NeverIndexed,  // literal-never-indexed (no table insert)
};

// ── dynamic table (RFC 7541 2.3) ────────────────────────────────────────
class DynamicTable {
public:
    explicit DynamicTable(uint32_t capacity = 4096) : capacity_(capacity) {}
    void SetCapacity(uint32_t cap);
    uint32_t Capacity() const { return capacity_; }
    uint32_t Count() const { return static_cast<uint32_t>(entries_.size()); }
    uint32_t Size() const { return size_; }
    // 1-based index into the dynamic region (overall index 62+); false OOB.
    bool Get(uint32_t index, std::string* name, std::string* value) const;
    // Insert at front (dynamic index 1); evicts old entries to stay within
    // capacity.  Returns false when the entry itself exceeds capacity.
    bool Insert(const std::string& name, const std::string& value);
    // index of the first exact / name-only match (0 = none)
    uint32_t FindExact(const std::string& name, const std::string& value) const;
    uint32_t FindName(const std::string& name) const;
    void Clear();
    static uint32_t EntrySize(const std::string& name,
                              const std::string& value);

private:
    struct Entry {
        std::string name;
        std::string value;
        uint32_t size;
    };
    std::vector<Entry> entries_;  // front = most recent (index 1)
    uint32_t capacity_;
    uint32_t size_ = 0;
    void EvictTo(uint32_t limit);
};

// ── encoder side (owns its view of the dynamic table) ───────────────────
class Encoder {
public:
    explicit Encoder(uint32_t tableCapacity = 4096)
        : table_(tableCapacity) {}
    DynamicTable& Table() { return table_; }
    // One field.  Indexed mode emits incremental indexing (and inserts
    // into the table); NeverIndexed emits the 0001 form (e.g. for
    // authorization), without inserting.
    void EncodeField(std::vector<uint8_t>* out, const std::string& name,
                     const std::string& value,
                     FieldMode mode = FieldMode::Indexed);

private:
    DynamicTable table_;
};

// ── decoder side ────────────────────────────────────────────────────────
class Decoder {
public:
    explicit Decoder(uint32_t tableCapacity = 4096) : table_(tableCapacity) {}
    DynamicTable& Table() { return table_; }
    // Decodes one header block (field sequence).  On failure the caller
    // should abort the connection: a half-updated table is unrecoverable.
    bool DecodeHeaderBlock(const uint8_t* data, std::size_t sz,
                           std::vector<std::pair<std::string, std::string>>*
                               out);

private:
    DynamicTable table_;
};

}  // namespace hpack
}  // namespace net
}  // namespace chaos

#endif  // CHAOS_NET_HPACK_H
