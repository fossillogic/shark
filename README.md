# **Shark File Tool**

**Shark** is a powerful, all-in-one command-line tool designed to simplify and centralize file operations. Instead of juggling multiple utilities for tasks like copying, moving, deleting, and searching for files, **Shark** consolidates them into a single, easy-to-use tool. With modern, intuitive commands, **Shark** streamlines your workflow, allowing you to manage files, manipulate archives, adjust permissions, synchronize directories, and much more—all with a single tool. Whether you're working with local files or need to push/pull files remotely, **Shark** handles it with speed and efficiency. Say goodbye to the clutter of multiple command tools—**Shark** does it all in one bite.

## 🦈 **shark** CLI Command Reference

| **Command**       | **Sub Flags**                                                                                      | **Description**                                                     |
|-------------------|---------------------------------------------------------------------------------------------------|---------------------------------------------------------------------|
| `shark move`      | `--rename`, `--force`, `--dry-run`, `--backup`                                                      | Move or rename files and directories. `--dry-run` simulates the move. |
| `shark copy`      | `--recursive`, `--backup`, `--no-overwrite`, `--force`                                               | Copy files or directories. `--recursive` copies directories.        |
| `shark delete`    | `--recursive`, `--force`, `--dry-run`, `--backup`                                                   | Delete files or directories. `--recursive` deletes subdirectories. |
| `shark list`      | `--long`, `--all`, `--human-readable`, `--sort=NAME|SIZE|DATE`                                       | List files and directories. Customize output using sort and format. |
| `shark show`      | `--lines=N`, `--head`, `--tail`, `--json`, `--yaml`                                                 | Display contents of a file, with options for viewing parts of it.   |
| `shark meta`      | `--size`, `--permissions`, `--owner`, `--timestamps`                                                | Show metadata of a file or directory (like `stat`).                 |
| `shark compress`  | `--gzip`, `--tar`, `--zip`, `--bzip2`, `--output=FILE`                                              | Compress files or directories into a specific format.               |
| `shark extract`   | `--tar`, `--zip`, `--gzip`, `--bzip2`, `--output=DIR`                                               | Extract files from archives. Specify the format or output directory.|
| `shark chmod`     | `--recursive`, `--permissions=MODE`                                                                 | Change file permissions. `--recursive` applies changes to subdirectories. |
| `shark ownership` | `--user=USER`, `--group=GROUP`, `--recursive`                                                        | Change file ownership (user/group).                                |
| `shark find`      | `--name=PATTERN`, `--size=SIZE`, `--type=TYPE`, `--modified=DATE`, `--max-depth=N`                   | Find files matching specific criteria (name, size, type, etc.).     |
| `shark search`    | `--pattern=PATTERN`, `--ignore-case`, `--files`, `--invert-match`, `--recursive`                     | Search file contents for patterns.                                  |
| `shark size`      | `--human-readable`, `--summarize`, `--depth=N`, `--max-depth=N`                                     | Display size of files or directories.                               |
| `shark disk`      | `--total`, `--human-readable`, `--inodes`                                                            | Display disk usage and free space.                                  |
| `shark tree`      | `--depth=N`, `--file-limit=N`, `--long`                                                              | Display directory structure as a tree, customizable with depth.     |
| `shark compare`   | `--binary`, `--ignore-whitespace`, `--recursive`, `--side-by-side`                                  | Compare two files or directories.                                   |
| `shark sync`      | `--dry-run`, `--recursive`, `--delete`, `--ignore-existing`                                          | Synchronize files between directories.                              |
| `shark update`    | `--timestamp=TIME`, `--create`, `--modify`, `--dry-run`                                              | Update file timestamps, or create files if they don’t exist.        |
| `shark open`      | `--editor=EDITOR`, `--viewer=VIEWER`                                                                | Open file with a specified editor or viewer.                         |
| `shark edit`      | `--editor=EDITOR`, `--line=N`                                                                       | Open file in a default or specified editor for editing.              |
| `shark push`      | `--remote=HOST`, `--user=USER`, `--port=PORT`, `--recursive`                                         | Push files to a remote location via SCP or another protocol.        |
| `shark pull`      | `--remote=HOST`, `--user=USER`, `--port=PORT`, `--recursive`                                         | Pull files from a remote location via SCP or another protocol.      |

### Notes:
- **--dry-run**: Simulate the command without actually performing any actions. Useful for previewing.
- **--recursive**: Apply the operation to directories and subdirectories.
- **--backup**: Create backup files before making changes (e.g., when deleting or overwriting).
- **--human-readable**: Show sizes in a human-readable format (e.g., MB, GB).
- **--ignore-case**: Make searches case-insensitive.
- **--editor/--viewer**: Specify custom editors or viewers for file operations.

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
    shark
    ```

## **Contributing**

Interested in contributing? Please open pull requests or create issues on the [GitHub repository](https://github.com/fossillogic/shark).

## **Feedback and Support**

For issues, questions, or feedback, open an issue on the [GitHub repository](https://github.com/fossillogic/fossil-app/shark).

## **License**

This project is licensed under the [Mozilla Public License](LICENSE).
