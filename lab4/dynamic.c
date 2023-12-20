#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>

const char* FIRST_LIBRARY = "build/libfirst_lib.so";
const char* SECOND_LIBRARY = "build/libsecond_lib.so";
void* descriptor = NULL;
int current_library = 2;

float(*Square)(float sideA, float sideB) = NULL;

char*(*translation)(long number) = NULL;

void switch_library() {
    if(current_library == 1) {

        if(descriptor != NULL)
            dlclose(descriptor);

        descriptor = dlopen(SECOND_LIBRARY, RTLD_LAZY);

        Square = dlsym(descriptor, "Square");
        translation = dlsym(descriptor, "translation");

        current_library = 2;


    } else {

        if(descriptor != NULL)
            dlclose(descriptor);

        descriptor = dlopen(FIRST_LIBRARY, RTLD_LAZY);

        Square = dlsym(descriptor, "Square");
        translation = dlsym(descriptor, "translation");

        current_library = 1;

    }
    printf("Library switched\n");
}

void print_array(char* temp_array) {
    bool flag = true;
    for (int index = sizeof(temp_array); index >= 0; index--) {
        if (temp_array[index] == 0 && flag) {
            continue;
        }
        flag = false;
        printf("%d", temp_array[index]);
    }
    printf("\n");
}

int main() {
    switch_library();
    int command;

    while(scanf("%d", &command) != EOF) {
        switch(command) {
            
            case(-1) : 
                dlclose(descriptor);
                return 0;    

            case(0) : 
                switch_library();
                break;

            case(1) : 

                float sideA, sideB;
                printf("Enter two sides of the figure: ");
                scanf("%f %f", &sideA, &sideB);
                printf("Result: %f\n", (*Square)(sideA, sideB));
                break;

            case(2) : 

                long number;
                printf("Enter the number to translate: ");
                scanf("%ld", &number);

                char* temp_array = (*translation)(number);
                print_array(temp_array);
                break;

        }
    }
}