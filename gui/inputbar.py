import tkinter as tk
from gui.theme import Theme
from gui.command_filter import is_allowed

class InputBar(tk.Frame):
    def __init__(self, master, send_callback):
        super().__init__(master, bg=Theme.PANEL)

        self.send_callback = send_callback

        self.entry = tk.Entry(
            self,
            bg="#101522",
            fg=Theme.ACCENT,
            insertbackground=Theme.ACCENT
        )
        self.entry.pack(fill="x", padx=10, pady=8)

        self.entry.bind("<Return>", self._on_enter)

    def _on_enter(self, event=None):
        cmd = self.entry.get()
        self.entry.delete(0, "end")

        if is_allowed(cmd):
            self.send_callback(cmd)
        else:
            self.send_callback("[BLOCKED COMMAND]")