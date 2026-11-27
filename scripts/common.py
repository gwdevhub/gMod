"""Shared by the build's Python scripts: repo paths, presets, logging, processes and the build-folder lock."""

import datetime
import os
import re
import shutil
import subprocess
import sys
import time

IS_WINDOWS = os.name == "nt"
HOST = "windows" if IS_WINDOWS else "linux"
# CMake's name for the host, which the presets put in each build folder: build/Windows/, build/Linux/.
CMAKE_HOST = "Windows" if IS_WINDOWS else "Linux"
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TOOLCHAIN_DIR = os.path.join(REPO_ROOT, ".toolchain")
LLVM_MINGW_DIR = os.path.join(TOOLCHAIN_DIR, "llvm-mingw-" + HOST)
LOG_DIR = os.path.join(REPO_ROOT, "logs")
EXE = ".exe" if IS_WINDOWS else ""

PRESETS = {"relwithdebinfo": "RelWithDebInfo", "debug": "Debug", "release": "Release"}
PRESET_BUILD_DIRS = {preset: os.path.join("build", CMAKE_HOST, preset) for preset in PRESETS}
BIN_DIR = os.path.join(REPO_ROOT, "bin")
CMAKE_MINIMUM = (3, 30)  # import std needs 3.30


class Failure(Exception):
    pass


class Log:
    def __init__(self, path):
        self.path = path
        os.makedirs(os.path.dirname(path), exist_ok=True)
        self.file = open(path, "a", encoding="utf-8", newline="\n")

    def line(self, text=""):
        console = sys.stdout.encoding or "utf-8"
        print(text.encode(console, errors="replace").decode(console), flush=True)
        self.file.write(text + "\n")
        self.file.flush()

    def step(self, text):
        self.line("==> " + text)

    def note(self, text):
        self.line("    " + text)

    def close(self):
        self.file.close()


def timestamp():
    return datetime.datetime.now().strftime("%Y%m%d-%H%M%S")


def run(log, command, what=None, env=None, cwd=REPO_ROOT, check=True):
    process = subprocess.Popen(command, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for raw in process.stdout:
        log.line(raw.decode("utf-8", errors="replace").rstrip("\r\n"))
    code = process.wait()
    if check and code != 0:
        raise Failure("%s failed with exit code %d" % (what or os.path.basename(command[0]), code))
    return code


def capture(command, cwd=REPO_ROOT):
    try:
        result = subprocess.run(command, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    except OSError:
        return None
    return result.stdout.decode("utf-8", errors="replace") if result.returncode == 0 else None


def read_pin():
    pin = {}
    with open(os.path.join(REPO_ROOT, "scripts", "llvm-mingw.version"), encoding="utf-8") as file:
        for line in file:
            match = re.match(r"^(\w+)=(.+)$", line.strip())
            if match:
                pin[match.group(1)] = match.group(2).strip()
    return pin


def cmake_version(path):
    output = capture([path, "--version"]) or ""
    match = re.search(r"cmake version (\d+)\.(\d+)", output)
    return (int(match.group(1)), int(match.group(2))) if match else None


def find_tool(name):
    return shutil.which(name)


def find_cmake():
    cmake = find_tool("cmake")
    if not cmake or (cmake_version(cmake) or (0, 0)) < CMAKE_MINIMUM:
        raise Failure("CMake %d.%d+ is needed (the project uses import std); install it or run the bootstrap script"
                      % CMAKE_MINIMUM)
    return cmake


class FolderLock:
    # Two ninja runs in one build folder at once corrupt its logs, so runs on a folder take turns.

    def __init__(self, folder, log):
        os.makedirs(folder, exist_ok=True)
        self.file = open(os.path.join(folder, ".build.lock"), "a+b")
        if not self._try():
            log.step("waiting for another build in %s to finish" % os.path.relpath(folder, REPO_ROOT))
            while not self._try():
                time.sleep(0.5)

    def _try(self):
        try:
            if IS_WINDOWS:
                import msvcrt
                self.file.seek(0)
                msvcrt.locking(self.file.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(self.file.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
            return True
        except OSError:
            return False

    def release(self):
        if IS_WINDOWS:
            import msvcrt
            self.file.seek(0)
            msvcrt.locking(self.file.fileno(), msvcrt.LK_UNLCK, 1)
        self.file.close()


def main_guard(function):
    try:
        sys.exit(function() or 0)
    except Failure as error:
        print("error: %s" % error, file=sys.stderr, flush=True)
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(130)
