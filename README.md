# Shark Tool
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

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|-----------------|
| `show` | Display files and directories. | `-a, --all` Show hidden files<br>`-l, --long` Detailed info<br>`-h, --human` Human-readable sizes<br>`-r, --recursive` Include subdirs<br>`-d, --depth <n>` Limit recursion depth<br>`--as` Format options **"list"**, **tree**, **graph**, **tiles**.<br>`--time` Show timestamps |
| `move` | Move or rename files/directories. | `-f, --force` Overwrite without prompt<br>`-i, --interactive` Ask before overwrite<br>`-b, --backup` Backup before move |
| `copy` | Copy files or directories. | `-r, --recursive` Copy subdirectories<br>`-u, --update` Only copy newer<br>`-p, --preserve` Keep permissions/timestamps |
| `remove` / `delete` | Delete files or directories. | `-r, --recursive` Delete contents<br>`-f, --force` No confirmation<br>`-i, --interactive` Confirm per file<br>`--trash` Move to system trash |
| `rename` | Rename files or directories. | `-f, --force` Overwrite target<br>`-i, --interactive` Confirm before overwrite |
| `create` | Create new directories or files. | `-p, --parents` Create parent dirs<br>`-t, --type` Create file or dir |
| `search` | Find files by name or content. | `-r, --recursive` Include subdirs<br>`-n, --name` Match filename<br>`-c, --content` Search in file contents<br>`-i, --ignore-case` Case-insensitive |
| `archive` | Create, extract, or list archives. | `-c, --create` New archive<br>`-x, --extract` Extract contents<br>`-l, --list` List archive<br>`-f <format>` Format: zip/tar/gz<br>`-p, --password` Encrypt archive |
| `view` | Output file contents to terminal. | `-n, --number` Number all lines<br>`-b, --non-blank` Number non-empty lines<br>`-s, --squeeze` Remove blank lines<br>`-h, --head <n>` First *n* lines<br>`-t, --tail <n>` Last *n* lines<br>`--time` Show timestamps |
| `compare` | Compare two files/directories (text or binary). | `-t, --text` Line diff<br>`-b, --binary` Binary diff<br>`--context <n>` Show context lines<br>`--ignore-case` Ignore case differences |
| `help` | Display help for supported commands. | `--examples` Show usage examples<br>`--man` Full manual |
| `sync` | Synchronize files/directories. | `-r, --recursive` Include subdirs<br>`-u, --update` Copy only newer<br>`--delete` Remove extraneous files from target |
| `watch` | Continuously monitor files or directories. | `-r, --recursive` Include subdirs<br>`-e, --events <list>` Filter events: create/modify/delete<br>`-t, --interval <n>` Poll interval in seconds |
| `rewrite` | Modify or update file contents, timestamps, or size. | `-i` In-place edit<br>`--append` Append instead of overwrite<br>`-n` No newline<br>`--size <n>` Set exact file size |
| `introspect` | Examine file contents, type, or metadata. | `-n <lines>` Number of lines<br>`-c` Count lines, words, bytes<br>`--fson` FSON output<br>`--mime` Show MIME type |
| `grammar` | Perform grammar analysis, correction, sanitization, tone detection, and content-risk scanning through the SOAP API. | `--check` Run grammar check<br>`--fix` Auto-correct grammar<br>`--sanitize` Remove rot-brain/meme language<br>`--suggest` Suggest alternatives<br>`--tone` Detect tone<br>`--detect <type>` Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble |
| `summary` | Generate a structured summary of one or more files (text, logs, code, docs). | `-l, --lines <n>` Limit lines analyzed<br>`--auto` Auto-detect file type<br>`--keywords` Extract keywords<br>`--topics` Topic clustering<br>`--stats` File statistics (chars/lines/entropy)<br>`--fson` Output structured FSON summary |
| `secure` | Encryption, decryption, signing, hashing, secure wipe, and integrity check. | `-e` Encrypt<br>`-d` Decrypt<br>`-s` Sign<br>`-v` Verify<br>`-h <alg>` Hash<br>`--scrub` Wipe<br>`--snapshot` Snapshot |
| `storage` | Deduplicate, catalog, snapshot, organize, and prune files. | `--dedupe` Duplicates<br>`--catalog` Catalog<br>`--index` Index<br>`--snapshot` Snapshot<br>`--prune` Prune |

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
| `shark summary --auto --keywords --stats article.md` | Generate an automatic structured summary with keywords and statistics for a document. |
| `shark secure -e -h sha256 secrets.txt` | Encrypt a file and generate a SHA-256 hash for integrity verification. |
| `shark storage --dedupe --catalog ~/Documents/` | Deduplicate files and create a catalog of your Documents directory. |

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
