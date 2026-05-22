// rwlock_upgrade_test.cpp — ReaderWriterLockSlim upgradeable read / upgrade /
// downgrade tests
//
// Tests the upgradeable read lock semantics:
//   - Multiple readers can coexist with upgradeable reader
//   - Only one upgradeable reader at a time
//   - Writer blocks when upgradeable reader holds
//   - Upgrade upgradeable→write (reads must drain)
//   - Downgrade write→upgradeable
//   - ExitUpgradeableRead release path

#include <gtest/gtest.h>

#include <synchronization.h>
#include <thread_state.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Basic upgradeable read acquire/release ──────────────────────────────

TEST(RWLockUpgrade, BasicAcquireRelease)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    // Acquire upgradeable read.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // Exit upgradeable read.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Readers coexist with upgradeable reader ─────────────────────────────

TEST(RWLockUpgrade, ReadersCoexistWithUpgradeable)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    // Acquire upgradeable read.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // Regular readers should still be able to enter.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);

    // Exit readers.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));

    // Exit upgradeable read.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Only one upgradeable reader at a time ───────────────────────────────

TEST(RWLockUpgrade, ExclusiveUpgradeable)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // A second upgradeable read attempt should timeout (poll).
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 0), 0);

    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Writer blocks when upgradeable reader is active ─────────────────────

TEST(RWLockUpgrade, WriterBlocksOnUpgradeable)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // Writer should not be able to enter (upgradeable reader holds).
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterWrite(rw, 0), 0);

    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    // After upgradeable releases, writer can enter.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterWrite(rw, 100), 1);
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitWrite(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Upgradeable reader blocks writer (concurrent test) ──────────────────

TEST(RWLockUpgrade, UpgradeableBlocksWriterConcurrent)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    std::atomic<bool> writer_entered{false};
    std::atomic<bool> writer_done{false};

    // Acquire upgradeable read on this thread.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    std::thread writer([&] {
        // Writer should block until upgradeable reader releases.
        int r = threading::ReaderWriterLockSlimEnterWrite(rw, 1000);
        if (r == 1) {
            writer_entered.store(true, std::memory_order_release);
            threading::ReaderWriterLockSlimExitWrite(rw);
        }
        writer_done.store(true, std::memory_order_release);
    });

    // Give writer time to start and block.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Writer should be blocked (not entered yet).
    EXPECT_FALSE(writer_entered.load(std::memory_order_acquire));

    // Release upgradeable read — writer should now proceed.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    writer.join();
    EXPECT_TRUE(writer_entered.load(std::memory_order_acquire));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Upgrade upgradeable→write ───────────────────────────────────────────

TEST(RWLockUpgrade, UpgradeToWrite)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // Upgrade to write — should succeed (no readers active).
    EXPECT_EQ(threading::ReaderWriterLockSlimUpgradeToWrite(rw, 100), 1);

    // Should now hold write — readers should not be able to enter.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 0), 0);

    // Downgrade back to upgradeable.
    EXPECT_TRUE(threading::ReaderWriterLockSlimDowngradeFromWrite(rw));

    // After downgrade, readers can enter again.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));

    // Exit upgradeable read.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Upgrade with concurrent readers must drain first ─────────────────────

TEST(RWLockUpgrade, UpgradeWaitsForReadersToDrain)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    std::atomic<bool> reader_done{false};

    // Reader thread acquires read lock and holds it briefly.
    std::thread reader([&] {
        EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));
        reader_done.store(true, std::memory_order_release);
    });

    // Give reader time to acquire.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Attempt upgrade — should block until reader drains.
    EXPECT_EQ(threading::ReaderWriterLockSlimUpgradeToWrite(rw, 1000), 1);
    EXPECT_TRUE(reader_done.load(std::memory_order_acquire));

    // Now hold write — release via downgrade.
    EXPECT_TRUE(threading::ReaderWriterLockSlimDowngradeFromWrite(rw));
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    reader.join();
    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── UpgradeToWrite fails if not upgradeable reader ──────────────────────

TEST(RWLockUpgrade, UpgradeFailsIfNotUpgradeableReader)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    // Not holding upgradeable read — UpgradeToWrite should fail.
    EXPECT_EQ(threading::ReaderWriterLockSlimUpgradeToWrite(rw, 100), -1);

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── DowngradeFromWrite fails if not holding write ───────────────────────

TEST(RWLockUpgrade, DowngradeFailsIfNotHoldingWrite)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // Not holding write (only upgradeable) — DowngradeFromWrite should fail.
    EXPECT_FALSE(threading::ReaderWriterLockSlimDowngradeFromWrite(rw));

    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));
    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Upgrade timeout when readers don't drain ────────────────────────────

TEST(RWLockUpgrade, UpgradeTimeout)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);

    // Hold a reader on another thread for a long time.
    std::thread reader([&] {
        EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Upgrade with very short timeout — should timeout.
    EXPECT_EQ(threading::ReaderWriterLockSlimUpgradeToWrite(rw, 50), 0);

    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    reader.join();
    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Upgradeable→write + ExitWrite (bypassing Downgrade) ─────────────────

TEST(RWLockUpgrade, UpgradeThenExitWrite)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);
    EXPECT_EQ(threading::ReaderWriterLockSlimUpgradeToWrite(rw, 100), 1);

    // ExitWrite while holding upgradeable read status.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitWrite(rw));

    // upgradeable_reader_tid is still set — we still logically hold
    // upgradeable read.  Readers can enter but other upgradeable readers
    // and writers cannot.

    // Regular reader can still enter.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));

    // Another upgradeable reader cannot enter.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 0), 0);

    // Release upgradeable read.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    // Now other upgradeable readers can enter.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 100), 1);
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

// ── Multi-threaded upgradeable contention ───────────────────────────────

TEST(RWLockUpgrade, MultiThreadContention)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    constexpr int kIterations = 10;
    std::atomic<int> upgradeable_count{0};
    std::atomic<int> writer_count{0};
    std::atomic<int> errors{0};

    auto worker = [&](bool use_upgradeable) {
        for (int i = 0; i < kIterations; ++i) {
            if (use_upgradeable) {
                int r = threading::ReaderWriterLockSlimEnterUpgradeableRead(rw, 500);
                if (r != 1) { errors.fetch_add(1, std::memory_order_relaxed); continue; }
                upgradeable_count.fetch_add(1, std::memory_order_relaxed);

                // Attempt upgrade (may or may not succeed depending on readers).
                int up = threading::ReaderWriterLockSlimUpgradeToWrite(rw, 0);
                if (up == 1) {
                    writer_count.fetch_add(1, std::memory_order_relaxed);
                    EXPECT_TRUE(threading::ReaderWriterLockSlimDowngradeFromWrite(rw));
                }

                EXPECT_TRUE(threading::ReaderWriterLockSlimExitUpgradeableRead(rw));
            } else {
                int r = threading::ReaderWriterLockSlimEnterRead(rw, 500);
                if (r != 1) { errors.fetch_add(1, std::memory_order_relaxed); continue; }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));
            }
        }
    };

    std::thread t1(worker, true);   // upgradeable thread
    std::thread t2(worker, true);   // upgradeable thread
    std::thread t3(worker, false);  // reader thread
    std::thread t4(worker, false);  // reader thread

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    EXPECT_EQ(errors.load(), 0);
    EXPECT_GT(upgradeable_count.load(), 0);

    threading::ReaderWriterLockSlimDestroy(rw);
}
