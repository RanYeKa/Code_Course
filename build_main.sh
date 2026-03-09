#!/bin/bash
# file: build_main.sh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
    cat <<USAGE
Usage:
  $0 [options] [subproject]

Build behavior:
  - No subproject: build all subprojects under ${ROOT_DIR} that contain a Makefile
  - One subproject: operate only on that subproject

Options:
  --mode <debug|release|asan>   Build mode (default: debug)
  --run                         Build and run the app
  --gdb                         Build and open gdb on the app
  --test                        Build and run tests
  --clean                       Clean build artifacts
  -h, --help                    Show this help

Examples:
  $0
  $0 --mode debug lab
  $0 --mode release lab
  $0 --mode asan --test lab
  $0 --mode debug --run lab
  $0 --mode debug --gdb lab
  $0 --clean lab
USAGE
}

list_projects() {
    local project
    while IFS= read -r project; do
        basename "${project}"
    done < <(
        find "${ROOT_DIR}" \
            -mindepth 1 -maxdepth 1 -type d \
            ! -name '.git' \
            ! -name '.vscode' \
            ! -name 'notes' \
            -exec test -f '{}/Makefile' ';' -print | sort
    )
}

project_exists() {
    local target="$1"
    [[ -d "${ROOT_DIR}/${target}" && -f "${ROOT_DIR}/${target}/Makefile" ]]
}

run_make() {
    local project_name="$1"
    local mode="$2"
    local action="$3"
    local project_dir="${ROOT_DIR}/${project_name}"

    printf "\n=== %s (%s, %s) ===\n" "${project_name}" "${mode}" "${action}"

    case "${action}" in
        build)
            make -C "${project_dir}" MODE="${mode}"
            ;;
        test)
            make -C "${project_dir}" MODE="${mode}" test
            ;;
        run)
            make -C "${project_dir}" MODE="${mode}" run
            ;;
        gdb)
            make -C "${project_dir}" MODE="${mode}" gdb
            ;;
        clean)
            make -C "${project_dir}" clean
            ;;
        *)
            echo "Unknown action: ${action}"
            return 1
            ;;
    esac
}

main() {
    local mode="debug"
    local action="build"
    local project=""

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --mode)
                if [[ $# -lt 2 ]]; then
                    echo "Missing value for --mode"
                    usage
                    exit 1
                fi
                mode="$2"
                shift 2
                ;;
            --run)
                action="run"
                shift
                ;;
            --gdb)
                action="gdb"
                shift
                ;;
            --test)
                action="test"
                shift
                ;;
            --clean)
                action="clean"
                shift
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                if [[ -n "${project}" ]]; then
                    echo "Only one subproject may be specified."
                    usage
                    exit 1
                fi
                project="$1"
                shift
                ;;
        esac
    done

    case "${mode}" in
        debug|release|asan) ;;
        *)
            echo "Invalid mode: ${mode}"
            usage
            exit 1
            ;;
    esac

    if [[ -n "${project}" ]]; then
        if ! project_exists "${project}"; then
            echo "Unknown subproject '${project}'."
            echo "Available subprojects:"
            list_projects | sed 's/^/  - /'
            exit 1
        fi
        run_make "${project}" "${mode}" "${action}"
        exit 0
    fi

    local failed=0
    local p
    while IFS= read -r p; do
        if ! run_make "${p}" "${mode}" "${action}"; then
            failed=1
        fi
    done < <(list_projects)

    if [[ ${failed} -ne 0 ]]; then
        echo "One or more subprojects failed."
        exit 1
    fi

    echo "All subprojects completed successfully."
}

main "$@"