#!/bin/bash
# file name and root related path remidner/build_main.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

"${ROOT_DIR}/build_main.sh" remidner