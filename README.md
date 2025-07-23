# **Shark File Tool**
**Shark** is the all-in-one command-line tool for admins and power users, designed to simplify and centralize file operations. Instead of juggling multiple utilities for tasks like copying, moving, deleting, listing, and searching for files, **Shark** consolidates them into a single, easy-to-use solution. With modern, intuitive commands, **Shark** streamlines your workflow—manage files, search file contents, compare directories, adjust permissions, display disk usage, and much more, all with speed and efficiency. Say goodbye to the clutter of multiple command-line tools—**Shark** does it all in one bite.

## 🦈 **shark** CLI Command Reference

| **Flags**         | **Description**                                                    |
|-------------------|--------------------------------------------------------------------|
| `shark --version` | Display the current version of the Shark tool.                     |
| `shark --help`    | Display help information for the Shark tool.                       |
| `shark --name`    | Display app name.                                                  |
| `shark --verbose` | Enable verbose output.                                             |
| `shark --clear`   | Clear output from the console.                                     |

| **Command**       | **Flags**                                   | **Description**                                                    |
|-------------------|---------------------------------------------|--------------------------------------------------------------------|
| `shark move`      | `--force`, `--interactive`                  | Move or rename files and directories.                              |
| `shark rename`    | `--force`                                   | Rename files or directories.                                       |
| `shark copy`      | `--force`, `--recursive`                    | Copy files or directories.                                         |
| `shark delete`    | `--force`, `--recursive`, `--interactive`   | Delete files or directories.                                       |
| `shark list`      | `--sort`, `--format`, `--all`, `--long`     | List files and directories. Customize output using sort and format.|
| `shark show`      | `--head`, `--tail`, `--lines`               | Display contents of a file, with options for viewing parts of it.  |
| `shark find`      | `--name`, `--size`, `--type`, `--hidden`    | Find files matching specific criteria (name, size, type, etc.).    |
| `shark where`     | `--name`, `--type`                          | Show the location of files or directories matching criteria.       |
| `shark search`    | `--pattern`, `--ignore-case`, `--count`     | Search file contents for patterns.                                 |
| `shark ask`       | `--message`, `--default`, `--timeout`       | Prompt for user input or confirmation before performing actions.   |
| `shark edit`      | `--editor`, `--backup`                      | Edit files directly from the command line.                         |
| `shark size`      | `--human-readable`, `--total`               | Display size of files or directories.                              |
| `shark disk`      | `--all`, `--human-readable`                 | Display disk usage and free space.                                 |
| `shark tree`      | `--depth`, `--all`, `--color`               | Display directory structure as a tree.                             |
| `shark compare`   | `--recursive`, `--brief`, `--ignore-case`   | Compare two files or directories.                                  |
| `shark create`    | `--directory`, `--parents`                  | Create new files or directories.                                   |
| `shark color`     | `enable`, `disable`, `auto`                 | <enable/disable/auto> Set color output mode                        |
| `shark clear`     | `--force`                                   | Clear output from console                                          |

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
