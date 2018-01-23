#include <thread>
#include <chrono>
#include <stdio.h>
#include <string.h>

#define THREAD_COUNT 8

volatile int tickets[THREAD_COUNT];
volatile int choosing[THREAD_COUNT];

volatile int resource;

void lock(int thread) {
    choosing[thread] = 1;
    int max_ticket = 0;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        int ticket = tickets[i];
        max_ticket = ticket > max_ticket ? ticket : max_ticket;
    }
    tickets[thread] = max_ticket + 1;
    choosing[thread] = 0;
    for (int other = 0; other < THREAD_COUNT; ++other) {
        while (choosing[other]) { }
        while (tickets[other] != 0 &&
               (tickets[other] < tickets[thread] ||
                (tickets[other] == tickets[thread] && other < thread))) { }
    }
}

void unlock(int thread) {
    tickets[thread] = 0;
}

void use_resource(int thread) {
    if (resource != 0) {
        printf("Resource was acquired by %d, but is still in-use by %d!\n",
               thread, resource);
    }
    resource = thread;
    printf("%d using resource...\n", thread);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    resource = 0;
}

void *thread_body(void *arg) {

    long thread = (long)arg;
	std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_COUNT-thread));
    lock(thread);
    use_resource(thread);
    unlock(thread);
    return NULL;
}

int main(int argc, char **argv) {
    memset((void*)tickets, 0, sizeof(tickets));
    memset((void*)choosing, 0, sizeof(choosing));
    resource = 0;

    std::thread threads[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; ++i) {
		threads[i] = std::thread(&thread_body, (void*)((long)i));

    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads[i].join();
    }

	getchar();
    return 0;
}