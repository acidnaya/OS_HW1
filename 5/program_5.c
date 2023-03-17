#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <signal.h>

pid_t child_pid = -1;
const int DEBUG = 0;

const int BUFFER_SIZE = 5000;
char *first = "first.fifo";
char *second = "second.fifo";

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

void read_(int fd, char buffer[]) {
    int size = read(fd, buffer, BUFFER_SIZE);
    if (size < 0){
        printf("Error while reading string from pipe\n");
        exit(-1);
    }
    buffer[size] = '\0';
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

int open_file_R(char file_name[]) {
    int fd;
    if((fd = open(file_name, O_RDONLY, 0666)) < 0) {
        printf("Error while opening file %s for reading\n", file_name);
        kill(child_pid, SIGKILL); // ну
        exit(-1);
    }
    return fd;
}

int open_file_W(char file_name[]) {
    int fd;
    if((fd = open(file_name, O_WRONLY | O_CREAT, 0666)) < 0) {
        printf("Error while opening file %s for writing\n", file_name);
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
    int fd1, fd2;

    mknod(first, S_IFIFO | 0666, 0);
    mknod(second, S_IFIFO | 0666, 0);

    print_pids(0);
    child_pid = fork_();

    if (child_pid == 0) {
        print_pids(1);
        int fd = open_file_R(argv[1]);
        read(fd, buffer, BUFFER_SIZE);
        close_file(fd);

        fd1 = open_file_W(first);
        write_(fd1, buffer);
        close_file(fd1);
    } else if (fork_() == 0) {
        print_pids(2);
        fd1 = open_file_R(first);
        read_(fd1, buffer);
        close_file(fd1);
        int digits = count_digits(buffer);
        char  ans[10] = {0};
        ft_itoa(digits, ans);
        fd2 = open_file_W(second);
        write_(fd2, ans);
        close_file(fd2);
    } else if (fork_() == 0) {
        print_pids(3);
        fd2 = open_file_R(second);
        read_(fd2, buffer);
        close_file(fd2);
        int fd = open_file_W(argv[2]);
        printf("Result %s is written in file %s\n", buffer, argv[2]);
        write_(fd, buffer);
        close_file(fd);
    }

    while(wait(NULL) > 0);
    return 0;
}