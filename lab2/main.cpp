#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <pthread.h>
#include "timer.h"

using namespace std;

const int MAX_THREADS = 1024;
const int NUM_RANGE = 65535;
const int N = 10;
const int M = 100;

const double M_MEMBER = 0.9;
const double M_INSERT = 0.05;
const double M_DELETE = 0.05;

int *raw_array;
int *input_array;
int *function_array;
int *linkedlist_array;

int program_type;
long thread_count;
pthread_mutex_t mutex;
pthread_rwlock_t rwlock;

struct Node {
    int data;
    Node *next;
};

struct Node* initialize(Node *head);

void getArgs(int argc, int argv);

void *serialProgram(void *rank);

void *parallelProgramMutex(void *rank);

void *parallelProgramRWLock(void *rank);

void initNode(struct Node **head, int n);

void displayList(struct Node *head);

void populateLinkedList(int arr[], int size, struct Node **head);

int insertNode(int value, struct Node **head);

int member(int value, struct Node *head);

int deleteNode(int value, struct Node **head);

struct Node *head;

int main() {

    // use for generation random values using random_shuffle algorithm
    srand(time(0));

    cin >> program_type >> thread_count;
    printf("Program type: %d \n", program_type);
    printf("Thread count: %d \n \n", thread_count);

    head = NULL;

    head = initialize(head);           /* Initialize arrays of data */

    /* call the relevant method according to the user input
     * 1 - Serial program
     * 2 - Parallel program (based on Pthreads) with one mutex for the entire linked list
     * 3 - Parallel program (based on Pthreads) with read-write locks for the entire linked list
     */

    long thread;
    pthread_t *thread_handles;
    double start, finish, elapsed;

    thread_handles = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
    switch (program_type) {
        case 1 : GET_TIME(start);
            displayList(head);
            for (thread = 0; thread < thread_count; thread++)
                pthread_create(&thread_handles[thread], NULL, serialProgram, (void *) thread);

            for (thread = 0; thread < thread_count; thread++)
                pthread_join(thread_handles[thread], NULL);
            GET_TIME(finish);
            elapsed = finish - start;

            printf("\nSerial Program \n");
            printf("------------------------------------------------------------------------ \n");
            printf("No. of threads: %d\n", thread_count);
            printf("The elapsed time: %e seconds\n", elapsed);

            printf("This is the final linkedlist after operations.\n");
            printf("------------------------------------------------------------------------ \n");
            displayList(head);
            break;
        case 2 :
            pthread_mutex_init(&mutex, NULL);

            GET_TIME(start);
            for (thread = 0; thread < thread_count; thread++)
                pthread_create(&thread_handles[thread], NULL, parallelProgramMutex, (void *) thread);

            for (thread = 0; thread < thread_count; thread++)
                pthread_join(thread_handles[thread], NULL);
            GET_TIME(finish);
            elapsed = finish - start;

            pthread_mutex_destroy(&mutex);
            free(thread_handles);

            printf("\nParallel Program with Mutex \n");
            printf("------------------------------------------------------------------------ \n");
            printf("No. of threads: %d,\n", thread_count);
            printf("The elapsed time: %e seconds\n", elapsed);

            printf("This is the final linkedlist after operations.\n");
            printf("------------------------------------------------------------------------ \n");
            displayList(head);
            break;
        case 3 :
            pthread_rwlock_init(&rwlock, NULL);

            GET_TIME(start);
            for (thread = 0; thread < thread_count; thread++)
                pthread_create(&thread_handles[thread], NULL, parallelProgramRWLock, (void *) thread);

            for (thread = 0; thread < thread_count; thread++)
                pthread_join(thread_handles[thread], NULL);
            GET_TIME(finish);
            elapsed = finish - start;

            pthread_rwlock_destroy(&rwlock);
            free(thread_handles);

            printf("\nParallel Program with RW Locks \n");
            printf("------------------------------------------------------------------------ \n");
            printf("No. of threads: %d,\n", thread_count);
            printf("The elapsed time: %e seconds\n", elapsed);

            printf("This is the final linkedlist after operations.\n");
            printf("------------------------------------------------------------------------ \n");
            displayList(head);
            break;
        default :
            printf("Invalid input for program type.");
    }
}

void *parallelProgramRWLock(void *rank) {
    long long i;
    long my_rank = (long) rank;
    long long my_n = N / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    for (i = my_first_i; i < my_last_i; i++) {
        if (function_array[i] == 0) {
            pthread_rwlock_rdlock(&rwlock);
            member(input_array[i], head);
            pthread_rwlock_unlock(&rwlock);
        } else if (function_array[i] == 1) {
            pthread_rwlock_wrlock(&rwlock);
            insertNode(input_array[i], &head);
            pthread_rwlock_unlock(&rwlock);
        } else if (function_array[i] == -1) {
            pthread_rwlock_wrlock(&rwlock);
            deleteNode(input_array[i], &head);
            pthread_rwlock_unlock(&rwlock);
        }
    }
}

