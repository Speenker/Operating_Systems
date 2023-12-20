#include "lib.h"

float Square(float sideA, float sideB) {
    return sideA * sideB;
}

char* translation(long number) {
    char* result = (char*)malloc(sizeof(char) * 100);
    int index = 0;
    
    for (; number > 0; number /= 2) {
        result[index] = number % 2;
        index++;
    }

    return result;
}