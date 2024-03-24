# quackOS
OS developed for the OSDEV course from UNED-2024.

## How to compile and run
If you just want to quickly compile and run the kernel you can execute the `run.sh` script as follows:
```
cd kernel
./run.sh
```
For more compile options you can use `./run.sh -h` to get a list of functionalities.

If you want to compile and run the kernel step by step, run the following commands from the kernel folder:
```
make clean
make cleansetup
make gpt
```

## How to debug
To debug the kernel you can just simply run:
```
./run.sh --debug
```

Or if you want to run all the steps by hand you can use the following commands:
```
make clean
make cleansetup
make debugpt
```
