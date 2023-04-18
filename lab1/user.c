#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char* proc_file = "/proc/var1";
    int fd = open(proc_file, O_RDWR);
    if (fd == -1)
    {
        perror("proc_file already opened\n");
        return 2;
    }
    size_t size = -1;
    read(fd, &size, sizeof(size));
    if (size != -1) {
        printf("Characters: %zu\n", size);
    }
    return 1;
}