#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

const int MAX_DIMENSION_SIZE = 10;
const int MAX_GENERATED_NUMBER = 50;

typedef struct complex_number{
    int imaginary_part;
    int real_part;
} complex_number;

typedef struct thread_parameters{
    complex_number **first_matrix;
    complex_number **second_matrix;
    complex_number **result_matrix;
    int start_row;
    int end_row;
    int columns_in_second_matrix;
    int matching_size;
} thread_parameters;

void *multiplication_of_one_element(void* parameters) {

    thread_parameters* data = (thread_parameters*) parameters;
    
    for (int index_of_result_row = data->start_row; index_of_result_row <= data->end_row; ++index_of_result_row) {
        for (int index_of_result_column = 0; index_of_result_column < data->columns_in_second_matrix; ++index_of_result_column) {
            for (int index_of_matching_size = 0; index_of_matching_size < data->matching_size; ++index_of_matching_size) {

                data->result_matrix[index_of_result_row][index_of_result_column].real_part += data->first_matrix[index_of_result_row][index_of_matching_size].real_part *
                                                                                            data->second_matrix[index_of_matching_size][index_of_result_column].real_part -
                                                                                            data->first_matrix[index_of_result_row][index_of_matching_size].imaginary_part *
                                                                                            data->second_matrix[index_of_matching_size][index_of_result_column].imaginary_part;

                data->result_matrix[index_of_result_row][index_of_result_column].imaginary_part += data->first_matrix[index_of_result_row][index_of_matching_size].real_part *
                                                                                                data->second_matrix[index_of_matching_size][index_of_result_column].imaginary_part +
                                                                                                data->first_matrix[index_of_result_row][index_of_matching_size].imaginary_part *
                                                                                                data->second_matrix[index_of_matching_size][index_of_result_column].real_part;
            }
        }
    }
    pthread_exit(NULL);
}

void print_matrix(complex_number** matrix, int number_of_rows, int number_of_columns) {
    for (int index_of_rows = 0; index_of_rows < number_of_rows; ++index_of_rows) {
        for (int index_of_columns = 0; index_of_columns < number_of_columns; ++index_of_columns) {
            printf("%d + %d*i\t", matrix[index_of_rows][index_of_columns].real_part, matrix[index_of_rows][index_of_columns].imaginary_part);
            fflush(stdout);
        }
        printf("\n");
    }
}

void free_matrix(complex_number** matrix, int number_of_rows) {
    for (int index_of_row = 0; index_of_row < number_of_rows; ++index_of_row) {
        free(matrix[index_of_row]);
    }
    free(matrix);
}

void threads_generation(int *entered_number_of_threads, 
                        complex_number **first_matrix,
                        complex_number **second_matrix,
                        complex_number **result_matrix,
                        int rows_in_first_matrix,
                        int matching_size,
                        int columns_in_second_matrix) {


    // Проверка на количество доступных потоков в системе
    int maximum_number_of_threads = (int) sysconf(_SC_NPROCESSORS_ONLN);

    if (*entered_number_of_threads > maximum_number_of_threads)
        *entered_number_of_threads = maximum_number_of_threads;

    pthread_t* threads_array = (pthread_t*) malloc(*entered_number_of_threads * sizeof(pthread_t));
    thread_parameters* array_of_thread_parameters = (thread_parameters*) malloc(*entered_number_of_threads * sizeof(thread_parameters));

    // Распределение перемножаемых рядов на потоки
    int index_of_start_row_for_thread = 0;
    int index_of_end_row_for_thread = -1;
    int number_of_rows_for_each_thread = rows_in_first_matrix / *entered_number_of_threads;
    int rows_for_distribution = rows_in_first_matrix % *entered_number_of_threads;

    for (int index_of_thread = 0; index_of_thread < *entered_number_of_threads; ++index_of_thread) {
        index_of_start_row_for_thread = index_of_end_row_for_thread + 1;
        index_of_end_row_for_thread = index_of_start_row_for_thread + number_of_rows_for_each_thread - 1;
        if (rows_for_distribution > 0) {
            index_of_end_row_for_thread++;
            rows_for_distribution--;
        }

        // Формирование параметров для поточной функции
        array_of_thread_parameters[index_of_thread].first_matrix = first_matrix;
        array_of_thread_parameters[index_of_thread].second_matrix = second_matrix;
        array_of_thread_parameters[index_of_thread].result_matrix = result_matrix;
        array_of_thread_parameters[index_of_thread].start_row = index_of_start_row_for_thread;
        array_of_thread_parameters[index_of_thread].end_row = index_of_end_row_for_thread;
        array_of_thread_parameters[index_of_thread].columns_in_second_matrix = columns_in_second_matrix;
        array_of_thread_parameters[index_of_thread].matching_size = matching_size;

        pthread_create(&(threads_array[index_of_thread]), NULL, multiplication_of_one_element, &array_of_thread_parameters[index_of_thread]);
    }
    
    for (int index_of_thread_number = 0; index_of_thread_number < *entered_number_of_threads; ++index_of_thread_number) {
        if (pthread_join(threads_array[index_of_thread_number], NULL) != 0)
            write(STDERR_FILENO, "Terminating thread error\n", strlen("Terminating thread error\n"));
    }

    print_matrix(result_matrix, rows_in_first_matrix, columns_in_second_matrix);

    free_matrix(first_matrix, rows_in_first_matrix);
    free_matrix(second_matrix, matching_size);
    free_matrix(result_matrix, rows_in_first_matrix);
    free(array_of_thread_parameters);
    free(threads_array);
}

