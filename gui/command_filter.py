ALLOWED_COMMANDS = [
    "show", "copy", "move", "delete", "rename", "create",
    "search", "archive", "compare", "sync", "watch",
    "rewrite", "introspect", "grammar", "cryptic", "split",
    "perm", "pipe", "undo", "link", "dedupe",
    "help", "version"
]

def is_allowed(cmd: str) -> bool:
    if not cmd.strip():
        return False
    root = cmd.split()[0]
    return root