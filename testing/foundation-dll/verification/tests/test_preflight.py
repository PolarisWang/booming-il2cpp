"""Tests for preflight.py — contract validation logic."""

import json
from pathlib import Path

from verification.orchestration.context import FamilyContext, StageResult
from verification.stages.preflight import run_preflight


class TestPreflight:
    def test_contract_not_found(self, tmp_path):
        ctx = FamilyContext(slug="missing", assembly="Test.Asm", family_dir=tmp_path)
        result = run_preflight(ctx, {})
        assert result.status == "failed"
        assert "Contract not found" in result.summary

    def test_contract_invalid_json(self, tmp_path):
        contract_path = tmp_path / "capability-family-contract.json"
        contract_path.write_text("not json", encoding="utf-8")
        ctx = FamilyContext(slug="bad-json", assembly="Test.Asm", family_dir=tmp_path)
        result = run_preflight(ctx, {})
        assert result.status == "failed"
        assert "Failed to read contract" in result.summary

    def test_empty_method_subject_ids(self, tmp_path):
        contract_path = tmp_path / "capability-family-contract.json"
        contract_path.write_text(json.dumps({"methodSubjectIds": []}), encoding="utf-8")
        ctx = FamilyContext(slug="empty", assembly="Test.Asm", family_dir=tmp_path)
        result = run_preflight(ctx, {})
        assert result.status == "failed"
        assert "No methodSubjectIds" in result.summary

    def test_missing_method_subject_ids_key(self, tmp_path):
        contract_path = tmp_path / "capability-family-contract.json"
        contract_path.write_text(json.dumps({"familyId": "test"}), encoding="utf-8")
        ctx = FamilyContext(slug="no-key", assembly="Test.Asm", family_dir=tmp_path)
        result = run_preflight(ctx, {})
        assert result.status == "failed"
        assert "No methodSubjectIds" in result.summary

    def test_valid_contract(self, tmp_path):
        mids = ["Subject1::Method1", "Subject2::Method2", "Subject3::Method3"]
        contract_path = tmp_path / "capability-family-contract.json"
        contract_path.write_text(json.dumps({
            "methodSubjectIds": mids,
            "familyId": "test-family",
            "displayName": "Test Family",
        }), encoding="utf-8")
        ctx = FamilyContext(slug="valid", assembly="Test.Asm", family_dir=tmp_path)
        result = run_preflight(ctx, {})
        assert result.status == "passed"
        assert result.details["methodCount"] == 3
        assert result.details["familyId"] == "test-family"
        assert result.details["displayName"] == "Test Family"
