# Tiger Compiler

Tiger compiler is a compiler for the tiger language which is described in the [book](https://www.cs.princeton.edu/~appel/modern/c/). This project is an attempt to implement tiger compiler in C language.

![book](./book.jpg)

## Build

The skeleton code from tiger site uses makefile to build the application, however, makefile is complex and difficult for me. I rather chooses (Premake)[https://github.com/premake/premake-core] to build the project.

By using Premake, it is easy to generate and build the project:

1. Make sure your OS is windows 10
2. Make sure your visual studio 2019 can compile and build C program
3. Execute `GenerateProjectFiles.bat` to generate visual studio solution files
4. Open solution and build
