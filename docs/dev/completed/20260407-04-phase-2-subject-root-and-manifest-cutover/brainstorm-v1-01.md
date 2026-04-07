# Phase 2 Subject Root And Manifest Cutover Brainstorm v1.01

Date: 2026-04-07
Status: implemented-first-slice

## Goal

- Promote `subjects/` from source-only containers to canonical subject roots.
- Extend `subject.manifest.json` with typed validation and validation-profile sections.
- Land at least one subject-local xUnit sample and one subject-local perf harness sample.

## Decisions

- Keep `defaultGoal / defaultMatrix / supportedGoals` as compatibility fields for now.
- Freeze structure before switching CLI or executor semantics.
- Use HelloWorldObject as the `unit` sample and GenericEcho as the `perf` sample.
