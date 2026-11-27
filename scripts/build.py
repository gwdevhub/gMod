#!/usr/bin/env python3
"""Builds gMod.dll and TpfConvert with llvm-mingw into build/<host>/<preset>/, configuring the preset first when it
has not been configured yet. Output lands in bin/ (the presets point CMAKE_RUNTIME_OUTPUT_DIRECTORY there).

    python3 scripts/build.py [--preset relwithdebinfo|debug|release]
"""

import require_python  # noqa: F401  (first: stops an old Python before anything else runs)

import argparse
import os

from common import (LOG_DIR, PRESET_BUILD_DIRS, PRESETS, REPO_ROOT, FolderLock, Log, find_cmake, main_guard, run,
                    timestamp)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--preset", choices=sorted(PRESETS), default="relwithdebinfo")
    args = parser.parse_args()
    log = Log(os.path.join(LOG_DIR, "build-%s-%s.log" % (args.preset, timestamp())))
    cmake = find_cmake()
    build_dir = os.path.join(REPO_ROOT, PRESET_BUILD_DIRS[args.preset])
    lock = FolderLock(build_dir, log)
    try:
        if not os.path.isfile(os.path.join(build_dir, "build.ninja")):
            log.step("configuring the %s preset" % args.preset)
            run(log, [cmake, "--preset", args.preset], "cmake configure")
        log.step("building the %s preset" % args.preset)
        run(log, [cmake, "--build", build_dir], "cmake build")
        log.line("built into bin/")
    finally:
        lock.release()
        log.close()


if __name__ == "__main__":
    main_guard(main)
