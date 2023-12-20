#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>

const int SIZE_OF_BUFFER = 100;
const char* SHARED_MEMORY_NAME = "shared_memory";
const int ACCESS_MODE = 0666;
const int SIZE_OF_SHARED_MEMORY = 4096;