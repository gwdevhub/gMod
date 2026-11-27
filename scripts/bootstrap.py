#!/usr/bin/env python3
"""Sets this checkout up to build gMod with llvm-mingw. The native bootstrap.sh / bootstrap.bat check and install
the host prerequisites, then run this. It builds nothing; the last lines say how. Re-running is cheap: each step is
skipped when its output is current.

  1. Installs the pinned llvm-mingw into .toolchain/ (on Linux also the case-insensitive header links).
  2. Checks vcpkg out at the registry baseline into .vcpkg/, unless VCPKG_ROOT already points at a checkout.
  3. Configures the relwithdebinfo preset into build/<host>/relwithdebinfo/ (no build).
"""

import require_python  # noqa: F401  (first: stops an old Python before anything else runs)

import argparse
import json
import os

import toolchain
from common import IS_WINDOWS, LOG_DIR, PRESET_BUILD_DIRS, REPO_ROOT, Failure, Log, capture, find_cmake, main_guard, run, timestamp

LOCAL_VCPKG = os.path.join(REPO_ROOT, ".vcpkg")


# vcpkg's helper scripts are part of each package's ABI hash, so checking it out at the registry baseline gives every
# machine the same hashes, and packages built once come from vcpkg's binary cache after that.
def ensure_vcpkg(log):
    existing = os.environ.get("VCPKG_ROOT")
    if existing and os.path.isfile(os.path.join(existing, "scripts", "buildsystems", "vcpkg.cmake")):
        log.step("using VCPKG_ROOT=" + existing)
        return existing
    with open(os.path.join(REPO_ROOT, "vcpkg-configuration.json"), encoding="utf-8") as file:
        baseline = json.load(file)["default-registry"]["baseline"]
    head = (capture(["git", "-C", LOCAL_VCPKG, "rev-parse", "HEAD"]) or "").strip() if os.path.isdir(
        os.path.join(LOCAL_VCPKG, ".git")) else ""
    if head != baseline:
        log.step("checking vcpkg out at the registry baseline %s into .vcpkg/" % baseline[:12])
        if not os.path.isdir(os.path.join(LOCAL_VCPKG, ".git")):
            run(log, ["git", "init", "-q", LOCAL_VCPKG])
        run(log, ["git", "-C", LOCAL_VCPKG, "fetch", "-q", "--depth", "1", "https://github.com/microsoft/vcpkg.git", baseline])
        run(log, ["git", "-C", LOCAL_VCPKG, "checkout", "-q", "--force", "FETCH_HEAD"])
    if not os.path.isfile(os.path.join(LOCAL_VCPKG, "vcpkg.exe" if IS_WINDOWS else "vcpkg")):
        log.step("bootstrapping vcpkg")
        script = "bootstrap-vcpkg.bat" if IS_WINDOWS else "bootstrap-vcpkg.sh"
        run(log, [os.path.join(LOCAL_VCPKG, script), "-disableMetrics"], script)
    return LOCAL_VCPKG


def configure(log):
    log.step("configuring the relwithdebinfo preset into %s (no build)" % PRESET_BUILD_DIRS["relwithdebinfo"])
    run(log, [find_cmake(), "--preset", "relwithdebinfo"], "cmake configure")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--log", help="append to this log file (the native bootstrap passes the one it started)")
    args = parser.parse_args()
    path = args.log or os.path.join(LOG_DIR, "bootstrap-%s.log" % timestamp())
    log = Log(path)
    log.line("bootstrap log: " + log.path)

    toolchain.provision(log)
    vcpkg_root = ensure_vcpkg(log)
    os.environ["VCPKG_ROOT"] = vcpkg_root
    configure(log)

    log.step("done. Nothing was built.")
    if vcpkg_root != LOCAL_VCPKG:
        log.note("(vcpkg is VCPKG_ROOT=%s; without it set, the presets use .vcpkg/)" % vcpkg_root)
    log.line()
    log.line("What to do next:")
    log.line("  python3 scripts/build.py [--preset debug]   (or: cmake --build build/%s/relwithdebinfo)"
             % ("Windows" if IS_WINDOWS else "Linux"))
    log.line("  The DLL and TpfConvert land in bin/. Re-running this bootstrap is safe and quick.")
    log.close()


if __name__ == "__main__":
    main_guard(main)
