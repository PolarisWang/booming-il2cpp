# Phase 2 Subject Root And Manifest Cutover Design v1.01

Date: 2026-04-07
Status: implemented-first-slice

## Structure

Each subject now has a stable structure contract:

- `source/`
- `validation/`
- `expected/`
- `baselines/`

## Manifest

Each subject manifest adds:

- `defaultValidationProfile`
- `validationProfiles`
- `validation`
- `expected`
- `baselines`

Compatibility is preserved by leaving the existing goal/matrix fields in place.

## Ownership

- HelloWorldObject owns its xUnit project under `subjects/HelloWorldObject/validation/unit/...`
- GenericEcho owns its perf harness and perf baselines under `subjects/GenericEcho/validation/perf/...` and `subjects/GenericEcho/baselines/perf/...`
