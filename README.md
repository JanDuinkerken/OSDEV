# quackOS
OS developed for the OSDEV course from UNED-2024.

## Dependencies
The kernel must be compiled using a **UNIX** system (Linux or MacOS) or **WSL** (Windows Subsystem for Linux), as compiling it directly on Windows will not work.
The following packages are necessary for compiling the project:
- **gcc**: The GNU Compiler Collection.
- **ld**: The GNU Linker.
- **tree**: The utility to explore folders.
- **qemu**: A virtual machine emulator.
- **gdb**: The GNU Debugger.
- **gnumakefile**: A makefile for building projects.
- **nasm**: The Netwide Assembler.
- **git**: A version control system.
- **sgdisk**: A utility for manipulating GPT partitions.
- **fdisk**: A command-line disk partitioning tool.
- **mkfs.fat**: A utility for creating FAT file systems.
- **mkfs.ext2**: A utility for creating ext2 file systems.
- **partprobe**: A tool for informing the OS kernel of partition table changes.

>  [!NOTE]
>  Keep in mind the names of the packages can change depending on your OS or distribution.

## How to compile and run
If you just want to quickly compile and run the kernel you can execute the `run.sh` script found inside the kernel directory as follows:
```bash
./run.sh
```
For more compile options you can use `./run.sh -h` to get a list of functionalities.

If you want to compile and run the kernel step by step, run the following commands from the kernel folder:
```bash
make clean
make cleansetup
make gpt
```

## How to debug
> [!IMPORTANT]
> Depending on your OS or distribution de `CMDNEWSCREEN` value found inside the GNUmakefile must be changed to issue a command that opens a new shell and executes a command.
> As an example, for Arch Linux using the KDE environment and the Konsole terminal emulator the value would be as follows:
> ```make
> CMDNEWSCREEN := konsole -e
> ```

To debug the kernel you can just simply run the following commands from the kernel folder:
```bash
./run.sh --debug
```

Or if you want to run all the steps by hand you can execute the following commands inside the kernel directory:
```bash
make clean
make cleansetup
make debugpt
```
