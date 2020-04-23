#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

static const long long int Num_To_Add = 100000000;
static const double Scale = 10.0 / RAND_MAX;

long long int add_serial(const int *numbers) {
    long long int sum = 0;
    for (long long int i = 0; i < Num_To_Add; i++) {
        sum += numbers[i];
    }
    return sum;
}

long long int add_parallel(const char *numbers) {
    long long int sum = 0;
    long long int First_i=0;
    long long int Last_i=Num_To_Add;
    for (long long int i = First_i; i < Last_i; i++) {
        sum += numbers[i];
    }
    return sum;
}

int main() {
    int *numbers = malloc(sizeof(long) * Num_To_Add);

    long long int chunk_size = Num_To_Add / omp_get_max_threads();
    #pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long long int chunk_start = p * chunk_size;
        long long int chunk_end = chunk_start + chunk_size;
        for (long long int i = chunk_start; i < chunk_end; i++) {
            numbers[i] = (int) (rand_r(&seed) * Scale);
        }
    }

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    long long int sum_s = add_serial(numbers);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    long long int sum_p = add_parallel(numbers);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Sum serial: %ld\nSum parallel: %ld", sum_s, sum_p);

    free(numbers);
    return 0;
}