import tkinter as tk
from gui.theme import Theme

COMMANDS = [
    "show", "copy", "move", "delete", "rename", "create",
    "search", "archive", "compare", "sync", "watch",
    "introspect", "grammar", "cryptic", "split"
]

class SidebarWidget(tk.Frame):
    def __init__(self, master, callback):
        super().__init__(master, bg=Theme.PANEL, width=200)
        self.callback = callback

        for cmd in COMMANDS:
            btn = tk.Button(
                self,
                text=cmd,
                fg=Theme.TEXT,
                bg=Theme.PANEL,
                activebackground="#1f2a3d",
                relief="flat",
                command=lambda c=cmd: self.callback(c)
            )
            btn.pack(fill="x", padx=5, pady=2)