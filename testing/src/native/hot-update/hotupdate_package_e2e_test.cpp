// hotupdate_package_e2e_test.cpp — E2E package loading test
//
// Creates a real package directory structure with manifest, assembly, and
// metadata files, then exercises LoadHotUpdatePackage / UnloadHotUpdatePackage.
//
// GC-free: tests only the package loading layer, no GC dependencies.

#include <hot_update.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

// AOT symbol stubs (required by linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

namespace hu = chaos::il2cpp::hot_update;

// ── Cross-platform temp directory helper ────────────────────────────────

class TempPackageDir {
public:
    TempPackageDir() {
#if defined(_WIN32) || defined(_WIN64)
        // Use GetTempPathA for the system temp directory.
        char tmp_base[MAX_PATH + 1];
        DWORD ret = GetTempPathA(MAX_PATH + 1, tmp_base);
        if (ret == 0 || ret > MAX_PATH) throw std::runtime_error("GetTempPathA failed");

        // GetTempFileNameA creates a zero-length temp file with a unique name.
        char tmp_name[MAX_PATH + 1];
        ret = GetTempFileNameA(tmp_base, "hup", 0, tmp_name);
        if (ret == 0) throw std::runtime_error("GetTempFileNameA failed");

        // Delete the zero-length file, then use the name as our directory name.
        std::string dir_name(tmp_name);
        DeleteFileA(dir_name.c_str());

        // Remove .tmp extension and use as directory.
        dir_name.erase(dir_name.size() - 4);
        path_ = dir_name + "\\";
        if (_mkdir(path_.c_str()) != 0) throw std::runtime_error("Failed to create dir: " + path_);
#else
        const char* tmp_base = std::getenv("TMPDIR");
        if (!tmp_base) tmp_base = "/tmp";
        char unique[L_tmpnam];
        errno_t err = tmpnam_s(unique, L_tmpnam);
        if (err != 0) throw std::runtime_error("tmpnam_s failed");
        path_ = std::string(tmp_base) + "/" + unique + "/";
        if (mkdir(path_.c_str(), 0700) != 0) throw std::runtime_error("Failed to create dir: " + path_);
#endif
    }

    ~TempPackageDir() {
        Cleanup();
    }

    const std::string& Path() const { return path_; }

    void WriteFile(const std::string& name, const std::string& content) {
        std::string full = path_ + name;
        std::ofstream ofs(full, std::ios::binary);
        ASSERT_TRUE(ofs.good()) << "Failed to create: " << full;
        ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
        ofs.close();
    }

    void WriteBinaryFile(const std::string& name, const std::vector<uint8_t>& data) {
        std::string full = path_ + name;
        std::ofstream ofs(full, std::ios::binary);
        ASSERT_TRUE(ofs.good()) << "Failed to create: " << full;
        ofs.write(reinterpret_cast<const char*>(data.data()),
                  static_cast<std::streamsize>(data.size()));
        ofs.close();
    }

private:
    void Cleanup() {
        if (path_.empty()) return;
        // Attempt to remove all files then the directory (best-effort).
        std::remove((path_ + "package.manifest.json").c_str());
        std::remove((path_ + "manifest.json").c_str());
#if defined(_WIN32) || defined(_WIN64)
        _rmdir(path_.c_str());
#else
        rmdir(path_.c_str());
#endif
    }

    std::string path_;
};

// ── Tests ───────────────────────────────────────────────────────────────

