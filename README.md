# simpleShell
simpleShell is a minimal shell made in C for learning (me learning).
This is only for Windows, if you want to run on linux you can fork this repo, modify some code and then compile.

```
simpleShell version 3.2
Type 'help' for list of available commands.
Welcome back, root.

root@simple-shell:/ > help
Available commands:
   Core:
       help       - Show this message.
       echo <msg> - Print message.
       clear      - Clear screen.
       ver        - Show version.
       about      - Info about shell.
       ezfetch    - Show shell info.
       exit       - Exit shell.

   MicroVFS:
       format      - Format virtual disk (vdisk.bin).
       mkdir <dir> - Create directory.
       mkfile <f>  - Create file.
       micro <f>   - Edit file.
       cat <f>     - Read file.
       rm <f>      - Delete file.
       cd <dir>    - Change directory.
       ls          - List files.
```

I added a filesystem called MicroVFS which you can see on the help message above.
It supports one-path parsing (e.g. test/test.txt, but not first/second/third/fourth.txt).
I'm lazy to explain, just explore the code.
