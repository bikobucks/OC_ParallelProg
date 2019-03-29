#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

static const long Num_To_Add = 100000000;
static const double Scale = 10.0 / RAND_MAX;

long add_serial(const char *numbers) {
    long sum = 0;
    for (long i = 0; i < Num_To_Add; i++) {
        sum += numbers[i];
    }
    return sum;
}


long add_parallel(const char *numbers) {
    long sum = 0;
    //Start of my Code
    //Bonaventure Biko:: March 28, 2019
    long number_of_threads = 10; //the number of threads allocated
    long my_array_size = Num_To_Add/number_of_threads; //the size of each array

    #pragma omp parallel num_threads(number_of_threads) //reduction (+:sum)
    {
        long my_first_i = omp_get_thread_num()*my_array_size;
        long my_last_i = my_first_i+my_array_size;
        long  my_sum = 0;
        //long my_cumm_sum = 0;

        for (long i = my_first_i; i < my_last_i; i++) {
            my_sum += numbers[ i];

        }
        //printf("\n%d",my_sum);


        #pragma omp critical
        sum += my_sum;
        //my_cumm_sum = my_sum[omp_get_thread_num()]+my_sum[omp_get_thread_num()+1];
    }
    //printf("\n");
    //End of my Code
    return sum;
}

int main() {
    char *numbers = malloc(sizeof(long) * Num_To_Add);

    long chunk_size = Num_To_Add / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            numbers[i] = (char) (rand_r(&seed) * Scale);
        }
    }

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    long sum_s = add_serial(numbers);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    long sum_p = add_parallel(numbers);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Sum serial: %ld\nSum parallel: %ld", sum_s, sum_p);

    free(numbers);
    return 0;
}

