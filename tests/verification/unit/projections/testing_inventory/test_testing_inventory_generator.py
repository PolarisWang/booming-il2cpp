from __future__ import annotations

import csv
import io
import json

from tests.unit.performance.testing_inventory_test_support import *


class TestTestingInventoryGenerator(TestingInventoryTestSupport):
    def test_build_inventory_outputs_generates_three_authority_tables_and_detailed_hints(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_tables")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        self.assertIn("capability", payload)
        self.assertIn("unitTest", payload)
        self.assertIn("benchmark", payload)

        capability_inventory = payload["capability"]
        unit_inventory = payload["unitTest"]
        benchmark_inventory = payload["benchmark"]

        self.assertEqual(2, len(capability_inventory["rows"]))
        self.assertEqual(6, len(unit_inventory["rows"]))
        self.assertEqual(1, len(benchmark_inventory["rows"]))

        for header in capability_inventory["headers"]:
            assert_has_detailed_hint(self, header)
        for header in unit_inventory["headers"]:
            assert_has_detailed_hint(self, header)
        for header in benchmark_inventory["headers"]:
            assert_has_detailed_hint(self, header)

        capability_status = {
            row["capabilityItem"]: row["contractStatus"]
            for row in capability_inventory["rows"]
        }
        self.assertEqual("ok", capability_status[31])
        self.assertEqual("missingBenchmark", capability_status[52])

    def test_build_inventory_outputs_keeps_stage_requirement_and_coverage_in_authority_json(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_stages")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        rows = payload["unitTest"]["rows"]
        by_stage = {row["stage"]: row for row in rows}

        self.assertEqual("covered", by_stage["collector"]["stageStatus"])
        self.assertEqual("required", by_stage["collector"]["stageRequirement"])
        self.assertEqual("covered", by_stage["collector"]["stageCoverage"])

        self.assertEqual("covered", by_stage["registry"]["stageStatus"])
        self.assertEqual("covered", by_stage["workspace"]["stageStatus"])

        self.assertEqual("required", by_stage["managed-proof"]["stageStatus"])
        self.assertEqual("required", by_stage["managed-proof"]["stageRequirement"])
        self.assertEqual("pending-proof", by_stage["managed-proof"]["stageCoverage"])

        self.assertEqual("required", by_stage["native-proof"]["stageStatus"])
        self.assertEqual("required", by_stage["native-proof"]["stageRequirement"])
        self.assertEqual("pending-proof", by_stage["native-proof"]["stageCoverage"])

        self.assertEqual("not-applicable", by_stage["hotupdate-proof"]["stageStatus"])
        self.assertEqual("not-applicable", by_stage["hotupdate-proof"]["stageRequirement"])
        self.assertEqual("n/a", by_stage["hotupdate-proof"]["stageCoverage"])

    def test_build_inventory_outputs_flattens_csv_and_html_from_authority_json(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_exports")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        csv_bundle = payload["csv"]
        html_payload = payload["html"]

        capability_csv = list(csv.DictReader(io.StringIO(csv_bundle["capability"])))
        unit_csv = list(csv.DictReader(io.StringIO(csv_bundle["unitTest"])))
        benchmark_csv = list(csv.DictReader(io.StringIO(csv_bundle["benchmark"])))

        self.assertEqual(2, len(capability_csv))
        self.assertEqual(6, len(unit_csv))
        self.assertEqual(1, len(benchmark_csv))
        self.assertIn("stageStatus", unit_csv[0])
        self.assertNotIn("stageRequirement", unit_csv[0])
        self.assertNotIn("stageCoverage", unit_csv[0])
        self.assertEqual("missing", benchmark_csv[0]["nativeStatus"])

        self.assertEqual(3, len(html_payload["tables"]))
        self.assertEqual("Capability Inventory", html_payload["tables"][0]["title"])
        self.assertEqual("UnitTest Inventory", html_payload["tables"][1]["title"])
        self.assertEqual("性能结果总表", html_payload["tables"][2]["title"])
        self.assertIn("一行代表一个性能方法在一台设备上的结果", html_payload["tables"][2]["description"])
        self.assertRegex(json.dumps(html_payload, ensure_ascii=False), r"[\u4e00-\u9fff]")

    def test_build_inventory_outputs_includes_fixed_views_in_roadmap_order(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_fixed_views")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        fixed_views = payload["html"]["fixedViews"]

        self.assertEqual(
            [
                "closure-strip",
                "responsibility-view",
                "route-topology-view",
                "platform-device-matrix",
            ],
            [view["id"] for view in fixed_views],
        )
        self.assertEqual("Closure Strip", fixed_views[0]["title"])
        self.assertEqual("testing-inventory", fixed_views[0]["rows"][0]["scopeCode"])

        route_rows = {row["routeCode"]: row for row in fixed_views[2]["rows"]}
        self.assertIn("managed", route_rows)
        self.assertIn("native", route_rows)
        self.assertIn("hotupdate", route_rows)

        platform_rows = {row["deviceProfileCode"]: row for row in fixed_views[3]["rows"]}
        self.assertIn("inventory-host", platform_rows)
        self.assertIn("bridge/windows-x64/fixture-device", platform_rows)

    def test_build_inventory_outputs_uses_plain_language_benchmark_hints(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_benchmark_plain_language")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        benchmark_inventory = payload["benchmark"]
        headers = {header["key"]: header for header in benchmark_inventory["headers"]}

        self.assertEqual("性能结果总表", benchmark_inventory["title"])
        self.assertIn("平台", headers["platformId"]["hint"]["summary"])
        self.assertIn("设备", headers["deviceId"]["hint"]["definition"])
        self.assertIn("有没有可用结果", headers["managedStatus"]["hint"]["summary"])
        self.assertIn("越小越快", " ".join(headers["managedMeanDurationMs"]["hint"]["values"]))
        self.assertIn("比托管模式快", " ".join(headers["nativeRelativeToManaged"]["hint"]["values"]))
        self.assertIn("过期", headers["isStale"]["hint"]["definition"])

        self.assertIn(
            "verification/archive/master/evidence-claims-master.json",
            headers["managedStatus"]["hint"]["source"],
        )
        self.assertNotIn("docs/benchmark/overview.json", headers["managedStatus"]["hint"]["source"])

    def test_build_inventory_outputs_renders_html_document_with_header_tooltips(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_html_document")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        html_document = payload["htmlDocument"]

        self.assertIn('<strong>字段含义</strong>', html_document)
        self.assertIn('<strong>为什么有这列</strong>', html_document)
        self.assertIn('<strong>数据来源</strong>', html_document)
        self.assertIn('<strong>取值说明</strong>', html_document)
        self.assertIn('<strong>判定规则</strong>', html_document)
        self.assertIn('<strong>空值代表什么</strong>', html_document)
        self.assertIn('const normalizedLabel = label || hint.label || "字段";', html_document)
        self.assertIn('const tooltipTitle = escapeHtml(hint.summary || hint.definition || `${normalizedLabel} 字段说明`);', html_document)
        self.assertIn('const ariaLabel = escapeHtml(`${normalizedLabel} 字段说明`);', html_document)
        self.assertIn('title="${tooltipTitle}"', html_document)
        self.assertIn('aria-label="${ariaLabel}"', html_document)
        self.assertIn('hintHtml(header.hint || {}, header.label || header.key)', html_document)
        self.assertIn('.hint[data-open="true"] .hint-card {', html_document)
        self.assertIn('const positionHintCard = (hint, anchorX, anchorY) => {', html_document)
        self.assertIn('hint.addEventListener("mousemove", (event) => openHint(hint, event.clientX, event.clientY));', html_document)
        self.assertIn('hint.dataset.open = "true";', html_document)
        self.assertIn('delete hint.dataset.open;', html_document)
        self.assertIn("性能结果总表", html_document)

    def test_build_inventory_outputs_renders_fixed_view_sections_before_detail_tables(self) -> None:
        generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_fixed_view_document")
        payload = generator_module.build_inventory_outputs(sample_inventory_source())

        html_document = payload["htmlDocument"]

        self.assertIn('const fixedViews = data.fixedViews || [];', html_document)
        self.assertIn('renderFixedView(view)', html_document)
        self.assertIn('class="view-chip"', html_document)
        self.assertLess(html_document.index("Closure Strip"), html_document.index("Capability Inventory"))
        self.assertLess(html_document.index("Responsibility View"), html_document.index("Capability Inventory"))
