#!/usr/bin/env python3
"""Builds gMod.dll and TpfConvert with llvm-mingw into build/<host>/<preset>/, configuring the preset first when it
has not been configured yet. Output lands in bin/ (the presets point CMAKE_RUNTIME_OUTPUT_DIRECTORY there).

    python3 scripts/build.py [--preset relwithdebinfo|debug|release] [--target gMod|TpfConvert]
                             [--clean | --rebuild]

--target builds only that target (both by default). --clean removes the outputs of the target(s) only, or the whole
build folder when no target is given. --rebuild cleans the target(s) and then builds them. Visual Studio's build,
clean and rebuild commands call this script with these options.
"""

import require_python  # noqa: F401  (first: stops an old Python before anything else runs)

import argparse
import os

from common import (LOG_DIR, PRESET_BUILD_DIRS, PRESETS, REPO_ROOT, FolderLock, Log, find_cmake, main_guard, run,
                    timestamp)

TARGETS = ("gMod", "TpfConvert")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--preset", choices=sorted(PRESETS), default="relwithdebinfo")
    parser.add_argument("--target", choices=TARGETS, action="append",
                        help="build only this target (repeat for several); default is all of them")
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--clean", action="store_true", help="remove the outputs of the target(s), or the whole build")
    action.add_argument("--rebuild", action="store_true", help="clean the target(s), then build them")
    args = parser.parse_args()
    log = Log(os.path.join(LOG_DIR, "build-%s-%s.log" % (args.preset, timestamp())))
    cmake = find_cmake()
    build_dir = os.path.join(REPO_ROOT, PRESET_BUILD_DIRS[args.preset])
    targets = args.target or []
    lock = FolderLock(build_dir, log)
    try:
        if not os.path.isfile(os.path.join(build_dir, "build.ninja")):
            log.step("configuring the %s preset" % args.preset)
            run(log, [cmake, "--preset", args.preset], "cmake configure")
        if args.clean or args.rebuild:
            if targets:
                for target in targets:
                    log.step("cleaning %s (%s preset)" % (target, args.preset))
                    run(log, [cmake, "--build", build_dir, "--target", target, "--", "-t", "clean"], "clean")
            else:
                log.step("cleaning the %s build folder" % args.preset)
                run(log, [cmake, "--build", build_dir, "--target", "clean"], "clean")
            if args.clean:
                log.line("cleaned")
                return
        log.step("building %s (%s preset)" % (", ".join(targets) or "all targets", args.preset))
        command = [cmake, "--build", build_dir]
        for target in targets:
            command += ["--target", target]
        run(log, command, "cmake build")
        log.line("built into bin/")
    finally:
        lock.release()
        log.close()


if __name__ == "__main__":
    main_guard(main)
