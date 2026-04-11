import subprocess

class SharkBridge:
    def __init__(self, binary_path="shark"):
        self.binary_path = binary_path

    def execute(self, command: str) -> str:
        try:
            result = subprocess.run(
                [self.binary_path] + command.split(),
                capture_output=True,
                text=True
            )
            return result.stdout + result.stderr
        except Exception as e:
            return f"[Bridge Error] {str(e)}"