#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char* proc_file = "/proc/var1";
    int fd = open(proc_file, O_RDWR);
    if (fd == -1)
    {
        perror("proc_file is already open\n");
        return 2;
    }
    char buf[2048];
    memset(buf, -1, sizeof(buf));
    read(fd, buf, sizeof(buf));
    size_t len;
    for (len = 0; len <= sizeof(buf); len++ ) {
        if (buf[len] == -1) break;
    }
    char dest[len];
    memcpy(dest, buf, sizeof(dest));
    printf("%s", dest);
    close(fd);
    return 1;
}