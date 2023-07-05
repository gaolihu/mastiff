#!/usr/bin/env bash

#set -e

MASTIFF_ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)"
MASTIFF_IN_DOCKER=false

# If inside docker container
if [ -f /.dockerenv ]; then
    MASTIFF_IN_DOCKER=true
    MASTIFF_ROOT_DIR="/mastiff"
fi

export MASTIFF_CONFIG_HOME="${MASTIFF_CONFIG_HOME:=$HOME/.mastiff}"

export MASTIFF_ROOT_DIR="${MASTIFF_ROOT_DIR}"
export MASTIFF_IN_DOCKER="${MASTIFF_IN_DOCKER}"
export MASTIFF_CACHE_DIR="${MASTIFF_ROOT_DIR}/.cache"

export TAB="    " # 4 spaces

export GPU_SETUP_COMPLETED

: ${VERBOSE:=yes}

STYLE_BOLD='\e[1m'
STYLE_DARK='\e[2m'
STYLE_UNDERLINE='\e[4m'
STYLE_BLINK='\e[5m'
STYLE_REVERSE='\e[7m'
STYLE_HIDE='\e[8m'

STYLE_NORMAL='\e[0m'
STYLE_DEFAULT='\e[0;31m'
STYLE_BLACK='\e[0;30m'
STYLE_RED='\e[0;31m'
STYLE_GREEN='\e[32m'
STYLE_YELLOW='\e[33m'
STYLE_BLUE='\e[34m'
STYLE_MAGENTA='\e[35m'
STYLE_CYAN='\e[36m'
STYLE_LGRAY='\e[37m'
STYLE_DGRAY='\e[90m'
STYLE_LRED='\e[91m'
STYLE_LGREEN='\e[92m'
STYLE_LYELLOW='\e[93m'
STYLE_LBULE='\e[94m'
STYLE_LMAGENTA='\e[95m'
STYLE_LCYAN='\e[96m'
STYLE_WHITE='\e[97m'
CHANGE_LINE='\n'

function log() {
    (echo >&2 -e "${STYLE_LMAGENTA}$*${STYLE_NORMAL}")
}

function log1() {
    (echo >&2 -e "${STYLE_LYELLOW}$*${STYLE_NORMAL}")
}

function info() {
    (echo >&2 -e "[${STYLE_BLUE}${STYLE_BOLD}INFO${STYLE_NORMAL}] ${STYLE_LCYAN}$*${STYLE_NORMAL}")
}

function error() {
    (echo >&2 -e "[${STYLE_RED}${STYLE_BOLD}ERROR${STYLE_NORMAL}] ${STYLE_RED}$*${STYLE_NORMAL}")
}

function warning() {
    (echo >&2 -e "[${STYLE_YELLOW}${STYLE_BOLD}WARNING${STYLE_NORMAL}] ${STYLE_YELLOW}$*${STYLE_NORMAL}")
}

function good() {
    (echo >&2 -e "[${STYLE_GREEN}${STYLE_BOLD} OK ${STYLE_NORMAL}] ${STYLE_LGREEN}$*${STYLE_NORMAL}")
}

function print_delim() {
    echo "---------------------------"
}

function get_now() {
    date +%s
}

function time_elapsed_s() {
    local start="${1:-$(get_now)}"
    local end="$(get_now)"
    echo "$end - $start" | bc -l
}

function success() {
    print_delim
    good "$1"
    print_delim
}

function fail() {
    print_delim
    error "$1"
    print_delim
    exit 1
}

function file_ext() {
    local filename="$(basename $1)"
    local actual_ext="${filename##*.}"
    if [[ "${actual_ext}" == "${filename}" ]]; then
        actual_ext=""
    fi
    echo "${actual_ext}"
}

function c_family_ext() {
    local actual_ext
    actual_ext="$(file_ext $1)"
    for ext in "h" "hh" "hxx" "hpp" "cxx" "cc" "cpp" "cu"; do
        if [[ "${ext}" == "${actual_ext}" ]]; then
            return 0
        fi
    done
    return 1
}

function find_c_cpp_srcs() {
    find "$@" -type f -name "*.h" \
        -o -name "*.c" \
        -o -name "*.hpp" \
        -o -name "*.cpp" \
        -o -name "*.hh" \
        -o -name "*.cc" \
        -o -name "*.hxx" \
        -o -name "*.cxx" \
        -o -name "*.cu"
}

