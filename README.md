## cplr - the C piler

The C piler *cplr* is a tool for executing C code from the shell and is meant to break the barrier between the shell and the core language of UNIX, which it does by making the compiler a tool like sed, grep and awk.

Its practical purpose is to allow the compilation and execution of short C programs for testing, system information retrieval, live coding and interactive development of C small sequences and programs.

At its most basic, *cplr* just runs C:
```
$ alias c=cplr
$ c 'puts("hello!")'
hello!
$ c 'printf("%d\n", getpagesize())'
4096
$ c -i linux/kvm.h 'printf("%zu\n", sizeof(struct kvm_regs))'
144
```

You can also create ad-hoc executables:
```
$ c -o realpath 'puts(realpath(argv[1],NULL))'
$ ./rp .
/home/user
```

Thanks to pkg-config, build configuration is easy:
```
$ c -P tinfo -i term.h 'setupterm(NULL,1,NULL)' 'printf("%d\n", tigetnum("colors"))'
256
```

Small C libraries can be used directly from source:
```
$ c -s linenoise.c -i linenoise.h 'printf("Line: %s\n", linenoise("% "))'
% hello
Line: hello
```

### How it works

*cplr* itself is a simple **compiler front-end** that does not have any comprehension of the C language or even data types, but this is why it works well because it does not try to do something hard. Instead we follow the UNIX philosophy and do just one thing: act as the **interactive equivalent of 'cc'**.

The current backend of *cplr* is based on **TinyCC**, which gives it a language level of **C99 or better** and the ability to interact with almost any C library on a normal Linux system - including large and complicated ones. You can run Gtk or Python in this tool without having to expect any issues.

### Future possibilities

 * C REPL (experimental at this point)
 * Garbage collection (in progress)
 * Crash handlers
 * Backtrace support
 * Bounds checking
 * Instruction trace
 * Internal disassembler (capstone/udis86/disarm)
 * Support other compilers through dlopen
 * Support launching into gdb/lldb
 * Source code and assembly dumping
 * Editor and pager integration
 * Live process code injection (researching)
 * Live kernel code injection (needs gcc)
 * Support for C++ and ObjC

