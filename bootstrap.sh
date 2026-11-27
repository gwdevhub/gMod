#!/usr/bin/env bash
# Checks and installs what Linux needs before scripts/bootstrap.py can run, then runs it. bootstrap.py installs
# llvm-mingw and vcpkg into the checkout and configures the build. It builds nothing; the last lines say how.
#
#   1. Installs the missing host packages with apt (Debian/Ubuntu) or dnf (Fedora): git, curl, python3, ninja,
#      and vcpkg's zip/unzip/pkg-config. CMake 3.30+ is only checked.
#   2. Runs scripts/bootstrap.py.
#
# Run it from anywhere:  bash bootstrap.sh
set -euo pipefail

if [ "$(uname -s)" != "Linux" ]; then
    echo "bootstrap.sh is for Linux. On Windows run bootstrap.bat instead." >&2
    exit 1
fi

REPO_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cd "$REPO_ROOT"

# Everything this script prints also goes to a timestamped log, which bootstrap.py then appends to.
mkdir -p logs
BOOTSTRAP_LOG="${REPO_ROOT}/logs/bootstrap-$(date +%Y%m%d-%H%M%S).log"
exec 3>&1 4>&2
exec > >(tee -a "$BOOTSTRAP_LOG") 2>&1
echo "bootstrap log: ${BOOTSTRAP_LOG}"

log() { echo "==> $*"; }
die() { echo "bootstrap: $*" >&2; exit 1; }

APT_PACKAGES=(git curl tar xz-utils python3 ninja-build zip unzip pkg-config)
DNF_PACKAGES=(git curl tar xz python3 ninja-build zip unzip pkgconf-pkg-config)

if command -v dpkg-query >/dev/null 2>&1; then
    missing=()
    for pkg in "${APT_PACKAGES[@]}"; do
        dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q "install ok installed" || missing+=("$pkg")
    done
    if [ "${#missing[@]}" -eq 0 ]; then
        log "host packages already present"
    else
        log "installing missing packages with apt: ${missing[*]}"
        sudo apt-get update
        sudo apt-get install -y --no-install-recommends "${missing[@]}"
    fi
elif command -v dnf >/dev/null 2>&1; then
    missing=()
    for pkg in "${DNF_PACKAGES[@]}"; do
        rpm -q "$pkg" >/dev/null 2>&1 || missing+=("$pkg")
    done
    if [ "${#missing[@]}" -eq 0 ]; then
        log "host packages already present"
    else
        log "installing missing packages with dnf: ${missing[*]}"
        sudo dnf install -y "${missing[@]}"
    fi
else
    die "this script installs packages with apt or dnf; on another distro install the equivalents of: ${APT_PACKAGES[*]}, then run python3 scripts/bootstrap.py"
fi

# cmake is not installed by apt here (the project needs >= 3.30 for import std, newer than some distros ship), so
# only check it.
command -v cmake >/dev/null 2>&1 || die "cmake not found; install cmake >= 3.30 (e.g. pip install --user 'cmake>=3.30')"
version=$(cmake --version | head -1 | awk '{print $3}')
if [ "$(printf '%s\n' 3.30 "$version" | sort -V | head -1)" != "3.30" ]; then
    die "cmake ${version} is too old; the project needs >= 3.30"
fi
python3 "${REPO_ROOT}/scripts/require_python.py" >/dev/null || die "install a newer python3, then re-run"

exec 1>&3 2>&4 3>&- 4>&-
exec python3 "${REPO_ROOT}/scripts/bootstrap.py" --log "$BOOTSTRAP_LOG"
