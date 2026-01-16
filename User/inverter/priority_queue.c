#include <stdbool.h>
#include <stdio.h>

#define MAX_SIZE 100

typedef struct
{
    struct QueueItem
    {
        int value;
        int priority;
    } items[MAX_SIZE];
    int rear;
} PriorityQueue;

bool isPriorityQueueEmpty(PriorityQueue *pq)
{
    return pq->rear == -1;
}

bool isPriorityQueueFull(PriorityQueue *pq)
{
    return pq->rear == MAX_SIZE - 1;
}

void enqueuePriority(PriorityQueue *pq, int value, int priority)
{
    if (isPriorityQueueFull(pq)){
        printf("Priority Queue is full. Cannot enqueue.\n");

        return;
    }

    int idx = pq->rear;

    while (idx >= 0 && pq->items[idx].priority < priority)
    {
        pq->items[idx + 1] = pq->items[idx];
        idx--;
    }

    pq->items[idx + 1].value = value;
    pq->items[idx + 1].priority = priority;

    pq->rear++;
}

int dequeuePriority(PriorityQueue *pq)
{
    if (isPriorityQueueEmpty(pq)){
        printf("Priority Queue is empty. Cannot dequeue.\n");

        return -1;
    }

    int value = pq->items[pq->rear].value;
    pq->rear--;

    return value;
}

static PriorityQueue eventPriorityQueue; // Static priority queue object

// Getter function for eventPriorityQueue
PriorityQueue *getEventPriorityQueue()
{
    return &eventPriorityQueue;
}