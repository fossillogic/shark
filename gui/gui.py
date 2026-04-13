import tkinter as tk

from gui.theme import Theme
from gui.terminal import EmbeddedTerminal
from gui.sidebar import Sidebar
from gui.inputbar import InputBar
from gui.command_filter import is_allowed


class SharkApp(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Shark Tool — Embedded Terminal")
        self.geometry("1200x750")
        self.configure(bg=Theme.BG)

        self._build_ui()
        self._start_terminal()

    def _build_ui(self):

        # sidebar
        self.sidebar = Sidebar(self, self.run_command)
        self.sidebar.pack(side="left", fill="y")

        # terminal output
        self.console = tk.Text(
            self,
            bg=Theme.CONSOLE_BG,
            fg=Theme.ACCENT,
            insertbackground=Theme.ACCENT,
            font=Theme.FONT,
            wrap="word"
        )
        self.console.pack(fill="both", expand=True)

        # input bar
        self.input = InputBar(self, self.run_command)
        self.input.pack(side="bottom", fill="x")

    def _start_terminal(self):
        self.term = EmbeddedTerminal(self.write_output)
        self.term.start()

    def write_output(self, data: str):
        self.console.insert("end", data)
        self.console.see("end")

    def run_command(self, cmd: str):
        if not is_allowed(cmd):
            self.write_output("[BLOCKED] command rejected\n")
            return

        if cmd.strip().startswith("play"):
            self.write_output("[GUI] play disabled\n")
            return

        self.term.send(cmd)


if __name__ == "__main__":
    app = SharkApp()
    app.mainloop()