from __future__ import annotations

import unittest


def resolve_shared_identity(
    aot_registry: dict[str, dict[str, object]],
    hot_update_registry: dict[str, dict[str, object]],
    subject_id: str,
) -> tuple[dict[str, object], dict[str, object]]:
    if subject_id not in aot_registry:
        raise KeyError(f"missing AOT subject id: {subject_id}")
    if subject_id not in hot_update_registry:
        raise KeyError(f"missing hot-update subject id: {subject_id}")
    return aot_registry[subject_id], hot_update_registry[subject_id]


class DualModeIdentityTests(unittest.TestCase):
    def test_subject_id_is_the_cross_version_join_key(self) -> None:
        subject_id = "MainApp/Gameplay.ScoreService::ApplyPatch(System.Int32)"
        aot_registry = {
            subject_id: {
                "token": 0x06000001,
                "runtimeHandle": "aot-method-1",
            }
        }
        hot_update_registry = {
            subject_id: {
                "token": 0x0600002A,
                "runtimeHandle": "hot-method-42",
            }
        }

        aot_binding, hot_binding = resolve_shared_identity(aot_registry, hot_update_registry, subject_id)

        self.assertNotEqual(aot_binding["token"], hot_binding["token"])
        self.assertEqual("aot-method-1", aot_binding["runtimeHandle"])
        self.assertEqual("hot-method-42", hot_binding["runtimeHandle"])

    def test_unknown_subject_id_is_rejected_even_if_tokens_exist(self) -> None:
        aot_registry = {
            "MainApp/Gameplay.ScoreService::ApplyPatch(System.Int32)": {
                "token": 0x06000001,
                "runtimeHandle": "aot-method-1",
            }
        }
        hot_update_registry = {
            "HotPatch/Gameplay.ScoreService::ApplyPatch(System.Int32)": {
                "token": 0x06000001,
                "runtimeHandle": "hot-method-1",
            }
        }

        with self.assertRaises(KeyError):
            resolve_shared_identity(
                aot_registry,
                hot_update_registry,
                "MainApp/Gameplay.ScoreService::ApplyPatch(System.Int32)",
            )


if __name__ == "__main__":
    unittest.main()
