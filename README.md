<p align="center">
    <img src=".github/logo.png" alt="Shark Tool Logo" width="620"/>
</p>

### A Command-Line Power Utility by **Fossil Logic**

Shark Tool is the ultimate **all-in-one file and system administration utility**. Built for admins, developers, and power users, Shark unifies essential file management, automation, and analysis tasks into a single, powerful command-line interface—eliminating the need for multiple separate tools. Its unique commands, such as `grammar` for advanced text analysis, `summary` for structured file summaries, and `introspect` for deep file inspection, set Shark apart by providing capabilities rarely found in traditional CLI utilities.

---

## Features

- Comprehensive file and directory operations (`show`, `copy`, `move`, `delete`, `rename`, `create`)
- Powerful archive creation, extraction, and listing (zip, tar, gz)
- Advanced recursive search by name or content with filtering options
- Smart file timestamp and metadata management
- Cross-platform compatibility (Linux, macOS, Windows)
- Command palette for quick access to all features
- Grammar analysis, correction, and tone detection via SOAP API
- Structured file summaries and statistics (keywords, topics, entropy)
- File synchronization and backup with flexible options
- Real-time file monitoring and change detection

## Command Palette

---

### Core File Operations

| **Command** | **Description** | **Flags** |
|-------------|-----------------|-----------|
| `show` | Display files and directories. | `-a`, `--all` (show hidden)<br>`-l`, `--long` (detailed info)<br>`-h`, `--human` (human-readable sizes)<br>`-r`, `--recursive` (include subdirs)<br>`-d`, `--depth <n>` (limit recursion)<br>`--as <mode>` (format: list/tree/graph)<br>`--time` (show timestamps) |
| `move` | Move or rename files/directories. | `-f`, `--force` (overwrite)<br>`-i`, `--interactive` (confirm overwrite)<br>`-b`, `--backup` (backup before move) |
| `copy` | Copy files or directories. | `-r`, `--recursive` (copy subdirs)<br>`-u`, `--update` (only newer)<br>`-p`, `--preserve` (keep permissions/timestamps) |
| `remove` / `delete` | Delete files or directories. | `-r`, `--recursive` (delete contents)<br>`-f`, `--force` (no confirmation)<br>`-i`, `--interactive` (confirm per file)<br>`--trash` (move to trash)<br>`--wipe` (secure overwrite before delete)<br>`--shred <passes>` (multi-pass secure deletion)<br>`--older-than <time>` (delete files older than)<br>`--larger-than <size>` (delete files larger than)<br>`--empty` (delete only empty dirs)<br>`--log <file>` (write deletion log) |
| `rename` | Rename files or directories. | `-f`, `--force` (overwrite target)<br>`-i`, `--interactive` (confirm overwrite) |
| `create` | Create new directories or files. | `-p`, `--parents` (create parent dirs)<br>`-t`, `--type <type>` (file or dir) |
| `search` | Find files by name or content. | `-r`, `--recursive` (include subdirs)<br>`-n`, `--name <pattern>` (filename match)<br>`-c`, `--content <pattern>` (search contents)<br>`-i`, `--ignore-case` (case-insensitive) |
| `archive` | Create, extract, or list archives. | `-c`, `--create` (new archive)<br>`-x`, `--extract` (extract)<br>`-l`, `--list` (list archive)<br>`-f <format>` (zip/tar/gz)<br>`-p`, `--password <pw>` (encrypt)<br>`--compress-level <1-9>` (compression level)<br>`--solid` (solid archive)<br>`--split <size>` (split archive)<br>`--stdout` (output to stdout)<br>`--verify` (verify archive)<br>`--sign` (sign archive)<br>`--exclude <pattern>` (exclude files) |
| `view` | Output file contents to terminal. | `-n`, `--number` (number lines)<br>`-b`, `--non-blank` (number non-blank)<br>`-s`, `--squeeze` (remove blank lines)<br>`-h`, `--head <n>` (first n lines)<br>`-t`, `--tail <n>` (last n lines)<br>`--time` (show timestamps) |
| `compare` | Compare two files/directories. | `-t`, `--text` (line diff)<br>`-b`, `--binary` (binary diff)<br>`--context <n>` (context lines)<br>`--ignore-case` (ignore case) |
| `help` | Display help for commands. | `--examples` (usage examples)<br>`--man` (full manual) |
| `sync` | Synchronize files/directories. | `-r`, `--recursive` (include subdirs)<br>`-u`, `--update` (only newer)<br>`--delete` (remove extraneous files) |
| `watch` | Monitor files or directories. | `-r`, `--recursive` (include subdirs)<br>`-e`, `--events <list>` (event filter)<br>`-t`, `--interval <n>` (poll interval) |
| `rewrite` | Modify file contents or metadata. | `-a`, `--append` (append)<br>`--in-place` (edit in place)<br>`--access-time` (update atime)<br>`--mod-time` (update mtime)<br>`--size <n>` (set file size) |
| `introspect` | Examine file contents/type/meta. | `--head <n>` (first n lines)<br>`--tail <n>` (last n lines)<br>`--count` (count lines/words/bytes)<br>`--type` (show type)<br>`--fson` (FSON output) |
| `grammar` | Analyze/correct grammar/style via SOAP API. | `--check` (analyze grammar & style)<br>`--correct` (apply grammar correction)<br>`--sanitize` (clean unsafe language)<br>`--suggest` (improvement suggestions)<br>`--summarize` (concise summary)<br>`--score` (readability/clarity/quality scores)<br>`--tone` (detect tone)<br>`--detect <type>` (detect traits: `conspiracy`, `spam`, `ragebait`, `clickbait`, `bot`, `marketing`, `technobabble`, `hype`, `political`, `offensive`, `misinfo`, `brain_rot`, `formal`, `casual`, `sarcasm`, `neutral`, `aggressive`, `emotional`, `passive`, `snowflake`, `redundant`, `poor_cohesion`, `repeated_words`)<br>`--reflow-width <n>` (reflow to width)<br>`--capitalize <mode>` (sentence-case or title-case)<br>`--format` (pretty-print with indentation)<br>`--declutter` (repair whitespace & word boundaries)<br>`--punctuate` (normalize punctuation) |
| `cryptic` | Encode or decode text using various ciphers. | `-e`, `--encode` (encode text)<br>`-d`, `--decode` (decode text)<br>`-c`, `--cipher <type>` (cipher: `caesar`, `vigenere`, `base64`, `base32`, `binary`, `morse`, `baconian`, `railfence`, `haxor`, `leet`, `rot13`, `atbash`) |

