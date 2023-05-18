// Angel Higueros - 20460

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int available_resources = 10; 
int iterations = 5; 
pthread_mutex_t mutex; 
sem_t semaphore; 
FILE *log_file; 


int decrease_count(int count, int thread_id) {
    for (int i = 0; i < count; i++)
        sem_wait(&semaphore); 
    
    pthread_mutex_lock(&mutex); 

    if (available_resources < count) {
        for (int i = 0; i < count; i++)
            sem_post(&semaphore);
        pthread_mutex_unlock(&mutex); 
        return -1;
    } else {
        available_resources -= count;
        fprintf(log_file, "Thread %d ha consumido %d recursos. Recursos restantes: %d\n", thread_id, count, available_resources);
        pthread_mutex_unlock(&mutex); 
        return 0;
    }
}

int increase_count(int count, int thread_id) {
    pthread_mutex_lock(&mutex); 

    available_resources += count;
    fprintf(log_file, "Thread %d ha devuelto %d recursos. Recursos disponibles: %d\n", thread_id, count, available_resources);

    pthread_mutex_unlock(&mutex);
    for (int i = 0; i < count; i++)
        sem_post(&semaphore); 

    return 0;
}

void *thread_func(void *id) {
    int thread_id = *((int *) id);
    for (int i = 0; i < iterations; i++) {
        pthread_mutex_lock(&mutex);
        if (available_resources > 0) {
            pthread_mutex_unlock(&mutex); 
            int resources_needed = rand() % available_resources + 1;
            while (decrease_count(resources_needed, thread_id) == -1);
            sleep(rand() % 3);

            increase_count(resources_needed, thread_id); 
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int num_threads = 5;
    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&semaphore, 0, available_resources);

    log_file = fopen("log.txt", "w");

    fprintf(log_file, "Creando threads\n");
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        fprintf(log_file, "Iniciando thread %d\n", i);
    }
    fprintf(log_file, "Esperando threads\n");


    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);
    fclose(log_file);

    return 0;
}
