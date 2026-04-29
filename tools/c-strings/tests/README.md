# Tests for C Strings Lesson

This folder contains a lightweight, dependency-free C test harness for the lesson string library.

## Why this style?

- No external frameworks required
- Easy for students to read and modify
- Works on any basic C toolchain

## Run tests

From the module root:

```bash
cd /home/randy/Workspace/REPOS/educational/tools/c-strings
make test
```

## Coverage snapshot

- create/destroy basics
- get/set char bounds behavior
- reverse + case transforms
- concat/slice/patch/splice
- find/contains/compare/equals
- StringBuilder append/clear/toString
