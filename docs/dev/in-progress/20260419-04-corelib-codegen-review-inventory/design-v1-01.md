# CoreLib Codegen Review Inventory Design v1.01

## 1. Goal

Produce the first real codegen review input set for `System.Private.CoreLib`.

The minimum useful result is:

- a generated file inventory source,
- a `codegen-review.json` update,
- a paired `codegen-review.md` update,
- no false claim that native proof or hotupdate proof are complete.

## 2. Boundary

This task is allowed to change:

- CoreLib audit packet codegen review files.
- tests that validate the codegen review inventory.
- optional tooling if a small inventory generator already fits existing patterns.

This task is not allowed to mark `completion-report.json` as `completed`.

## 3. Inventory Rules

The review must distinguish:

- source input files,
- generated native files,
- native dispatch manifest,
- hotupdate binding manifest,
- runtime helper bindings,
- public member coverage.

If no full CoreLib generated native file exists yet, the review remains `blocked`, but must state that from a real inventory check rather than an empty placeholder.