TEST(HotUpdatePackageE2E, LoadAndUnload)
{
    TempPackageDir dir;

    // Create manifest.json.
    const char* manifest = R"({
        "PackageId": "test-package-v1",
        "TargetAotVersion": "1.0.0",
        "Name": "TestAssembly.dll",
        "SupplementalMetadata": "TestAssembly.metadata"
    })";
    dir.WriteFile("manifest.json", manifest);

    // Create dummy assembly file (256 bytes of recognizable pattern).
    std::vector<uint8_t> asm_data(256);
    for (int i = 0; i < 256; i++) {
        asm_data[i] = static_cast<uint8_t>(i);
    }
    dir.WriteBinaryFile("TestAssembly.dll", asm_data);

    // Create dummy metadata file (128 bytes).
    std::vector<uint8_t> meta_data(128);
    for (int i = 0; i < 128; i++) {
        meta_data[i] = static_cast<uint8_t>(0xFF - i);
    }
    dir.WriteBinaryFile("TestAssembly.metadata", meta_data);

    // ── Load ──────────────────────────────────────────────────────────
    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    ASSERT_TRUE(loaded) << "LoadHotUpdatePackage returned false";

    // Verify handle fields.
    EXPECT_TRUE(handle.loaded);

    EXPECT_STREQ(handle.package_id, "test-package-v1");
    EXPECT_STREQ(handle.target_aot_version, "1.0.0");
    EXPECT_STREQ(handle.assembly_name, "TestAssembly.dll");

    EXPECT_GT(handle.module_id, 0u);
    EXPECT_GT(handle.domain_id, 0u);

    ASSERT_NE(handle.assembly_image.data, nullptr);
    EXPECT_EQ(handle.assembly_image.size, 256u);

    ASSERT_NE(handle.metadata_image.data, nullptr);
    EXPECT_EQ(handle.metadata_image.size, 128u);

    // Verify assembly data content.
    for (uint32_t i = 0; i < 256 && i < handle.assembly_image.size; i++) {
        EXPECT_EQ(handle.assembly_image.data[i], static_cast<unsigned char>(i));
    }

    // Verify metadata data content.
    for (uint32_t i = 0; i < 128 && i < handle.metadata_image.size; i++) {
        EXPECT_EQ(handle.metadata_image.data[i], static_cast<unsigned char>(0xFF - i));
    }

    // ── Unload ────────────────────────────────────────────────────────
    hu::UnloadHotUpdatePackage(&handle);

    EXPECT_FALSE(handle.loaded);
    EXPECT_EQ(handle.module_id, 0u);
    EXPECT_EQ(handle.domain_id, 0u);
    EXPECT_EQ(handle.assembly_image.data, nullptr);
    EXPECT_EQ(handle.assembly_image.size, 0u);
    EXPECT_EQ(handle.metadata_image.data, nullptr);
    EXPECT_EQ(handle.metadata_image.size, 0u);
    EXPECT_EQ(handle.package_id, nullptr);
    EXPECT_EQ(handle.target_aot_version, nullptr);
    EXPECT_EQ(handle.assembly_name, nullptr);
}

TEST(HotUpdatePackageE2E, LoadWithPackageManifest)
{
    // Test that package.manifest.json is preferred over manifest.json.
    TempPackageDir dir;

    // Write both — package.manifest.json should be read.
    const char* package_manifest = R"({
        "PackageId": "preferred-version",
        "TargetAotVersion": "2.0.0",
        "Name": "Preferred.dll",
        "SupplementalMetadata": "Preferred.meta"
    })";
    dir.WriteFile("package.manifest.json", package_manifest);

    const char* fallback_manifest = R"({
        "PackageId": "fallback-version",
        "TargetAotVersion": "0.0.0",
        "Name": "Fallback.dll",
        "SupplementalMetadata": "Fallback.meta"
    })";
    dir.WriteFile("manifest.json", fallback_manifest);

    // Create dummy files for the preferred name.
    dir.WriteBinaryFile("Preferred.dll", {1, 2, 3, 4});
    dir.WriteBinaryFile("Preferred.meta", {5, 6, 7, 8});

    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    ASSERT_TRUE(loaded);

    EXPECT_STREQ(handle.package_id, "preferred-version");
    EXPECT_STREQ(handle.assembly_name, "Preferred.dll");

    hu::UnloadHotUpdatePackage(&handle);
}

TEST(HotUpdatePackageE2E, MissingManifest)
{
    TempPackageDir dir;
    // No manifest file at all.

    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    EXPECT_FALSE(loaded);
}