---

### Global Flags (Available to All Commands)

| **Flag** | **Description** |
|-----------|-----------------|
| `--help` | Show command help. |
| `--version` | Display Shark Tool version. |
| `-v, --verbose` | Enable detailed output. |
| `-q, --quiet` | Suppress standard output. |
| `--dry-run` | Simulate actions without changes. |
| `--color` | Colorize output where applicable. |
| `--time` | Display timestamps in output. |

---

### Usage Examples

| **Example** | **Description** |
|--------------|-----------------|
| `shark show -alh --as=tree --time` | List all files (including hidden) in long, human-readable format as a tree, with timestamps. |
| `shark move -i -b old.txt archive/old.txt` | Move a file interactively, creating a backup before moving. |
| `shark copy -rp src/ backup/` | Recursively copy the `src/` directory to `backup/`, preserving permissions and timestamps. |
| `shark remove -r --trash temp/` | Recursively move the `temp/` directory and its contents to the system trash. |
| `shark rename -i draft.md final.md` | Rename a file with confirmation before overwriting the target. |
| `shark create -p -t dir logs/archive/2024/` | Create a nested directory structure for logs. |
| `shark search -rc "config"` | Recursively search for the string "config" inside files. |
| `shark archive -c -f tar project.tar src/` | Create a TAR archive from the `src/` directory. |
| `shark view -n -h 20 --time notes.txt` | View the first 20 lines of a file with line numbers and timestamps. |
| `shark compare -t main_v1.c main_v2.c --context 5` | Show a line-by-line diff of two files with 5 lines of context. |
| `shark sync -ru src/ dest/` | Synchronize files from `src/` to `dest/`, copying only newer files recursively. |
| `shark watch -r -e create,delete src/` | Monitor the `src/` directory recursively for file creation and deletion events. |
| `shark rewrite -i --append log.txt "New entry"` | Append a new entry to a log file in-place. |
| `shark introspect --mime report.pdf` | Show the MIME type of a file. |
| `shark grammar --check --tone notes.txt` | Run a grammar check and detect tone in a text file. |

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

This project is licensed under the [Apache Public License](LICENSE).
