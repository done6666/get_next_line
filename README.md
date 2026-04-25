_This project has been created as part of the 42 curriculum by opektas._

# get_next_line

## Description

`get_next_line` is a C function that reads and returns one line at a time from a given file descriptor. Successive calls to the function allow reading an entire file line by line, retaining state between calls through the use of a **static variable**. The function works correctly whether reading from a regular file or from standard input (`fd = 0`), and handles any `BUFFER_SIZE` value set at compile time.

The returned line includes the terminating `\n` character, except when the end of the file is reached and the file does not end with a newline.

### File structure

| File                    | Purpose                                                           |
| ----------------------- | ----------------------------------------------------------------- |
| `get_next_line.c`       | Core function and three static helper functions                   |
| `get_next_line_utils.c` | String utility functions (`ft_strlen`, `ft_strjoin`, `ft_strchr`) |
| `get_next_line.h`       | Header: prototype, includes, and `BUFFER_SIZE` default            |

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

The function maintains a single `static char *backup` variable inside `get_next_line`. This pointer persists across calls and holds any data that was read from the file descriptor but not yet returned as a complete line. The algorithm is divided into three phases, each handled by a dedicated static helper function:

```
get_next_line(fd)
    │
    ├─► ft_read_to_backup(fd, backup)   — accumulate data until '\n' or EOF
    │
    ├─► ft_get_line(backup)             — extract and return the first line
    │
    └─► ft_new_backup(backup)           — trim the returned line from backup
```

### Phase 1 — `ft_read_to_backup`

Reads from `fd` in chunks of `BUFFER_SIZE` bytes using `read()`. After each read, the chunk is appended to `backup` via `ft_strjoin` (which also frees the old `backup`). Reading continues until either a `\n` is found inside `backup` or `read()` returns 0 (EOF). If `read()` returns -1, both `buffer` and `backup` are freed and `NULL` is returned.

This approach ensures **minimal reads per call**: as soon as a newline is detected in the accumulated buffer, no further `read()` calls are made.

### Phase 2 — `ft_get_line`

Scans `backup` from the beginning up to and including the first `\n` (or to the end if no `\n` exists). Allocates a new string of exactly the right size and copies the line into it. This is the string returned to the caller.

### Phase 3 — `ft_new_backup`

After the line has been extracted, the remaining content in `backup` (everything after the `\n`) must be preserved for the next call. `ft_new_backup` advances past the `\n`, copies the tail into a newly allocated string, frees the old `backup`, and returns the new one. If there is nothing left after the `\n`, it frees `backup` and returns `NULL`, so the next call starts clean.

### Justification

The static-buffer approach is the canonical solution for `get_next_line` because:

- **It avoids `lseek`**: the file descriptor's position is never rewound. Data read beyond the current line boundary is safely held in `backup` rather than discarded.
- **It works on non-seekable descriptors**: pipes, standard input, and sockets cannot use `lseek`. This design handles all of them transparently.
- **Memory efficiency**: only one `BUFFER_SIZE`-sized chunk is allocated per read cycle. There is no full file preload.
- **Correctness across buffer sizes**: because the logic does not depend on the buffer aligning with newlines, the function behaves identically for `BUFFER_SIZE=1`, `BUFFER_SIZE=42`, or `BUFFER_SIZE=10000000`.

The main trade-off of using a single static pointer (as opposed to a static array indexed by `fd`) is that this implementation handles **one file descriptor at a time**. Interleaving reads from different file descriptors is undefined behavior in this mandatory version. The bonus part addresses this with an array-indexed approach.

---

## Resources

### References

- `man 2 read` — POSIX read system call
- `man 3 malloc` / `man 3 free` — heap memory management
- [Static variables in C — cppreference.com](https://en.cppreference.com/w/c/language/storage_duration)
- [File descriptors — The Linux Programming Interface, Kerrisk](https://man7.org/tlpi/)
- 42 Norm v4 — coding standard enforced throughout the project

### AI usage

AI was used to **generate this README file** based on the completed source code and the subject's documentation requirements. The implementation itself — the algorithm design, the three-phase structure, and all C source files — was written independently beforehand. AI was not consulted during the coding phase.
