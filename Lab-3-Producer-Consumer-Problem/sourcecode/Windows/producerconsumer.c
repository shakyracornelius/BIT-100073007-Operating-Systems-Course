// BIT 100073007 Operating Systems Course Lab 3: Producer-Consumer Problem
// WIN32

#include <stdio.h>   // printf(), fprintf()
#include <windows.h> //
#include <stdlib.h>  // rand()
#include <string.h>  // strlen()
#include <tchar.h>   // ?
#include <strsafe.h> // ?

#define BUFFER_SIZE 4

#define PRODUCERS 3
#define CONSUMERS 4

#define PRODUCER_ITERATIONS 4 // number of times producer loops
#define CONSUMER_ITERATIONS 3 // number of times consumer loops

// Function prototype
DWORD WINAPI Producer(LPVOID lpParam);
DWORD WINAPI Consumer(LPVOID lpParam);

// define threads
DWORD consumerId[CONSUMERS], producerId[PRODUCERS];
HANDLE producerthreads[PRODUCERS];
HANDLE consumerthreads[CONSUMERS];

// define semephore empty, full, mutex
HANDLE empty, full, mutex;

typedef struct
{
    char value[BUFFER_SIZE];
    int next_in, next_out;
} buffer_t;

buffer_t buffer;

// insertInitial function is used to add an initial to the buffer
int insertInitial(char initial, long int id)
{
    buffer.value[buffer.next_in] = initial;
    buffer.next_in = (buffer.next_in + 1) % BUFFER_SIZE;
    printf("producer %ld: produced %c\n", id, initial);

    printf("\t\t\t\t\t\tBuffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%c ", buffer.value[i]);
    }
    printf("\n");

    return 0;
}

// consumeInitial function is used to consume an initial from the buffer
int consumeInitial(char *initial, long int id)
{
    *initial = buffer.value[buffer.next_out];
    buffer.value[buffer.next_out] = '-';
    buffer.next_out = (buffer.next_out + 1) % BUFFER_SIZE;
    printf("\t\t\tconsumer %ld: consumed %c\n", id, *initial);

    printf("\t\t\t\t\t\tBuffer: ");

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%c ", buffer.value[i]);
    }
    printf("\n");

    return 0;
}

// Producer will iterate PRODUCER_ITERATIONS times and call the insertInitial function to insert an initial to the buffer
// Producer argument param is an integer id of the producer used to distiguish between the multiple producer threads
DWORD WINAPI Producer(LPVOID lpParam)
{

    char initials[] = "XQL";
    int length = strlen(initials);

    char initial;
    long int id = (long int)lpParam;

    int j = PRODUCER_ITERATIONS;

    while (j--)
    {
        // wait for random length of time from 0 to 3 seconds
        int randnum = rand() % 4; // range between 0 and 3

        // insert random initial into buffer
        int randInitial = rand() % length;

        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);
        initial = initials[randInitial];
        if (insertInitial(initial, id))
            fprintf(stderr, "Error while inserting to buffer\n");

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(full, 1, NULL);
    }

    ExitThread(0);
}

// Consumer will iterate CONSUMER_ITERATIONS times and call the consumeInitial function to insert an initial to the buffer
// Consumer argument param is an integer id of the consumer used to distiguish between the multiple consumer threads
DWORD WINAPI Consumer(LPVOID lpParam)
{

    char initial;
    long int id = (long int)lpParam;

    int k = CONSUMER_ITERATIONS;

    while (k--)
    {
        Sleep((rand() % 6) * 1000);

        // read from buffer
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);
        if (consumeInitial(&initial, id))
            fprintf(stderr, "Error while removing from buffer\n");
        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(empty, 1, NULL);
    }

    ExitThread(0);
}

int _tmain()

{
    buffer.next_in = 0;
    buffer.next_out = 0;

    // Initialize buffer with '-'
    printf("\nInitialize buffer of size %d with '-' \n", BUFFER_SIZE);

    printf("\t\t\t\t\t\tBuffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer.value[i] = '-';
        printf("%c ", buffer.value[i]);
    }
    printf("\n");

    // create producer and consumer threads
    long int i;

    printf("\nCreating %d Consumers and %d Producers\n\n", CONSUMERS, PRODUCERS);

    full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
    empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    mutex = CreateSemaphore(NULL, 1, 1, NULL);

    if (full == NULL)
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    if (empty == NULL)
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    if (mutex == NULL)
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    // create the producer threads
    for (i = 0; i < PRODUCERS; i++)
    {
        producerthreads[i] = CreateThread(NULL, 0, Producer, (LPVOID)i, 0, &producerId[i]);

        if (producerthreads[i] == NULL)
        {
            printf("Error on line 197\n");
            ExitProcess(3);
        }
    }

    // create consumer threads
    for (i = 0; i < CONSUMERS; i++)
    {
        consumerthreads[i] = CreateThread(NULL, 0, Consumer, (LPVOID)i, 0, &consumerId[i]);

        if (consumerthreads[i] == NULL)
        {
            printf("Error on line 197\n");
            ExitProcess(3);
        }
    }

    // wait for threads to complete
    for (i = 0; i < PRODUCERS; i++)
    {
        WaitForSingleObject(producerthreads[i], INFINITE);
    }

    for (i = 0; i < CONSUMERS; i++)
    {
        WaitForSingleObject(consumerthreads[i], INFINITE);
    }

    for (int i = 0; i < PRODUCERS; i++)
    {
        CloseHandle(producerthreads[i]);
    }

    for (int i = 0; i < CONSUMERS; i++)
    {
        CloseHandle(consumerthreads[i]);
    }

    CloseHandle(full);
    CloseHandle(empty);
    CloseHandle(mutex);

    return 0;
}