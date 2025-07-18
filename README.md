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
| `shark --clear`     | Clear terminal or console output.                           |

### Commands

| **Command**        | **Flags**                                                    | **Description**                                                                 |
|--------------------|--------------------------------------------------------------|---------------------------------------------------------------------------------|
| `shark create`     | `--type=<file/dir>`, `--name=<file_name>`                    | Create new files or directories with specified type and name.                  |
| `shark delete`     | `--force`                                                    | Remove files or directories. Optionally move them to trash instead of permanent deletion. |
| `shark move`       | `--force`, `--backup`                                        | Move or rename files and directories with options to overwrite or create backups. |
| `shark rename`     | `--force`, `--backup`                                        | Rename files or directories with conflict handling options.                    |
| `shark copy`       | `--preserve`, `--link=<hard/sym>`                            | Duplicate files or directories while preserving attributes or creating links.  |
| `shark list`       | `--as=<list/tree>`, `--sort=<disc/asc>`                      | Display files or directories in a list or tree format with sorting options.    |
| `shark show`       | `--lines=<n>`, `--offset=<n>`                                | View file contents with support for pagination and line offsets.               |
| `shark find`       | `--name=<pattern>`, `--type`                                 | Search for files or directories by name, type, or other criteria.              |
| `shark size`       | `--human-readable`                                           | Show the size of files or directories with optional human-readable formatting. |
| `shark compare`    | `--binary`, `--diff`, `--hash`                               | Compare files or directories using binary, diff, or hash methods.              |
| `shark ask`        | `--exists=<path>`, `--not-exist=<path>`, `--type=<file/dir>` | Verify the existence or type of files or directories.                          |
| `shark change`     | `--target=<path>`, `--value=<new>`, `--owner`, `--mode`      | Update file properties such as permissions, ownership, or mode.                |

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
