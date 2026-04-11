BLOCKED_COMMANDS = {"play"}

ALLOWED_COMMANDS = [
    "show", "copy", "move", "delete", "rename", "create",
    "search", "archive", "compare", "sync", "watch",
    "rewrite", "introspect", "grammar", "cryptic", "split",
    "perm", "snapshot", "pipe", "alias", "undo", "link", "dedupe",
    "help", "version"
]

def is_allowed(cmd: str) -> bool:
    if not cmd.strip():
        return False
    root = cmd.split()[0]
    return root not in BLOCKED_COMMANDS