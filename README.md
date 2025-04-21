# **Shark File Tool**

**Shark** is a powerful, all-in-one command-line tool designed to simplify and centralize file operations. Instead of juggling multiple utilities for tasks like copying, moving, deleting, and searching for files, **Shark** consolidates them into a single, easy-to-use tool. With modern, intuitive commands, **Shark** streamlines your workflow, allowing you to manage files, manipulate archives, adjust permissions, synchronize directories, and much more—all with a single tool. Whether you're working with local files or need to push/pull files remotely, **Shark** handles it with speed and efficiency. Say goodbye to the clutter of multiple command tools—**Shark** does it all in one bite.

## 🦈 **shark** CLI Command Reference

| **Command**       | **Description**                                                     | **Subflags**                                                                 |
|-------------------|---------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `shark --version` | Display the current version of the Shark tool.                     | None                                                                        |
| `shark --help`    | Display help information for the Shark tool.                       | None                                                                        |
| `shark move`      | Move or rename files and directories.                              | `--force`, `--interactive`, `--backup`                                      |
| `shark copy`      | Copy files or directories.                                         | `--recursive`, `--preserve`, `--verbose`                                    |
| `shark delete`    | Delete files or directories.                                       | `--force`, `--recursive`, `--dry-run`                                       |
| `shark list`      | List files and directories. Customize output using sort and format.| `--sort`, `--format`, `--human-readable`                                    |
| `shark show`      | Display contents of a file, with options for viewing parts of it.  | `--lines`, `--bytes`, `--encoding`                                          |
| `shark find`      | Find files matching specific criteria (name, size, type, etc.).    | `--name`, `--size`, `--type`, `--ignore-case`                               |
| `shark search`    | Search file contents for patterns.                                 | `--pattern`, `--ignore-case`, `--line-numbers`                              |
| `shark size`      | Display size of files or directories.                              | `--human-readable`, `--total`, `--sort`                                     |
| `shark disk`      | Display disk usage and free space.                                 | `--all`, `--human-readable`, `--threshold`                                  |
| `shark tree`      | Display directory structure as a tree.                             | `--depth`, `--all`, `--human-readable`                                      |
| `shark compare`   | Compare two files or directories.                                  | `--recursive`, `--ignore-case`, `--detailed`                                |
| `shark sync`      | Synchronize files between directories.                             | `--delete`, `--dry-run`, `--verbose`                                        |
| `shark update`    | Update file timestamps, or create files if they don’t exist.       | `--access-time`, `--modification-time`, `--create`                          |
| `shark open`      | Open file with a specified editor or viewer.                       | `--editor`, `--viewer`, `--read-only`                                       |
| `shark edit`      | Open file in a default or specified editor for editing.            | `--editor`, `--backup`, `--encoding`                                        |
| `shark push`      | Push files to a remote location via SCP or another protocol.       | `--protocol`, `--port`, `--verbose`                                         |
| `shark pull`      | Pull files from a remote location via SCP or another protocol.     | `--protocol`, `--port`, `--verbose`                                         |

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
