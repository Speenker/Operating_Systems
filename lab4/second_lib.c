#include "lib.h"

float Square(float sideA, float sideB) {
    return 0.5 * (sideA * sideB);
}

char* translation(long number) {
    char* result = (char*)malloc(sizeof(char) * 100);
    int index = 0;
    
    for (; number > 0; number /= 3) {
        result[index] = number % 3;
        index++;
    }

    return result;
}