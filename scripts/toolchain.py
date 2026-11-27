"""Provisions the build's toolchain into .toolchain/ (git-ignored), each step skipped when current:
  llvm-mingw-<host>  the pinned release in llvm-mingw.version, checked against its SHA-256
  case-include       Linux: links for the mixed-case Windows header names the sources use

    python3 scripts/toolchain.py
"""

import require_python  # noqa: F401  (first: stops an old Python before anything else runs)

import hashlib
import os
import re
import shutil
import sys
import urllib.request

from common import (IS_WINDOWS, capture, HOST, LLVM_MINGW_DIR, LOG_DIR, REPO_ROOT, TOOLCHAIN_DIR, Failure, Log, main_guard,
                    read_pin, run, timestamp)

CASE_INCLUDE_DIR = os.path.join(TOOLCHAIN_DIR, "case-include")
SCANNED_FOLDERS = ("header", "source", "modules", "TpfConvert")
SCANNED_EXTENSIONS = (".cpp", ".h", ".hpp", ".inl", ".c", ".ixx")
STAMP_NAME = ".gmod-version"


def download(url, path):
    with urllib.request.urlopen(url) as response, open(path, "wb") as file:
        shutil.copyfileobj(response, file, 1 << 20)


def sha256(path):
    digest = hashlib.sha256()
    with open(path, "rb") as file:
        for block in iter(lambda: file.read(1 << 20), b""):
            digest.update(block)
    return digest.hexdigest()


def read_stamp(folder):
    try:
        with open(os.path.join(folder, STAMP_NAME), encoding="utf-8") as file:
            return file.read().strip()
    except OSError:
        return None


def install_llvm_mingw(log):
    pin = read_pin()
    asset, expected = pin[HOST + "_asset"], pin[HOST + "_sha256"]
    stamp = "%s %s" % (pin["version"], expected)
    if read_stamp(LLVM_MINGW_DIR) == stamp:
        log.step("llvm-mingw %s already installed" % pin["version"])
        return
    log.step("installing llvm-mingw %s into .toolchain/%s/" % (pin["version"], os.path.basename(LLVM_MINGW_DIR)))
    os.makedirs(TOOLCHAIN_DIR, exist_ok=True)
    archive = os.path.join(TOOLCHAIN_DIR, asset)
    download("https://github.com/mstorsjo/llvm-mingw/releases/download/%s/%s" % (pin["version"], asset), archive)
    actual = sha256(archive)
    if actual != expected:
        os.remove(archive)
        raise Failure("llvm-mingw download has SHA-256 %s, expected %s" % (actual, expected))
    extracted = os.path.join(TOOLCHAIN_DIR, re.sub(r"\.(zip|tar\.xz)$", "", asset))
    for old in (LLVM_MINGW_DIR, extracted):
        shutil.rmtree(old, ignore_errors=True)
    # tar keeps the Linux release's symlinks; Windows' own tar.exe (not Git's) unpacks the zip far faster than zipfile.
    tar = os.path.join(os.environ.get("SystemRoot", r"C:\Windows"), "System32", "tar.exe") if IS_WINDOWS else "tar"
    run(log, [tar, "-xf", archive, "-C", TOOLCHAIN_DIR], "unpacking llvm-mingw")
    os.remove(archive)
    os.rename(extracted, LLVM_MINGW_DIR)
    with open(os.path.join(LLVM_MINGW_DIR, STAMP_NAME), "w", encoding="utf-8") as file:
        file.write(stamp + "\n")


def replace_link(target, link):
    os.makedirs(os.path.dirname(link), exist_ok=True)
    if os.path.islink(link):
        if os.readlink(link) == target:
            return
        os.remove(link)
    os.symlink(target, link)


def link_headers():
    mingw_include = os.environ.get("MINGW_INCLUDE_DIR", os.path.join(LLVM_MINGW_DIR, "generic-w64-mingw32", "include"))
    pattern = re.compile(rb"#include <([A-Za-z0-9_./-]+\.h)>")
    included = set()
    for folder in SCANNED_FOLDERS:
        for directory, _, names in os.walk(os.path.join(REPO_ROOT, folder)):
            for name in names:
                if name.endswith(SCANNED_EXTENSIONS):
                    with open(os.path.join(directory, name), "rb") as file:
                        included.update(match.decode() for match in pattern.findall(file.read()))
    for header in sorted(included):
        lowercase = header.lower()
        if os.path.exists(os.path.join(mingw_include, header)) or not os.path.exists(os.path.join(mingw_include, lowercase)):
            continue
        replace_link(os.path.join(mingw_include, lowercase), os.path.join(CASE_INCLUDE_DIR, header))
    # DirectXTex includes <wrl\client.h>; clang on Linux takes the backslash literally, so link that exact name. The
    # dependency file records it with a slash, and a missing dependency rebuilds the file every time, so link that too.
    for windows_path in ("wrl\\client.h",):
        slash_path = windows_path.replace("\\", "/")
        replace_link(os.path.join(mingw_include, slash_path), os.path.join(CASE_INCLUDE_DIR, windows_path))
        replace_link(os.path.join(mingw_include, slash_path), os.path.join(CASE_INCLUDE_DIR, slash_path))


def provision(log):
    install_llvm_mingw(log)
    if not IS_WINDOWS:
        link_headers()


def main():
    if len(sys.argv) != 1:
        raise Failure(__doc__)
    provision(Log(os.path.join(LOG_DIR, "toolchain-%s.log" % timestamp())))


if __name__ == "__main__":
    main_guard(main)
