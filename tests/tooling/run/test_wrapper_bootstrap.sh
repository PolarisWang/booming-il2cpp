#!/usr/bin/env bash
set -euo pipefail

case "$(uname -s)" in
  Linux|Darwin)
    echo "test_wrapper_bootstrap.sh is intended to run on a Unix host once run.sh is implemented."
    ;;
  *)
    echo "Skipping shell wrapper bootstrap test on non-Unix host."
    ;;
esac
