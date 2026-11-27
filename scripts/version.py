#!/usr/bin/env python3
"""Prints gMod's file version (MAJOR.MINOR.PATCH.TWEAK) from the VERSION_* variables in CMakeLists.txt.

    python3 scripts/version.py
"""

import os
import re

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def version():
    with open(os.path.join(REPO_ROOT, "CMakeLists.txt"), encoding="utf-8") as file:
        text = file.read()
    parts = []
    for name in ("MAJOR", "MINOR", "PATCH", "TWEAK"):
        match = re.search(r"^set\(VERSION_%s (\d+)\)" % name, text, re.MULTILINE)
        if not match:
            raise SystemExit("VERSION_%s not found in CMakeLists.txt" % name)
        parts.append(match.group(1))
    return ".".join(parts)


if __name__ == "__main__":
    print(version())
