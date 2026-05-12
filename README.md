_This project has been created as part of the 42 curriculum by opektas._

# get_next_line

## Description

`get_next_line` is a C function that reads and returns one line at a time from a given file descriptor. Successive calls allow reading an entire file line by line, retaining state between calls through a **static pointer to a memory struct**. The function works correctly whether reading from a regular file or from standard input (`fd = 0`), and handles any `BUFFER_SIZE` value set at compile time.

The returned line includes the terminating `\n` character, except when the end of the file is reached and the file does not end with a newline.

### File structure

| File                    | Purpose                                                                              |
| ----------------------- | ------------------------------------------------------------------------------------ |
| `get_next_line.c`       | Core function and four static helper functions                                       |
| `get_next_line_utils.c` | String utilities: `ft_strlen`, `ft_strjoin`, `ft_strchr`, `ft_strdup`, `ft_substr`  |
| `get_next_line.h`       | Header: struct definition, prototypes, includes, and `BUFFER_SIZE` default           |

---

## Instructions

### Compilation

Compile together with your own source files, optionally specifying a buffer size:

```bash
# With a custom buffer size
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c

# Without the flag (defaults to 42, defined in the header)
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c
```

### Usage example

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int     fd;
    char    *line;

    fd = open("file.txt", O_RDONLY);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

### Function prototype

```c
char *get_next_line(int fd);
```

Returns the next line from `fd`, or `NULL` on EOF or error.

---

## Algorithm

### Overview

All state is stored in a single `static t_memory *memory` pointer inside `get_next_line`. The struct groups the read buffer, the accumulated backup string, the byte count from the last `read()` call, and a helper pointer for newline detection. Each call passes through three phases handled by dedicated static functions:

```
get_next_line(fd)
    │
    ├─► init_memory(&memory)      — allocate and initialise the struct on first call
    │
    ├─► read_to_newline(fd, &memory) — accumulate data until '\n' or EOF
    │
    └─► cut_line(&memory)         — extract the line, trim backup, return to caller
```

### `t_memory` struct

```c
typedef struct s_memory
{
    char    *backup;   // data read but not yet returned
    char    *buffer;   // raw read buffer of BUFFER_SIZE bytes
    int      bytes;    // return value of the last read() call
    char    *newline;  // reused as a temporary pointer during cut_line
}   t_memory;
```

### Phase 1 — `init_memory`

Runs only when `memory` is `NULL` (the very first call, or after a full cleanup). Allocates the struct and the fixed-size `buffer`. Sets `bytes` to `1` so the read loop in the next phase starts normally. If any allocation fails, `free_memory` is called and `0` is returned, causing `get_next_line` to return `NULL`.

### Phase 2 — `read_to_newline`

Reads from `fd` in chunks of `BUFFER_SIZE` bytes. After each `read()`, the chunk is appended to `backup` via `ft_strjoin` (which also frees the old `backup`). The loop continues until either `\n` is found in `backup` or `bytes` reaches `0` (EOF). If `read()` returns `-1`, `free_memory` is called and `0` is returned.

This ensures **minimal reads per call**: reading stops as soon as a newline is detected in the accumulated data.

### Phase 3 — `cut_line`

If `backup` is empty or `NULL`, there is nothing left to return — `free_memory` is called and `NULL` is returned to signal EOF. Otherwise:

1. A line is extracted from `backup` up to and including the first `\n` (or to the end if no `\n` exists) using `ft_substr`.
2. The remainder after the `\n` is duplicated into a new string via `ft_strdup` and becomes the new `backup`.
3. The old `backup` is freed.
4. The extracted line is returned to the caller.

### `free_memory`

Frees `backup`, `buffer`, and the struct itself, then sets the pointer to `NULL`. Called on error, EOF, or failed allocation to ensure no memory is leaked between calls.

---

## Resources

### References

- `man 2 read` — POSIX read system call
- `man 3 malloc` / `man 3 free` — heap memory management
- [Static variables in C — cppreference.com](https://en.cppreference.com/w/c/language/storage_duration)
- [File descriptors — The Linux Programming Interface, Kerrisk](https://man7.org/tlpi/)
- 42 Norm v4 — coding standard enforced throughout the project

### AI usage

AI was used to **generate test cases** for verifying edge-case behaviour (empty files, no trailing newline, large buffer sizes, etc.). The implementation — struct design, algorithm, and all C source files — was written independently.
