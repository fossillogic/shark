import os
import pty
import subprocess
import threading

class EmbeddedTerminal:
    def __init__(self, on_output, binary="shark"):
        self.on_output = on_output
        self.binary = binary

        self.master_fd = None
        self.slave_fd = None
        self.process = None

    def start(self):
        self.master_fd, self.slave_fd = pty.openpty()

        self.process = subprocess.Popen(
            [self.binary],
            stdin=self.slave_fd,
            stdout=self.slave_fd,
            stderr=self.slave_fd,
            text=True
        )

        threading.Thread(target=self._reader, daemon=True).start()

    def _reader(self):
        while True:
            try:
                data = os.read(self.master_fd, 1024).decode()
                if data:
                    self.on_output(data)
            except OSError:
                break

    def send(self, cmd: str):
        if self.master_fd:
            os.write(self.master_fd, (cmd + "\n").encode())