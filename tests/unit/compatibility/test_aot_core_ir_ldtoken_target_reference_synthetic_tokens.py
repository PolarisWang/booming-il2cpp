from tests.unit.compatibility.aot_core_ir_ldtoken_target_reference_test_support import *


class TestAotCoreIrLdtokenTargetReferenceSyntheticTokens(AotCoreIrLdtokenTargetReferenceTestSupport):
    def test_synthetic_field_and_method_ldtoken_get_target_reference(self) -> None:
        self._ensure_synthetic_output_generated()

        aot_core_ir = load_json(self.synthetic_output_path)
        method = next(method for method in aot_core_ir["methods"] if method["subjectId"] == SYNTHETIC_MAIN_SUBJECT_ID)
        instructions = [instruction for instruction in method["instructions"] if instruction["op"] == "ldtoken"]

        type_token = next(instruction for instruction in instructions if instruction["reference"]["subjectKind"] == "type")
        field_token = next(instruction for instruction in instructions if instruction["reference"]["subjectKind"] == "field")
        method_token = next(instruction for instruction in instructions if instruction["reference"]["subjectKind"] == "method")

        self.assertEqual(f"{SYNTHETIC_ASSEMBLY_NAME}/TokenHolder", type_token["targetReference"]["subjectId"])
        self.assertEqual(1, type_token["targetReference"]["kind"])

        self.assertEqual(f"{SYNTHETIC_ASSEMBLY_NAME}/TokenHolder::Value", field_token["targetReference"]["subjectId"])
        self.assertEqual(2, field_token["targetReference"]["kind"])
        self.assertEqual(f"{SYNTHETIC_ASSEMBLY_NAME}/TokenHolder", field_token["targetReference"]["declaringTypeSubjectId"])

        self.assertEqual(
            f"{SYNTHETIC_ASSEMBLY_NAME}/TokenHolder::Helper:System.Void()",
            method_token["targetReference"]["subjectId"],
        )
        self.assertEqual(3, method_token["targetReference"]["kind"])
