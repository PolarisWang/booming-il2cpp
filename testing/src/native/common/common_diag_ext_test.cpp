/// common_diag_ext_test.cpp — CommonLibrary diagnostic edge-case tests
///
/// Supplementary tests for log/trace/profile edge cases not covered by
/// the primary test files (P3-F: CommonLibrary 诊断补充测试).

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/log.h>
#include <chaos/trace.h>
#include <chaos/profile.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════
// Log edge cases
// ═══════════════════════════════════════════════════════════════════════

TEST(CommonDiagLog, EmptyCategory) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_ERROR("", "msg");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[]"));
    EXPECT_NE(std::string::npos, output.find("msg"));
}

TEST(CommonDiagLog, VeryLongMessage) {
    std::string long_msg(4096, 'X');
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_ERROR("Long", long_msg.c_str());
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("[Long]"));
    EXPECT_NE(std::string::npos, output.find("XXX"));
}

TEST(CommonDiagLog, SpecialCharacters) {
    testing::internal::CaptureStdout();
    CHAOS_IL2CPP_LOG_ERROR("Spec", "line1\nline2\t\"quoted\"");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(std::string::npos, output.find("\"quoted\""));
}

TEST(CommonDiagLog, ConcurrentManyWrites) {
    // 4 threads, 100 writes each — no crash, no deadlock
    auto worker = [](int id) {
        for (int i = 0; i < 100; i++) {
            CHAOS_IL2CPP_LOG_INFO_M("Stress", "thread={0} iter={1}", id, i);
        }
    };
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++)
        threads.emplace_back(worker, t);
    for (auto& t : threads) t.join();
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════
// Trace edge cases
// ═══════════════════════════════════════════════════════════════════════

static bool trace_flush_contains(const char* path, const char* expected) {
    CHAOS_IL2CPP_LOG_TRACE_FLUSH(path);
    auto* f = std::fopen(path, "r");
    if (!f) return false;
    char line[1024];
    bool found = false;
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strstr(line, expected)) { found = true; break; }
    }
    std::fclose(f);
    std::remove(path);
    return found;
}

TEST(CommonDiagTrace, EmptyStage) {
    CHAOS_IL2CPP_LOG_TRACE("", "op", "");
    EXPECT_TRUE(trace_flush_contains("diag_ext_empty_stage.tmp", "\"s\":\"\""));
}

TEST(CommonDiagTrace, EmptyOperation) {
    CHAOS_IL2CPP_LOG_TRACE("stage", "", "");
    EXPECT_TRUE(trace_flush_contains("diag_ext_empty_op.tmp", "\"o\":\"\""));
}

TEST(CommonDiagTrace, VeryLongExtra) {
    std::string long_extra(512, 'Y');
    CHAOS_IL2CPP_LOG_TRACE("big", "data", "{0}", long_extra);
    // Should not crash; output may be truncated to kTraceLineMax
    EXPECT_TRUE(trace_flush_contains("diag_ext_long_extra.tmp", "YYYY"));
}

TEST(CommonDiagTrace, MultipleSubjectsSameFlush) {
    CHAOS_IL2CPP_LOG_TRACE("subj_a", "op1", "");
    CHAOS_IL2CPP_LOG_TRACE("subj_b", "op2", "");
    const char* path = "diag_ext_multi_subj.tmp";
    CHAOS_IL2CPP_LOG_TRACE_FLUSH(path);
    auto* f = std::fopen(path, "r");
    ASSERT_NE(nullptr, f);
    char line[512];
    bool found_a = false, found_b = false;
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strstr(line, "\"subj_a\"")) found_a = true;
        if (std::strstr(line, "\"subj_b\"")) found_b = true;
    }
    std::fclose(f);
    std::remove(path);
    EXPECT_TRUE(found_a);
    EXPECT_TRUE(found_b);
}

// ═══════════════════════════════════════════════════════════════════════
// Profile edge cases
// ═══════════════════════════════════════════════════════════════════════

TEST(CommonDiagProfile, EmptyScopeName) {
    CHAOS_IL2CPP_PROFILE_RESET();
    { CHAOS_IL2CPP_PROFILE_SCOPE(""); }
    SUCCEED();
}

TEST(CommonDiagProfile, DeeplyNestedScopes) {
    CHAOS_IL2CPP_PROFILE_RESET();
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("level0");
        {
            CHAOS_IL2CPP_PROFILE_SCOPE("level1");
            {
                CHAOS_IL2CPP_PROFILE_SCOPE("level2");
                {
                    CHAOS_IL2CPP_PROFILE_SCOPE("level3");
                }
            }
        }
    }
    SUCCEED();
}

TEST(CommonDiagProfile, VeryLongScopeName) {
    CHAOS_IL2CPP_PROFILE_RESET();
    std::string long_name(256, 'N');
    { CHAOS_IL2CPP_PROFILE_SCOPE(long_name.c_str()); }
    SUCCEED();
}

TEST(CommonDiagProfile, ConcurrentProfileScopes) {
    CHAOS_IL2CPP_PROFILE_RESET();
    auto worker = []() {
        for (int i = 0; i < 50; i++) {
            CHAOS_IL2CPP_PROFILE_SCOPE("concurrent_scope");
        }
    };
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();
    SUCCEED();
}
