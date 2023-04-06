#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char* type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>


int k;
mutex m;
int ordersProcessing = 0;
mutex mut;
int waiting[2];

sem_t burgerSem;
sem_t friesSem;

// Do not change
void process_order() {
    sleep(2);
}


void place_order(int type) {
    /**
     *  Add logic for synchronization before order processing
     *  Check if already k orders in process; 
     *     if true -> wait (print waiting)
     *     otherwise place this order (print order)
     *  Use type_names[type] to print the order type
     */
    //cout << "burgers waiting : " << waiting[BURGER] << " | fries waiting : " << waiting[FRIES] << " ";
    unique_lock<mutex> ul(mut);
    if (ordersProcessing >= k) {   
        cout << "Waiting: " << type_names[type] << endl;
        waiting[type]++;
        if (type == BURGER) {
            ul.unlock();
            sem_wait(&burgerSem);
            ul.lock();
        } else {
            ul.unlock();
            sem_wait(&friesSem);
            ul.lock();
        }
        cout << "Order: " << type_names[type] << endl;
    } else {
        cout << "Order: " << type_names[type] << endl; 
    }
    ordersProcessing++;
    ul.unlock();


    
    process_order();        // Do not remove, simulates preparation

    /**
     *  Add logic for synchronization after order processed
     *  Allow next order of the same type to proceed if there is any waiting; if not, allow the other type to proceed.
     */
    ul.lock();
    ordersProcessing--;
    if (type == BURGER) {
        if (waiting[0] > 0) {
            sem_post(&burgerSem);
            waiting[0]--;
        } else if (waiting[1] > 0) {
            sem_post(&friesSem);
            waiting[1]--;
        }
    } else {
        if (waiting[1] > 0) {
            sem_post(&friesSem);
            waiting[1]--;
        } else if (waiting[0] > 0) {
            sem_post(&burgerSem);
            waiting[0]--;
        }
    }
    ul.unlock();
}

int main() {
    // Initialize necessary variables, semaphores etc.
    sem_init(&burgerSem, 0, 0);
    sem_init(&friesSem, 0, 0);
    
    // Read data: done for you, do not change
    pii incoming[MAX_THREADS];
    int _type, _arrival;
    int t;
    cin >> k;
    cin >> t;
    for (int i = 0; i < t; ++i) {
        cin >> _type >> _arrival;
        incoming[i].first = _type;
        incoming[i].second = _arrival;
    }

    // Create threads: done for you, do not change
    thread* threads[MAX_THREADS];
    for (int i = 0; i < t; ++i) {
        _type = incoming[i].first;
        threads[i] = new thread(place_order, _type);
        if (i < t - 1) {
            int _sleep = incoming[i + 1].second - incoming[i].second;
            sleep(_sleep);
        }
    }

    // Join threads: done for you, do not change
    for (int i = 0; i < t; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    return 0;
}