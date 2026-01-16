#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

#define MAX_SIZE 100

PriorityQueue
{
    struct QueueItem
    {
        int value;
        int priority;
    } items[MAX_SIZE];
    int rear;
};

bool isPriorityQueueEmpty(PriorityQueue *pq);
bool isPriorityQueueFull(PriorityQueue *pq);
void enqueuePriority(PriorityQueue *pq, int value, int priority);
int dequeuePriority(PriorityQueue *pq);
PriorityQueue *getEventPriorityQueue();

#endif // PRIORITY_QUEUE_H