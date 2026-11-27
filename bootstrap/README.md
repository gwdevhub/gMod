# IDE settings

`scripts/bootstrap.py` copies these into the checkout (backing up what it replaces to `bootstrap-backup/`):

- `idea/` (Linux) and `idea-windows/` (Windows): the CLion module file. It excludes the toolchain, vcpkg, build,
  bin and log folders from indexing.
- `vscode/`: settings and recommended extensions for VS Code (clangd and CMake Tools), pointed at the presets and
  `.toolchain/`.

CLion and Rider use the `relwithdebinfo`, `debug` and `release` CMake profiles from `CMakePresets.json`. On Linux the
bootstrap also sets up the Flatpak CLion and Rider: their sandbox reaches the host's CMake and ninja through
`.toolchain/host-tools/bin/host-tool`, and their Default toolchain points at the pinned CMake (`scripts/cmake.version`).

On Windows the bootstrap writes `gMod.slnx` for Visual Studio and Rider, from `scripts/vs_solution.py`. Its build,
clean and rebuild commands call `scripts/build.py --target`. Re-run `python scripts/vs_solution.py` after adding or
removing source files.

Close CLion and Rider before running the bootstrap: both rewrite their own settings on exit. `bootstrap.sh` and
`bootstrap.bat` refuse to run while either is open.
