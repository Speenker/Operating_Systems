#include "lib.h"

int main() {
    char buffer[SIZE_OF_BUFFER];

    while ((read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        for (int char_in_buffer_iterator = 0; char_in_buffer_iterator < sizeof(buffer); char_in_buffer_iterator++) {
            buffer[char_in_buffer_iterator] = toupper(buffer[char_in_buffer_iterator]);
        }
        write(STDOUT_FILENO, buffer, sizeof(buffer));
    }
    return 0;
}