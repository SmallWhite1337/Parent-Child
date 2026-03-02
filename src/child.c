/* child.c

    Лабораторная работа №2.

    Дочерний процесс:
    - выводит своё имя (argv[0])
    - выводит PID и PPID
    - выводит среду выполнения
 
    В зависимости от аргумента:
    "+"  — вывод через extern environ
    "*"  — вывод через envp (3-й параметр main)

    Выполнил: Родкевич Н.А.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

static void print_environment_environ(void) {  //  Вывод среды через глобальную переменную environ
    for(int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

static void print_environment_envp(char *envp[]) {  //  Вывод среды через третий параметр main()
    for(int i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s [+ | *]\n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("Process name: %s\n", argv[0]);
    printf("PID: %ld\n", (long)getpid());
    printf("PPID: %ld\n", (long)getppid());
    printf("Environment:\n");
    if(strcmp(argv[1], "+") == 0) {
        print_environment_environ();
    } else if (strcmp(argv[1], "*") == 0) {
        print_environment_envp(envp);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}