#include "lib.h"

int main() {
    int pipe_parent_to_first_child[2];
    int pipe_second_child_to_parent[2];
    if (pipe(pipe_parent_to_first_child) == -1) {
        printf("pipe1 error");
        return -1;
    }
    if (pipe(pipe_second_child_to_parent) == -1) {
        printf("pipe1 error");
        return -1;
    }
    
    pid_t first_child_process_id = fork();
    if (first_child_process_id == -1) {
        printf("first_child fork error");
        return -1;
    }

    if (first_child_process_id == 0) {
        close(pipe_parent_to_first_child[1]);
        close(pipe_second_child_to_parent[0]);

        dup2(pipe_parent_to_first_child[0], STDIN_FILENO);
        dup2(pipe_second_child_to_parent[1], STDOUT_FILENO);

        execl("child1", "child1", NULL);
    } else {
        pid_t second_child_process_id = fork();
        if (first_child_process_id == -1) {
            printf("second_child fork error");
            return -1;
        }

        if (second_child_process_id == 0) {
            close(pipe_parent_to_first_child[0]);
            close(pipe_second_child_to_parent[1]);

            dup2(pipe_parent_to_first_child[1], STDOUT_FILENO);
            dup2(pipe_second_child_to_parent[0], STDIN_FILENO);

            execl("child2", "child2", NULL);
        } else {
            close(pipe_parent_to_first_child[0]);
            close(pipe_second_child_to_parent[1]);

            char buffer[SIZE_OF_BUFFER];

            while (1) {
                printf("Enter a string (or '~' to exit): ");
                fgets(buffer, sizeof(buffer), stdin);

                if (buffer[0] == '~') {
                    break;
                }

                write(pipe_parent_to_first_child[1], buffer, sizeof(buffer));

                read(pipe_second_child_to_parent[0], buffer, sizeof(buffer));
                printf("Modified string: %.*s\n", (int)sizeof(buffer), buffer);
            }

            close(pipe_parent_to_first_child[1]);
            close(pipe_second_child_to_parent[0]);
        }
    }
    return 0;
}