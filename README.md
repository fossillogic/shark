# **Shark Tool**

**Shark** is a powerful, all-in-one command-line tool designed to simplify and centralize file operations. Instead of juggling multiple utilities for tasks like copying, moving, deleting, listing, and searching for files, **Shark** consolidates them into a single, easy-to-use tool. With modern, intuitive commands, **Shark** streamlines your workflow, allowing you to manage files, search file contents, compare directories, adjust permissions, display disk usage, and much more—all with one tool. **Shark** handles it with speed and efficiency. Say goodbye to the clutter of multiple command-line tools—**Shark** does it all in one bite.

## 🦈 **shark** CLI Command Reference

| **Command**        | **Flags**                          | **Description**                                                                 |
|--------------------|------------------------------------|---------------------------------------------------------------------------------|
| `shark --version`  | None                               | Display the current version of the Shark tool.                                 |
| `shark --help`     | None                               | Display help information for the Shark tool.                                   |
| `shark --name`     | None                               | Display app name.                                                              |
| `shark --verbose`  | None                               | Enable verbose output.                                                         |
| `shark create`     | `--type`, `--permissions`          | Create new files or directories.                                               |
| `shark delete`     | `--recursive`, `--force`           | Delete files or directories.                                                   |
| `shark move`       | `--force`, `--backup`              | Move or rename files and directories.                                          |
| `shark rename`     | `--force`, `--backup`              | Rename files or directories.                                                   |
| `shark copy`       | `--recursive`, `--preserve`        | Copy files or directories.                                                     |
| `shark list`       | `--sort`, `--format`               | List files and directories. Customize output using sort and format.            |
| `shark show`       | `--lines`, `--offset`              | Display contents of a file, with options for viewing parts of it.              |
| `shark find`       | `--name`, `--size`, `--type`       | Find files or directories matching specific criteria (name, size, type, etc.). |
| `shark where`      | `--name`, `--path`                 | Locate files or directories by name or path.                                   |
| `shark search`     | `--pattern`, `--ignore-case`       | Search file contents for patterns.                                             |
| `shark backup`     | `--destination`, `--compress`      | Create backups of files or directories with optional compression.              |
| `shark size`       | `--human-readable`, `--total`      | Display size of files or directories.                                          |
| `shark disk`       | `--all`, `--free`                  | Display disk usage and free space.                                             |
| `shark tree`       | `--depth`, `--all`                 | Display directory structure as a tree.                                         |
| `shark compare`    | `--ignore-case`, `--binary`        | Compare two files or directories.                                              |
| `shark color`      | `enable`, `disable`, `auto`        | Set color output mode.                                                         |
| `shark info`       | `--details`, `--type`              | Get detailed information about a file or directory.                            |
| `shark clear`      | None                               | Clear output from console.                                                     |
| `shark file`       | `--create`, `--modify`, `--delete` | Perform basic file operations in a simplified format.                          |
| `shark ask`        | `--exists`, `--type`, `--perm`     | Query whether a file exists, does not exist, or is of a specified type.        |
| `shark change`     | `--target`, `--value`              | Modify properties or metadata of a file or directory.                          |

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
