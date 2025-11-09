# 🦈 Shark Tool
### A Command-Line Power Utility by **Fossil Logic**

Shark Tool is a modern, **all-in-one command-line toolkit** designed for administrators, developers, and power users. It streamlines complex file and system operations into a single, unified interface — combining traditional UNIX-style commands with **Jellyfish AI integration** for intelligent automation and analysis.

---

## 🚀 Features

- 🧰 Common file and directory operations (show, copy, move, delete, etc.)
- 🧠 Integrated **Jellyfish AI** commands (`chat`, `ask`, `summary`)
- 🗂 Archive creation and extraction
- 🔍 Recursive search and content filtering
- 🕒 Smart file timestamp management
- 🧩 Cross-platform design (Linux, macOS, Windows)
- 💬 AI-augmented explanations, analysis, and contextual assistance

## 🧭 Command Palette

---

### 🗂 Core File Operations

| **Command** | **Description** | **Common Flags** |
|--------------|-----------------|------------------|
| `show` | Display files and directories. | `-a, --all` Show hidden files<br>`-l, --long` Detailed info<br>`-h, --human` Human-readable sizes<br>`-r, --recursive` Include subdirs<br>`--as` format options "list", tree, graph<br>`--time` Show timestamps |
| `move` | Move or rename files/directories. | `-f, --force` Overwrite without prompt<br>`-i, --interactive` Ask before overwrite<br>`-b, --backup` Backup before move |
| `copy` | Copy files or directories. | `-r, --recursive` Copy subdirectories<br>`-v, --verbose` Print each file<br>`-u, --update` Only copy newer<br>`-p, --preserve` Keep permissions/timestamps |
| `remove` / `delete` | Delete files or directories. | `-r, --recursive` Delete contents<br>`-f, --force` No confirmation<br>`-i, --interactive` Confirm per file<br>`--trash` Move to system trash |
| `rename` | Rename files or directories. | `-f, --force` Overwrite target<br>`-i, --interactive` Confirm before overwrite |
| `create` | Create new directories. | `-p, --parents` Create parent dirs<br>`-v, --verbose` Print each dir |
| `touch` | Create or update file timestamps. | `-a` Update access time<br>`-m` Update modify time<br>`-t <timestamp>` Custom timestamp<br>`-c, --no-create` Skip missing files |
| `search` | Find files by name or content. | `-r, --recursive` Include subdirs<br>`-n, --name` Match filename<br>`-c, --content` Search in file contents<br>`-i, --ignore-case` Case-insensitive |
| `archive` | Create, extract, or list archives. | `-c, --create` New archive<br>`-x, --extract` Extract contents<br>`-l, --list` List archive<br>`-f <format>` Format: zip/tar/gz<br>`-p, --password` Encrypt archive |
| `view` | Output file contents to terminal. | `-n, --number` Number all lines<br>`-b, --non-blank` Number non-empty lines<br>`-s, --squeeze` Remove blank lines<br>`-h, --head <n>` First *n* lines<br>`-t, --tail <n>` Last *n* lines<br>`--color` Highlight syntax<br>`--time` Show timestamps |
| `compare` | Compare two files/directories (text, binary, or semantic AI diff). | `-t, --text` Line diff<br>`-b, --binary` Binary diff<br>`--ai` Semantic diff<br>`--context <n>` Show context lines<br>`--ignore-case` Ignore case differences |
| `help` | Display help or ask AI for guidance. | `-a, --ask <query>` Ask Jellyfish AI<br>`-l, --list` List all commands<br>`--examples` Show usage examples<br>`--man` Full manual |

---

### 🤖 AI Commands (Jellyfish Integration)

| **Command** | **Description** | **Common Flags** |
|--------------|-----------------|------------------|
| `chat` | Start an interactive AI chat session. | `-f, --file <path>` Use file content<br>`-m, --model <name>` Select model<br>`-s, --system <role>` AI persona<br>`--save <path>` Save chat transcript<br>`--context` Keep session history |
| `ask` | Ask Jellyfish AI a one-shot question. | `-f, --file <path>` Provide file context<br>`--explain` Force explanation<br>`--analyze` Deep analysis<br>`-q, --quiet` Minimal output |
| `summery` | Generate a concise AI summary of a file or directory. | `-f, --file <path>` Use file content<br>`--depth <level>` Summary depth<br>`-q, --quiet` Minimal output<br>`--color` Highlight key items<br>`--time` Include timestamps |

---

### 🌍 Global Flags (Available to All Commands)

| **Flag** | **Description** |
|-----------|-----------------|
| `--help` | Show command help. |
| `--version` | Display Shark Tool version. |
| `-v, --verbose` | Enable detailed output. |
| `-q, --quiet` | Suppress standard output. |
| `--dry-run` | Simulate actions without changes. |
| `--color` | Colorize output where applicable. |
| `--time` | Display timestamps in output. |

### 🧩 Usage Examples

| **Example** | **Description** |
|--------------|-----------------|
| `shark show -lh --as=graph --time` | Show files in long, human-readable format with as graph display and timestamps. |
| `shark copy -rp src/ backup/` | Copy directory recursively, preserving attributes. |
| `shark search -rc "config"` | Recursively search for “config” inside files. |
| `shark archive -c -f zip project.zip src/` | Create a ZIP archive from the `src/` directory. |
| `shark view -n --color --time file.txt` | View file content with line numbers, syntax highlighting, and timestamps. |
| `shark ask -f logs.txt --analyze` | Ask Jellyfish AI to analyze a log file. |
| `shark chat "Help me debug this error message"` | Start an interactive AI chat session in the terminal. |
| `shark summery -f report.txt --depth 2 --color --time` | Generate a concise AI summary of a file, highlighting key points with timestamps. |
| `shark compare main_v1.c main_v2.c --ai --context 5` | Compare two source files with semantic AI diff and show 5 lines of surrounding context. |
| `shark help --ask "How do I safely delete directories?"` | Ask AI-powered help for guidance on safe file deletion. |

## **Prerequisites**

Ensure you have the following installed before starting:

- **Meson Build System**: This project relies on Meson. For installation instructions, visit the official [Meson website](https://mesonbuild.com/Getting-meson.html).

## **Setting Up Meson Build**

1. **Install Meson**:
    - Follow the installation guide on the [Meson website](https://mesonbuild.com/Getting-meson.html) for your operating system.

## **Setting Up, Compiling, Installing, and Running the Project**

1. **Clone the Repository**:

    ```sh
    git clone https://github.com/fossillogic/shark.git
    cd shark
    ```

2. **Configure the Build**:

    ```sh
    meson setup builddir
    ```

3. **Compile the Project**:

    ```sh
    meson compile -C builddir
    ```

4. **Install the Project**:

    ```sh
    meson install -C builddir
    ```

5. **Run the Project**:

    ```sh
    shark --help
    ```

## **Contributing**

Interested in contributing? Please open pull requests or create issues on the [GitHub repository](https://github.com/fossillogic/shark).

## **Feedback and Support**

For issues, questions, or feedback, open an issue on the [GitHub repository](https://github.com/fossillogic/shark/issues).

## **License**

This project is licensed under the [Mozilla Public License](LICENSE).
