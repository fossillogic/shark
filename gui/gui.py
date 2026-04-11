import tkinter as tk
from gui.console import ConsoleWidget
from gui.sidebar import SidebarWidget
from gui.input import InputBarWidget
from gui.core_bridge import SharkBridge
from gui.theme import Theme

class SharkApp(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Shark Tool GUI")
        self.geometry("1100x700")
        self.configure(bg=Theme.BG)

        # backend bridge to CLI binary
        self.bridge = SharkBridge()

        self._build_ui()

    def _build_ui(self):
        self.sidebar = SidebarWidget(self, self.run_command)
        self.sidebar.pack(side="left", fill="y")

        self.console = ConsoleWidget(self)
        self.console.pack(side="top", fill="both", expand=True)

        self.inputbar = InputBarWidget(self, self.run_command)
        self.inputbar.pack(side="bottom", fill="x")

    def run_command(self, command: str):
        """Executes Shark CLI commands (except play)."""
        if command.strip().startswith("play"):
            self.console.write("[ERROR] 'play' is disabled in GUI mode\n")
            return

        output = self.bridge.execute(command)
        self.console.write(output + "\n")