function proto_ext() {
    if [[ "$(file_ext $1)" == "proto" ]]; then
        return 0
    else
        return 1
    fi
}

function find_proto_srcs() {
    find "$@" -type f -name "*.proto"
}

function py_ext() {
    if [[ "$(file_ext $1)" == "py" ]]; then
        return 0
    else
        return 1
    fi
}

function find_py_srcs() {
    find "$@" -type f -name "*.py"
}

function bash_ext() {
    local actual_ext
    actual_ext="$(file_ext $1)"
    for ext in "sh" "bash" "bashrc"; do
        if [[ "${ext}" == "${actual_ext}" ]]; then
            return 0
        fi
    done
    return 1
}

function bazel_extended() {
    local actual_ext="$(file_ext $1)"
    if [[ -z "${actual_ext}" ]]; then
        if [[ "${arg}" == "BUILD" || "${arg}" == "WORKSPACE" ]]; then
            return 0
        else
            return 1
        fi
    else
        for ext in "BUILD" "bazel" "bzl"; do
            if [[ "${ext}" == "${actual_ext}" ]]; then
                return 0
            fi
        done
        return 1
    fi
}

function prettier_ext() {
    local actual_ext
    actual_ext="$(file_ext $1)"
    for ext in "md" "json" "yml"; do
        if [[ "${ext}" == "${actual_ext}" ]]; then
            return 0
        fi
    done
    return 1
}

function find_prettier_srcs() {
    find "$@" -type f -name "*.md" \
        -or -name "*.json" \
        -or -name "*.yml"
}

# Exits the script if the command fails.
function run() {
    if [ "${VERBOSE}" = yes ]; then
        echo "${@}"
        "${@}" || exit $?
    else
        local errfile="${MASTIFF_ROOT_DIR}/.errors.log"
        echo "${@}" >"${errfile}"
        if ! "${@}" >>"${errfile}" 2>&1; then
            local exitcode=$?
            cat "${errfile}" 1>&2
            exit $exitcode
        fi
    fi
}

#commit_id=$(git log -1 --pretty=%H)
function git_sha1() {
    if [ -x "$(which git 2>/dev/null)" ] &&
        [ -d "${MASTIFF_ROOT_DIR}/.git" ]; then
    git rev-parse --short HEAD 2>/dev/null || true
fi
}

function git_date() {
    if [ -x "$(which git 2>/dev/null)" ] &&
        [ -d "${MASTIFF_ROOT_DIR}/.git" ]; then
    git log -1 --pretty=%ai | cut -d " " -f 1 || true
fi
}

function git_time() {
    if [ -x "$(which git 2>/dev/null)" ] &&
        [ -d "${MASTIFF_ROOT_DIR}/.git" ]; then
    git log -1 --pretty=%ai|| true
fi
}

function git_branch() {
    if [ -x "$(which git 2>/dev/null)" ] &&
        [ -d "${MASTIFF_ROOT_DIR}/.git" ]; then
    git rev-parse --abbrev-ref HEAD
else
    echo "@non-git"
fi
}

function read_one_char_from_stdin() {
    local answer
    read -r -n1 answer
    # Bash 4.x+: ${answer,,} to lowercase, ${answer^^} to uppercase
    echo "${answer}" | tr '[:upper:]' '[:lower:]'
}

function optarg_check_for_opt() {
    local opt="$1"
    local optarg="$2"
    if [[ -z "${optarg}" || "${optarg}" =~ ^-.* ]]; then
        error "Missing parameter for ${opt}. Exiting..."
        exit 3
    fi
}

pathremove() {
    local IFS=':'
    local NEWPATH
    local DIR
    local PATHVARIABLE=${2:-PATH}
    for DIR in ${!PATHVARIABLE}; do
        if [ "$DIR" != "$1" ]; then
            NEWPATH=${NEWPATH:+$NEWPATH:}$DIR
        fi
    done
    export $PATHVARIABLE="$NEWPATH"
}

pathprepend() {
    pathremove $1 $2
    local PATHVARIABLE=${2:-PATH}
    export $PATHVARIABLE="$1${!PATHVARIABLE:+:${!PATHVARIABLE}}"
}

pathappend() {
    pathremove $1 $2
    local PATHVARIABLE=${2:-PATH}
    export $PATHVARIABLE="${!PATHVARIABLE:+${!PATHVARIABLE}:}$1"
}

export -f pathremove pathprepend pathappend

success "load mastiff.bashrc"
