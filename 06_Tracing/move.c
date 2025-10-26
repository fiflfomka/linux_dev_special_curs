#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("ARGV 1 ERROR: expceted: ./move FILE_IN FILE_OUT\n");
        return 1;
    }

    if (access(argv[1], R_OK) != 0) {
        printf("ACCESS 1 ERROR: input file is not exist\n");
        return 2;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf("OPEN 1 ERROR: can't open in file\n");
        return 3;
    }

    long filesize = lseek(fd, 0, SEEK_END);
    if (filesize < 0) {
        printf("LSEEK 1 ERROR: can't jump on the end of file\n");
        return 4;
    }

    long res = lseek(fd, 0, SEEK_SET);
    if (res < 0) {
        printf("LSEEK 2 ERROR: can't jump on the start of file\n");
        return 5;
    }

    void* buf = malloc(filesize);
    long read_size = read(fd, buf, filesize);
    if (read_size < 0) {
        printf("READ 1 ERROR: can't read from file\n");
        return 6;
    }
    if (read_size != filesize) {
        printf("READ 2 ERROR: incorrect size readed\n");
        return 7;
    }

    if (close(fd) != 0) {
        printf("CLOSE 1 ERROR: can't close input file\n");
        return 8;
    }

    fd = -1;
    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0664);
    if (fd2 < 0) {
        printf("OPEN 2 ERROR: can't open out file\n");
        return 9;
    }

    long write_size = write(fd2, buf, filesize);
    if (write_size < 0) {
        printf("WRITE 1 ERROR: can't write to out file\n");
        return 10;
    }
    if (write_size != filesize) {
        printf("WRITE 2 ERROR: incorrect size of write\n");
        return 11;
    }

    if (unlink(argv[1]) != 0) {
        printf("UNLINK 1 ERROR: can't remove in file\n");
        return 12;
    }

    return 0;
}
