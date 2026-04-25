from tests.unit.compatibility.aot_core_ir_ldtoken_target_reference_test_support import *


class TestAotCoreIrLdtokenTargetReferenceRealTypes(AotCoreIrLdtokenTargetReferenceTestSupport):
    def test_real_csharp_type_ldtoken_gets_aot_core_ir_target_reference(self) -> None:
        self._ensure_type_bundle_generated()

        aot_core_ir = load_json(self.type_output_root / "aot-core-ir.json")
        method = next(method for method in aot_core_ir["methods"] if method["subjectId"] == TYPE_HELPER_SUBJECT_ID)
        ldtoken = next(
            instruction
            for instruction in method["instructions"]
            if instruction["op"] == "ldtoken"
            and instruction["reference"]["subjectId"] == f"{TYPE_FIXTURE_ASSEMBLY_NAME}/TokenCarrier"
        )

        self.assertEqual("System.RuntimeTypeHandle", ldtoken["resultType"])
        self.assertEqual(
            {
                "assemblyName": TYPE_FIXTURE_ASSEMBLY_NAME,
                "subjectKind": "type",
                "subjectId": f"{TYPE_FIXTURE_ASSEMBLY_NAME}/TokenCarrier",
            },
            ldtoken["reference"],
        )
        self.assertEqual(f"{TYPE_FIXTURE_ASSEMBLY_NAME}/TokenCarrier", ldtoken["targetReference"]["subjectId"])
        self.assertEqual(1, ldtoken["targetReference"]["kind"])
