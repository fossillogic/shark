# **Shark Tool**

Shark is a powerful, all-in-one command-line tool designed to simplify and centralize file tasks for system administrators, developers, and terminal power users.

Instead of juggling a suite of separate utilities for copying, moving, deleting, listing, and searching files, Shark unifies these capabilities into a single, modern interface. Whether you’re managing files, searching contents, adjusting permissions, comparing directories, displaying disk usage, or handling archives—Shark makes it faster, cleaner, and more intuitive.

With consistent syntax, powerful subcommands, and rich flag support, Shark streamlines your workflow and helps you get more done with fewer keystrokes. Say goodbye to command-line clutter—Shark does it all in one bite.

### Sub-flags

| **Flag**            | **Description**                                             |
|---------------------|-------------------------------------------------------------|
| `shark --version`   | Display the current version of the Shark tool.              |
| `shark --help`      | Display help information for the Shark tool.                |
| `shark --name`      | Display app name.                                           |
| `shark --verbose`   | Enable verbose output for all commands.                     |
| `shark --color`     | Set color output mode: `enable`, `disable`, or `auto`.      |
| `shark clear`       | Clear terminal or console output.                           |

### Commands

| **Command**        | **Flags**                                                                 | **Description**                                                                 |
|--------------------|---------------------------------------------------------------------------|---------------------------------------------------------------------------------|
| `shark create`     | `--type=<file|dir>`, `--permissions=<rwx>`                                | Create new files or directories with specific type and permissions.            |
| `shark delete`     | `--recursive`, `--force`, `--trash`                                       | Delete files or directories. Optionally move to trash instead of hard delete.  |
| `shark move`       | `--force`, `--backup`, `--overwrite`                                      | Move or rename files and directories with overwrite and backup options.        |
| `shark rename`     | `--force`, `--backup`                                                     | Rename files or directories with conflict resolution.                          |
| `shark copy`       | `--recursive`, `--preserve`, `--symlinks`, `--hard`                       | Copy files or directories with control over attributes and link types.         |
| `shark list`       | `--what=<files|devices|modules|hardware>`, `--sort`, `--format`           | List system information. Defaults to file listing. Supports devices, kernel modules, and hardware info. |
| `shark show`       | `--lines=<n>`, `--offset=<n>`, `--tail`, `--head`                         | Show file contents with pagination and range options.                         |
| `shark find`       | `--name=<pattern>`, `--size=<gt|lt>:<value>`, `--type`                    | Find items by name, size, or type.                                             |
| `shark where`      | `--name=<pattern>`, `--path=<regex>`                                      | Locate files or directories by name or full path pattern.                      |
| `shark search`     | `--pattern=<regex>`, `--ignore-case`, `--whole-word`                      | Search inside files with advanced pattern matching.                            |
| `shark backup`     | `--destination=<path>`, `--compress`, `--encrypt`                         | Backup files or directories with optional compression and encryption.          |
| `shark size`       | `--human-readable`, `--total`, `--summarize`                              | Display size of items with formatting and summary options.                     |
| `shark disk`       | `--all`, `--free`, `--used`, `--inodes`                                   | Show disk statistics including inode usage.                                    |
| `shark tree`       | `--depth=<n>`, `--all`, `--dirs-only`, `--files-only`                     | Print directory tree with control over depth and content.                      |
| `shark compare`    | `--ignore-case`, `--binary`, `--diff`, `--hash`                           | Compare files or directories using various strategies.                         |
| `shark info`       | `--details`, `--type`, `--stat`, `--checksum`                             | Display metadata and statistics about a file or directory.                     |
| `shark clean`      | `--temp`, `--cache`, `--logs`, `--dry-run`                                | Clean up generated or temporary files, with optional preview.                  |
| `shark file`       | `--create`, `--modify`, `--delete`, `--split=<lines|bytes>`, `--join=<file1,file2,...>`, `--output=<file>` | Perform basic file operations. Includes splitting and joining files.            |
| `shark ask`        | `--exists=<path>`, `--not-exist`, `--type=<file|dir>`                     | Check for file/directory existence or type.                                    |
| `shark change`     | `--target=<path>`, `--value=<new>`, `--owner`, `--mode`                   | Modify file properties like permissions or ownership.                          |
| `shark diff`       | `--unified`, `--side-by-side`, `--ignore-case`, `--context=<n>`           | Show textual differences between files. Supports unified and side-by-side views. |
| `shark archive`    | `--create`, `--extract`, `--format=zip|tar|gz`, `--output=<file>`         | Create or extract archive files in supported formats.                          |

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

For issues, questions, or feedback, open an issue on the [GitHub repository](https://github.com/fossillogic/shark/issues).

## **License**

This project is licensed under the [Mozilla Public License](LICENSE).
