#!/usr/bin/env bash
set -euo pipefail

case "$(uname -s)" in
  Linux|Darwin)
    echo "test_run_cli.sh is intended to run on a Unix host once run.sh command-layer support is implemented."
    ;;
  *)
    echo "Skipping shell CLI test on non-Unix host."
    ;;
esac
