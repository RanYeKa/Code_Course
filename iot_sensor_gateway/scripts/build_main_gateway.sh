#!/bin/bash
# file: gateway/build_main.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# get current dir name
CURRENT_DIR="$(basename "${SCRIPT_DIR}")"

usage() {
    cat <<USAGE
Usage: $0 [--debug|--release|--asan] [--run|--gdb|--test|--clean]

Examples:
  $0
  $0 --debug
  $0 --release
  $0 --asan --test
  $0 --debug --run
  $0 --debug --gdb
  $0 --clean
USAGE
}

MODE="debug"
ACTION="build"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --debug)   MODE="debug" ;;
        --release) MODE="release" ;;
        --asan)    MODE="asan" ;;
        --run)     ACTION="run" ;;
        --gdb)     ACTION="gdb" ;;
        --test)    ACTION="test" ;;
        --clean)   ACTION="clean" ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
    shift
done

# use the current dir name in the build_main.sh
# case "${ACTION}" in
#     build)
#         "${ROOT_DIR}/build_main.sh" --mode "${MODE}" "${CURRENT_DIR}"
#         ;;
#     run)
#         "${ROOT_DIR}/build_main.sh" --mode "${MODE}" --run "${CURRENT_DIR}"
#         ;;
#     gdb)
#         "${ROOT_DIR}/build_main.sh" --mode "${MODE}" --gdb "${CURRENT_DIR}"
#         ;;
#     test)
#         "${ROOT_DIR}/build_main.sh" --mode "${MODE}" --test "${CURRENT_DIR}"
#         ;;
#     clean)
#         "${ROOT_DIR}/build_main.sh" --clean "${CURRENT_DIR}"
#         ;;
# esac

case "${ACTION}" in
    build) make -C "${ROOT_DIR}" MODE="${MODE}" ;;
    run)   make -C "${ROOT_DIR}" MODE="${MODE}" run ;;
    gdb)   make -C "${ROOT_DIR}" MODE="${MODE}" gdb ;;
    test)  make -C "${ROOT_DIR}" MODE="${MODE}" test ;;
    clean) make -C "${ROOT_DIR}" clean ;;
esac