void *parallelProgramMutex(void *rank) {
    long long i;
    long my_rank = (long) rank;
    long long my_n = N / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    for (i = my_first_i; i < my_last_i; i++) {
        if (function_array[i] == 0) {
            member(input_array[i], head);
        } else if (function_array[i] == 1) {
            pthread_mutex_lock(&mutex);
            insertNode(input_array[i], &head);
            pthread_mutex_unlock(&mutex);
        } else if (function_array[i] == -1) {
            pthread_mutex_lock(&mutex);
            deleteNode(input_array[i], &head);
            pthread_mutex_unlock(&mutex);
        }
    }
}

void *serialProgram(void *rank) {
    long long i;
    long my_rank = (long) rank;
    long long my_n = N / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    for (i = my_first_i; i < my_last_i; i++) {
        if (function_array[i] == 0) member(input_array[i], head);
        else if (function_array[i] == 1) insertNode(input_array[i], &head);
        else if (function_array[i] == -1) deleteNode(input_array[i], &head);
    }

    displayList(head);
}

/*------------------------------------------------------------------
 * Function:    Initialize
 * Purpose:     Initialize global variables
 * In args:     -
 * Globals out: RAW_DATA, INPUT_ARRAY, FUNCTION_ARRAY
 */

struct Node* initialize(struct Node *head) {

    // initialize the arrays
    raw_array = new int[NUM_RANGE];
    input_array = new int[M];
    function_array = new int[M];
    linkedlist_array = new int[N];

    // calculate count of each function member, insert and delete
    const int count_member = M * M_MEMBER;
    const int count_insert = M * M_INSERT;
    const int count_delete = M * M_DELETE;

    // fill the function array with the required number of functions
    for (int i = 0; i < M; i++)
        if (i < count_member) function_array[i] = 0;
        else if (i < count_member + count_insert) function_array[i] = 1;
        else function_array[i] = -1;

    // fill a raw data array with the numbers from 0 to 2^16 -1
    for (int i = 0; i < NUM_RANGE; i++)
        raw_array[i] = i;

    // shuffle the raw data array and function array
    random_shuffle(raw_array, raw_array + NUM_RANGE);
    random_shuffle(function_array, function_array + M);

    // fill the input array with the random numbers from shuffled raw data array
    for (int i = 0; i < M; i++)
        input_array[i] = raw_array[rand() % NUM_RANGE];

    // fill the linkedlist array with the first N elements from shuffled raw data array
    for (int i = 0; i < N; i++)
        linkedlist_array[i] = raw_array[i];

    printf("This is the input array. Size = %d \n", M);
    printf("------------------------------------------------------------------------ \n");
    for (int i = 0; i < M; i++)
        printf("%d ", input_array[i]);
    printf("\n \n");

    printf("This is the functions array. Size = %d \n", M);
    printf("------------------------------------------------------------------------ \n");
    for (int i = 0; i < M; i++)
        printf("%d ", function_array[i]);
    printf("\n \n");

    printf("This is the initial linkedlist just after populating. Size = %d \n", N);
    printf("------------------------------------------------------------------------ \n");
    populateLinkedList(linkedlist_array, N, &head);
    displayList(head);
    printf("\n \n");

    return head;
}


//Display the linked list
void displayList(struct Node *head) {
    printf("--->>> ");
    struct Node *curr = head;
    while (curr != NULL) {
        cout << curr->data << " ";
        curr = curr->next;
    }
    printf("\n");
}

//Create the linked list
void populateLinkedList(int *arr, int size, struct Node **head) {
    for (int a = 0; a < size; a++) {
        insertNode(arr[a], head);
    }
}

//Add initial node to the linked list
void initNode(struct Node **head, int n) {
    struct Node *curr = *head;
    curr->data = n;
    curr->next = NULL;
}

//Return whether the value is a member
int member(int value, struct Node *head) {
    struct Node *curr = head;
    while (curr != NULL && curr->data < value) {
        curr = curr->next;
    }
    if (curr == NULL || curr->data > value) {
        printf("NO Member : %d \n", value);
        return 0;
    } else {
        printf("YES Member : %d \n", value);
        return 1;
    }
}

//Delete a node from the list
int deleteNode(int value, struct Node **head) {
    struct Node *curr = *head;
    struct Node *pred = NULL;
    while (curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }
    if (curr != NULL || curr->data == value) {
        if (pred == NULL) { /*pred first node.*/
            *head = curr->next;
            free(curr);
        } else {
            pred->next = curr->next;
            free(curr);
        }
        printf("YES Delete : %d \n", value);
        return 1;
    } else { /*value isn't in the list.*/
        printf("NO Delete : %d \n", value);
        return 0;
    }
}

int insertNode(int value, struct Node **head) {
    struct Node *curr = *head;
    struct Node *pred = NULL;
    struct Node *temp;

    while (curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }
    if (curr == NULL || curr->data > value) {
        temp = (struct Node *) malloc(sizeof(struct Node));
        temp->data = value;
        temp->next = curr;
        if (pred == NULL) /*pred first node.*/
            *head = temp;
        else
            pred->next = temp;
        printf("YES Insert : %d \n", value);
        return 1;
    } else {
        return 0;
    }
}