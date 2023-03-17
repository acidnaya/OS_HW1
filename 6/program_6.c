#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

const int BUFFER_SIZE = 5000;
const int DEBUG = 0;

int count_digits(char str[]) {
    int i, sum;
    i = 0;
    sum = 0;
    while (str[i] != '\0') {
        if(str[i] <= '9' && str[i] >= '0') {
            sum += str[i] - '0';
        }
        ++i;
    }
    return sum;
}

void write_(int fd, char msg[]) {
    int size = strlen(msg);
    int written = write(fd, msg, size);
    if (size != written){
        printf("Error while writing!\n");
        exit(-1);
    }
}

int read_(int fd, char buffer[]) {
    int size = read(fd, buffer, BUFFER_SIZE);
    if (size < 0){
        printf("Error while reading string from pipe\n");
        exit(-1);
    }
    buffer[size] = '\0';
    return size;
}

void open_pipe(int fd[]) {
    if(pipe(fd) < 0) {
        printf("Error while opening pipe\n");
        exit(-1);
    }
}

int fork_() {
    int result = fork();
    if (result < 0) {
        printf("Error while forking\n");
        exit(-1);
    }
    return result;
}

void close_reading(int fd[]) {
    if (close(fd[0]) < 0){
        printf("Error while closing reading side of pipe\n");
        exit(-1);
    }
}

void close_writing(int fd[]) {
    if (close(fd[1]) < 0){
        printf("Error while closing writing side of pipe\n");
        exit(-1);
    }
}

int open_file(char file_name[]) {
    int fd;
    if((fd = open(file_name, O_RDWR | O_CREAT, 0666)) < 0) {
        printf("Error while opening file %s\n", file_name);
        exit(-1);
    }
    return fd;
}

void close_file(int fd) {
    if(close(fd) < 0) {
        printf("Error while closing file\n");
    }
}

char *ft_itoa(int num, char *str) {
    if(str == NULL) {
        return NULL;
    }
    sprintf(str, "%d", num);
    return str;
}

void print_pids(int block) {
    if (DEBUG) {
        pid_t parent = getppid();
        pid_t child = getpid();

        printf("block: %d, parent: %d, child: %d\n", block, parent, child);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: ./program input.txt output.txt\n");
        return(0);
    }

    char  buffer[BUFFER_SIZE + 1] = {0};
    int fd1[2], fd2[2];

    open_pipe(fd1);
    open_pipe(fd2);

    print_pids(0);
    int fork1;

    fork1 = fork_();
    if (fork1 == 0) {
        print_pids(1);
        int fd = open_file(argv[1]);
        read(fd, buffer, BUFFER_SIZE);
        close_file(fd);
        write_(fd1[1], buffer);
        close_writing(fd1);

        close_writing(fd2);
        read_(fd2[0], buffer);
        close_reading(fd2);
        fd = open_file(argv[2]);
        write_(fd, buffer);
        close_file(fd);
        printf("Result %s is written in file %s\n", buffer, argv[2]);
    } else if (fork_() == 0) {
        print_pids(2);
        close_writing(fd1);
        read_(fd1[0], buffer);
        int digits = count_digits(buffer);
        char  ans[10] = {0};
        ft_itoa(digits, ans);
        close_reading(fd1);
        write_(fd2[1], ans);
        close_writing(fd2);
        exit(0);
    }

    while(wait(NULL) > 0);
    return 0;
}
