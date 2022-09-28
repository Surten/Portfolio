#include "queue.h"

queue_t* create_queue(int capacity){
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    queue->head = 0;
    queue->tail = 0;
    queue->i = 0;
    queue->capacity = capacity;
    queue->array = malloc(sizeof(void*)*capacity);
    return queue;
}
void delete_queue(queue_t *queue) {
    free(queue->array);
    free(queue);
}
bool push_to_queue(queue_t *queue, void *data){
    if(queue->i == queue->capacity){
        return false;
    }
    queue->i = queue->i + 1;
    queue->array[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    return true;
}
void* pop_from_queue(queue_t *queue){
    void *N;
    if(queue->i == 0){
        return NULL;
    }
    N = queue->array[queue->head];
    queue->array[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->i = queue->i - 1;
    return N;
}
void* get_from_queue(queue_t *queue, int idx){
    void* N;
    if(queue->i == 0 || (queue->head + idx) % queue->capacity>= queue->tail || queue->capacity <= idx || idx < 0){
        return NULL;
    }
    N = queue->array[(queue->head + idx) % queue->capacity];
    return N;
}
int get_queue_size(queue_t *queue){
    return queue->i;
}
