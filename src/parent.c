/* parent.c

    Лабораторная работа №2.
    
    Родительский процесс:
    - выводит и сортирует собственную среду
    - формирует сокращённую среду из файла env
    - по символам '+', '*', 'q' управляет дочерними процессами
 
    Выполнил: Родкевич Н.А.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

static int cmp_strings(const void* a, const void* b) {  //  Функция сравнения строк для qsort()
    const char *s1 = *(const char * const *)a;
    const char *s2 = *(const char * const *)b;
    return strcmp(s1, s2);
}

static void print_and_sort_environment(void) {  //  Копирует, сортирует и выводит окружение процесса
    int count = 0;
    while(environ[count] != NULL) {
        count++;
    }
    char **env_copy = malloc(count * sizeof(char*));
    if(!env_copy) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < count; i++) {
        env_copy[i] = environ[i];
    }
    //env_copy[count] = NULL;
    setenv("LC_COLLATE", "C", 1);
    qsort(env_copy, count, sizeof(char *), cmp_strings);
    for(int i = 0; i < count; i++) {
        printf("%s\n", env_copy[i]);
    }
    free(env_copy);
}

static void trim_newline(char *s) {  //  Удаляет символ '\n' в конце строки
    int len = strlen(s);
    if(len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

static char **build_reduced_environment(const char *filename) {  // Читает файл env и формирует массив окружения для передачи в execve()
    FILE *file = fopen(filename, "r");
    if(!file) {
        perror("fopen env");
        exit(EXIT_FAILURE);
    }
    char **new_env = NULL;
    int size = 0;
    int capacity = 0;
    char buffer[256];
    while(fgets(buffer, sizeof(buffer), file)) {
        trim_newline(buffer);
        if(buffer[0] == '\0') {
            continue;
        }
        char *value = getenv(buffer);
        if(!value) {
            continue;
        }
        int needed = strlen(buffer) + strlen(value) + 2;
        char *entry = malloc(needed);
        if(!entry) {
            perror("malloc");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        snprintf(entry, needed, "%s=%s", buffer, value);
        if (size + 1 >= capacity) {
            capacity = capacity ? capacity * 2 : 8;
            char **tmp = realloc(new_env, capacity * sizeof(char *));
            if (!tmp) {
                perror("realloc");
                fclose(file);
                //free_environment(new_env);
                exit(EXIT_FAILURE);
            }
            new_env = tmp;
        }
        new_env[size++] = entry;
    }
    fclose(file);
    if(!new_env) {
        new_env = malloc(sizeof(char *));
        if(!new_env) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        size = 0;
    }
    new_env[size] = NULL;
    return new_env;
}

static void free_environment(char **envp) {  //  Освобождает память сокращённой среды
    if(!envp) {
        return;
    }
    for(int i = 0; envp[i] != NULL; i++) {
        free(envp[i]);
    }
    free(envp);
}

int main(void) {
    print_and_sort_environment();
    char **reduced_env = build_reduced_environment("env");
    char *child_path = getenv("CHILD_PATH");
    if(!child_path) {
        child_path = ".";
    }
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/child", child_path);
    unsigned counter = 0;
    int ch;
    while((ch = getchar()) != EOF) {
        if(ch == '\n') {
            continue;
        }
        if(ch == 'q') {
            break;
        }
        if(ch == '+' || ch == '*') {
            pid_t pid = fork();
            if(pid < 0) {
                perror("fork");
                continue;
            }
            if(pid == 0) {
                char name[32];
                snprintf(name, sizeof(name), "child_%02u", counter % 100);
                char *argv_child[3];
                argv_child[0] = name;
                argv_child[1] = (ch == '+') ? "+" : "*";
                argv_child[2] = NULL;
                execve(full_path, argv_child, reduced_env);
                perror("execve");
                exit(EXIT_FAILURE);
            } else {
                wait(NULL);
                counter++;
            }
        }
    }
    free_environment(reduced_env);
    return 0;
}