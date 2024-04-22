# Quik Start

### before start

* Ensure that the computer's operating system is **Linux**
* Ensure that the computer is installed with **g++** and **Cmake** tools

### how to build

firstly, cd the repository: ~/monkey_language/
and than type following commands in terminal:

```
mkdir build
cd build
cmake ..
make
```

It will complete compilation in a few minutes, and generate an executable program "monkey"

### how to run

```
cd build
# 1. compile and run in one command
./monkey run
# 2. open command line mod
./monkey cmd
```

if you choose execute the command #1, you should write down the monkey_language program in a text file named "input.txt", which is under "build" directory. (if "input.txt" doesn't exist, you should create it firstly.)

if you choose execute the other #2, you should type monkey_language command in terminal, and each command should be ended with ";".