int main(int argc, char* argv[]) {
    if (argc != 2)
        write(STDERR_FILENO, "Incorrect number of threads\n", strlen("Incorrect number of threads\n"));
    
    int entered_number_of_threads = atoi(argv[1]);

    srand((unsigned) time(NULL));

    int rows_in_first_matrix = 5;//rand() % MAX_DIMENSION_SIZE + 1;
    int columns_in_second_matrix = 5;//rand() % MAX_DIMENSION_SIZE + 1;
    int matching_size = 5;//rand() % MAX_DIMENSION_SIZE + 1;

    complex_number **first_matrix = (complex_number**) malloc(rows_in_first_matrix * sizeof(complex_number*));
    complex_number **second_matrix = (complex_number**) malloc(matching_size * sizeof(complex_number*));
    complex_number **result_matrix = (complex_number**) malloc(rows_in_first_matrix * sizeof(complex_number*));

    for (int index_of_row = 0; index_of_row < rows_in_first_matrix; ++index_of_row) {
        first_matrix[index_of_row] = (complex_number*) malloc(matching_size * sizeof(complex_number));
        result_matrix[index_of_row] = (complex_number*) malloc(columns_in_second_matrix * sizeof(complex_number));
    }

    for (int index_of_row = 0; index_of_row < matching_size; ++index_of_row) {
        second_matrix[index_of_row] = (complex_number*) malloc(columns_in_second_matrix * sizeof(complex_number));
    }

    // Генерация матриц
    for (int index_of_row = 0; index_of_row < rows_in_first_matrix; ++index_of_row) {
        for (int index_of_column = 0; index_of_column < matching_size; ++index_of_column) {
            first_matrix[index_of_row][index_of_column].imaginary_part = rand() % MAX_GENERATED_NUMBER;
            first_matrix[index_of_row][index_of_column].real_part = rand() % MAX_GENERATED_NUMBER;
        }
    }
    printf("First matrix with %d rows and %d columns created\n", rows_in_first_matrix, matching_size);

    for (int index_of_row = 0; index_of_row < matching_size; ++index_of_row) {
        for (int index_of_column = 0; index_of_column < columns_in_second_matrix; ++index_of_column) {
            second_matrix[index_of_row][index_of_column].imaginary_part = rand() % MAX_GENERATED_NUMBER;
            second_matrix[index_of_row][index_of_column].real_part = rand() % MAX_GENERATED_NUMBER;
        }
    }
    printf("Second matrix with %d rows and %d columns created\n", matching_size, columns_in_second_matrix);
    
    printf("\nFirst matrix: \n");
    print_matrix(first_matrix, rows_in_first_matrix,matching_size);

    printf("\nSecond matrix: \n");
    print_matrix(second_matrix, matching_size, columns_in_second_matrix);

    printf("\nResult matrix: \n");


    struct timeval start_of_programm, end_of_programm;
    gettimeofday(&start_of_programm, NULL);

    threads_generation(&entered_number_of_threads, first_matrix, second_matrix, result_matrix, rows_in_first_matrix, matching_size, columns_in_second_matrix);

    gettimeofday(&end_of_programm, NULL);

    double time_in_work = (end_of_programm.tv_sec - start_of_programm.tv_sec) * 1000.0 +
                            (end_of_programm.tv_usec - start_of_programm.tv_usec) / 1000.0;
    printf("\nWorking time with %d threads: %.20f\n", entered_number_of_threads, time_in_work);

    return 0;
}