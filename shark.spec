# -*- mode: python ; coding: utf-8 -*-

import os
from PyInstaller.utils.hooks import collect_submodules

block_cipher = None

# ---- PATHS ----
project_root = os.path.abspath(".")
shark_binary = os.path.join(project_root, "builddir", "code", "logic", "shark")

# ---- PYTHON GUI ENTRY ----
gui_script = os.path.join(project_root, "gui", "gui.py")

# ---- ANALYSIS ----
a = Analysis(
    [gui_script],
    pathex=[project_root],
    binaries=[
        (shark_binary, ".")   # embed C CLI binary into app root
    ],
    datas=[],
    hiddenimports=collect_submodules("tkinter"),
    hookspath=[],
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
)

# ---- PYZ (Python bytecode archive) ----
pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

# ---- EXECUTABLE ----
exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name="shark-tool",
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,   # IMPORTANT: GUI ONLY (no terminal window)
    icon=None        # optionally: "assets/shark.ico"
)

# ---- BUILD OUTPUT ----
coll = COLLECT(
    exe,
    a.binaries,
    a.zipfiles,
    a.datas,
    strip=False,
    upx=True,
    name="shark"
)