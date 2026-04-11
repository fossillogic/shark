import tkinter as tk
from gui.theme import Theme

class ConsoleWidget(tk.Frame):
    def __init__(self, master):
        super().__init__(master, bg=Theme.CONSOLE_BG)

        self.text = tk.Text(
            self,
            bg=Theme.CONSOLE_BG,
            fg=Theme.ACCENT,
            insertbackground=Theme.ACCENT,
            font=("Consolas", 11),
            wrap="word"
        )
        self.text.pack(fill="both", expand=True)

    def write(self, data: str):
        self.text.insert("end", data)
        self.text.see("end")