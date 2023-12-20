#include "lib.h"

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
    int command;

    while(scanf("%d", &command) != EOF) {
        switch(command) {
            
            case(-1) : 
                return 0;    
            
            case(1) : 

                float sideA, sideB;
                printf("Enter two sides of the figure: ");
                scanf("%f %f", &sideA, &sideB);
                printf("Result: %f\n", Square(sideA, sideB));
                break;

            case(2) : 

                long number;
                printf("Enter the number to translate: ");
                scanf("%ld", &number);

                char* temp_array = translation(number);
                print_array(temp_array);
                break;

        }
    }
}