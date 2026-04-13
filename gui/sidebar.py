import tkinter as tk
from gui.theme import Theme

COMMANDS = [
    "show", "copy", "move", "delete", "rename", "create",
    "search", "archive", "compare", "sync", "watch",
    "introspect", "grammar", "cryptic", "split"
]

class Sidebar(tk.Frame):
    def __init__(self, master, on_command):
        super().__init__(master, bg=Theme.PANEL, width=200)
        self.on_command = on_command

        for cmd in COMMANDS:
            btn = tk.Button(
                self,
                text=cmd,
                fg=Theme.ACCENT,
                bg=Theme.PANEL,
                activebackground="#1f2a3d",
                relief="flat",
                command=lambda c=cmd: self.on_command(c)
            )
            btn.pack(fill="x", padx=6, pady=2)