TEST(HotUpdatePackageE2E, MissingAssemblyFile)
{
    TempPackageDir dir;

    const char* manifest = R"({
        "PackageId": "missing-asm",
        "TargetAotVersion": "1.0.0",
        "Name": "Missing.dll",
        "SupplementalMetadata": "Meta.bin"
    })";
    dir.WriteFile("manifest.json", manifest);
    dir.WriteBinaryFile("Meta.bin", {0xAB});

    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    EXPECT_FALSE(loaded);
}

TEST(HotUpdatePackageE2E, MissingMetadataFile)
{
    TempPackageDir dir;

    const char* manifest = R"({
        "PackageId": "missing-meta",
        "TargetAotVersion": "1.0.0",
        "Name": "Asm.dll",
        "SupplementalMetadata": "Missing.meta"
    })";
    dir.WriteFile("manifest.json", manifest);
    dir.WriteBinaryFile("Asm.dll", {0x01});

    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    EXPECT_FALSE(loaded);
}

TEST(HotUpdatePackageE2E, MalformedJson)
{
    TempPackageDir dir;

    dir.WriteFile("manifest.json", "this is not valid json");
    dir.WriteBinaryFile("Asm.dll", {0x01});
    dir.WriteBinaryFile("Meta.bin", {0x02});

    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    EXPECT_FALSE(loaded);
}

TEST(HotUpdatePackageE2E, LoadUnloadLoadReload)
{
    // Load → Unload → Load again should succeed.
    TempPackageDir dir;

    const char* manifest = R"({
        "PackageId": "reload-test",
        "TargetAotVersion": "1.0.0",
        "Name": "Reload.dll",
        "SupplementalMetadata": "Reload.meta"
    })";
    dir.WriteFile("manifest.json", manifest);
    dir.WriteBinaryFile("Reload.dll", {0x01, 0x02, 0x03});
    dir.WriteBinaryFile("Reload.meta", {0x04, 0x05});

    // First load.
    {
        hu::HotUpdatePackageHandle handle{};
        ASSERT_TRUE(hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle));
        ASSERT_TRUE(handle.loaded);
        uint32_t first_mod_id = handle.module_id;
        uint32_t first_dom_id = handle.domain_id;

        hu::UnloadHotUpdatePackage(&handle);
        EXPECT_FALSE(handle.loaded);

        // Reload — should get new module_id and domain_id.
        ASSERT_TRUE(hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle));
        EXPECT_TRUE(handle.loaded);
        EXPECT_NE(handle.module_id, 0u);
        EXPECT_NE(handle.domain_id, 0u);
        // Module IDs are monotonically increasing.
        EXPECT_GT(handle.module_id, first_mod_id);

        hu::UnloadHotUpdatePackage(&handle);
    }
}

TEST(HotUpdatePackageE2E, UnloadWithoutLoad)
{
    // Calling UnloadHotUpdatePackage on a zero-initialized handle is a no-op.
    hu::HotUpdatePackageHandle handle{};
    EXPECT_NO_THROW(hu::UnloadHotUpdatePackage(&handle));
}

TEST(HotUpdatePackageE2E, EmptyPackageDir)
{
    TempPackageDir dir;
    // Directory exists but is empty.

    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage(dir.Path().c_str(), &handle);
    EXPECT_FALSE(loaded);
}

TEST(HotUpdatePackageE2E, InvalidPath)
{
    hu::HotUpdatePackageHandle handle{};
    bool loaded = hu::LoadHotUpdatePackage("C:\\nonexistent\\path\\", &handle);
    EXPECT_FALSE(loaded);
}

TEST(HotUpdatePackageE2E, NullHandle)
{
    // LoadHotUpdatePackage should handle null handle gracefully.
    EXPECT_NO_THROW({
        bool result = hu::LoadHotUpdatePackage(nullptr, nullptr);
        EXPECT_FALSE(result);
    });
}
