// Angel Higueros - 20460

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int available_resources = 10; // numero de recursos disponibles
int iterations = 5; // Número de iteraciones por thread
pthread_mutex_t mutex; // Mutex para proteger el recurso compartido
sem_t semaphore; // Semáforo para controlar el acceso al recurso
FILE *log_file; // Archivo para registrar los eventos


int decrease_count(int count, int thread_id) {
    sem_wait(&semaphore); // Espera a que un recurso esté disponible
    fprintf(log_file, "%d - Semáforo abierto con éxito.\n", thread_id);

    pthread_mutex_lock(&mutex); // Bloquea el mutex

    if (available_resources < count) {
        pthread_mutex_unlock(&mutex); // Desbloquea el mutex
        sem_post(&semaphore); // Señala que un recurso está disponible
        return -1;
    } else {
        available_resources -= count;
        fprintf(log_file, "%d - (!) Recurso tomado\n", thread_id);
        pthread_mutex_unlock(&mutex); // Desbloquea el mutex
        return 0;
    }
}

int increase_count(int count, int thread_id) {
    pthread_mutex_lock(&mutex); // Bloquear el mutex

    available_resources += count;
    fprintf(log_file, "%d - Recurso devuelto :)\n", thread_id);

    pthread_mutex_unlock(&mutex); // Desbloquear el mutex
    sem_post(&semaphore); // recurso está disponible

    return 0;
}

void *thread_func(void *id) {
    int thread_id = *((int *) id);
    for (int i = 0; i < iterations; i++) {
        fprintf(log_file, "Iniciando iteración %d\n", i + 1);
        while (decrease_count(1, thread_id) == -1);

        // Simula trabajar con el recurso
        fprintf(log_file, "%d - Buenos días! Recurso usado\n", thread_id);
        sleep(rand() % 3); 

        increase_count(1, thread_id); // Devuelve el recurso
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Ajuste el número de threads
    int num_threads = 5;
    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    // Inicializar el mutex y el semáforo
    pthread_mutex_init(&mutex, NULL);
    sem_init(&semaphore, 0, available_resources);

   
    log_file = fopen("log.txt", "w");

    // Crear los threads
    fprintf(log_file, "Creando threads\n");
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        fprintf(log_file, "Iniciando thread %d\n", i);
    }
    fprintf(log_file, "Esperando threads\n");


    // Esperar a que todos los threads terminen
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);
    fclose(log_file);

    return 0;
}
