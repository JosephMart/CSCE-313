#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <bits/unique_ptr.h>
#include <cmath>
#include <sys/time.h>
#include "reqchannel.H"
#include "BoundedBuffer.h"


using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

#define NUM_PEOPLE 3
#define NUM_BINS 10
#define MAX_WORKER_THREADS 126

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

typedef struct gen_req_params {
    string patient_name;
    int n;
    BoundedBuffer<string> *buff;
} REQ_PARAMS;

typedef struct stats_params {
    int patient_index;
    int n;
    BoundedBuffer<string> *buff;
} STATS_PARAMS;

typedef struct work_thread_params {
    BoundedBuffer<string> *buff;
    BoundedBuffer<string> **SSB_container;
    RequestChannel *chan;
} WORK_THREAD_PARAMS;


enum people {
    JOHN, JANE, JOE
};
string names[] = {"John Doe", "Jane Smith", "Joe Smith"};

int HISTOGRAM[NUM_PEOPLE][NUM_BINS];

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/
void *request_thread_func(void *req_args);

void *build_hist(void *func_params);

void *worker_thread_func(void *func_params);

BoundedBuffer<string> *lookup(string req, BoundedBuffer<string> **SBB_container);

void print_histogram(string name, int data[]);


int main(int argc, char **argv) {
//    setbuf(stdout, NULL);
    int c = 0;
    extern char *optarg;

    // (-n) number of data requests per person
    int req_per_person = 0;
    // (-b) size of bounded buffer between request and worker threads
    int buffer_size = 0;
    // (-w) number of worker threads
    int num_worker_threads = 0;

    while ((c = getopt(argc, argv, "n:b:w:")) != -1) {
        switch (c) {
            case 'n':
                req_per_person = (unsigned int) strtoul(optarg, NULL, 10);
                break;
            case 'b':
                buffer_size = (unsigned int) strtoul(optarg, NULL, 10);
                break;
            case 'w':
                num_worker_threads = (unsigned int) strtoul(optarg, NULL, 10);
                break;
            default:
                break;
        }
    }

    // Initialize the threads - One thread for each person
    pthread_t request_threads[NUM_PEOPLE];
    pthread_t stat_threads[NUM_PEOPLE];

    // Create threads passed on passed param
    pthread_t worker_threads[num_worker_threads];


    // Start the dataserver
    pid_t id = fork();
    if (id < 0) {
        cerr << "Failed to fork." << '\n';
        exit(1);
    }

    if (id == 0) {
        execv("./dataserver", 0);
        _exit(1);
    }

    // DEBUG parsed params
    printf("Requests per Person: %i\n"
                   "Buffer Size: %i\n"
                   "Number of Worker Threads: %i\n", req_per_person, buffer_size, num_worker_threads);

    // Start Connection to data server
    // ------------------------------------------------
    cout << "CLIENT STARTED:" << endl;
    cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;
    // ------------------------------------------------

    // Setup timers
    // ------------------------------------------------
    timeval begin, end;
    gettimeofday(&begin, NULL);
    // ------------------------------------------------

    // Create worker bounded buffer
    // ------------------------------------------------
    BoundedBuffer<string> *work_buffer = new BoundedBuffer<string>(buffer_size);
    // ------------------------------------------------

    // Start request threads (1 for each patient)
    // ------------------------------------------------
    REQ_PARAMS *req_params;

    // Create Req Params
    for (int i = JOHN; i <= JOE; ++i) {
        cout << "Generating Request thread for " << names[i] << endl;
        req_params = new REQ_PARAMS{
                names[i],           // Patient name
                req_per_person,// Num of Worker Threads
                work_buffer
        };
        pthread_create(&request_threads[i], NULL, request_thread_func, (void *) req_params);
    }
    // ------------------------------------------------

    // Create Statistic bounded buffer
    // ------------------------------------------------
    BoundedBuffer<string> *stats_buffer[NUM_PEOPLE];
    // ------------------------------------------------

    // Start statistic threads (1 for each patient)
    // ------------------------------------------------
    STATS_PARAMS *stat_params;

    for (int i = JOHN; i <= JOE; ++i) {
        cout << "Generating Statistic thread for " << names[i] << endl;
        stats_buffer[i] = new BoundedBuffer<string>(req_per_person);
        stat_params = new STATS_PARAMS{
                i,        // Patient name
                req_per_person,  // Number of request
                stats_buffer[i]  // Stats Buffer
        };

        pthread_create(&stat_threads[i], NULL, build_hist, (void *) stat_params);
    }
    // ------------------------------------------------

    // Start w worker threads
    // ------------------------------------------------
    RequestChannel *req_channel;
    string reply;
    WORK_THREAD_PARAMS *w_t_params;

    for (int i = 0; i < num_worker_threads; ++i) {
        reply = chan.send_request("newthread");
        cout << "Reply to request for new thread is '" << reply << "'" << endl;

        req_channel = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);

        w_t_params = new WORK_THREAD_PARAMS{
                work_buffer,  // BoundedBuffer buff;
                stats_buffer, // SBB_Container
                req_channel   // RequestChannel;
        };

        pthread_create(&worker_threads[i], NULL, worker_thread_func, (void *) w_t_params);
    }
    // ------------------------------------------------

    // Wait for request threads to finish
    for (int j = 0; j < NUM_PEOPLE; ++j) {
        pthread_join(request_threads[j], NULL);
    }

    // Kill the workers
    for (int i = 0; i < num_worker_threads; ++i) {
        work_buffer->Deposit("quit");
    }

    // Wait for worker threads to exit
    for (int i = 0; i < num_worker_threads; ++i) {
        pthread_join(worker_threads[i], NULL);
    }

    // Wait for stat threads to exit
    for (int i = 0; i < NUM_PEOPLE; ++i) {
        pthread_join(stat_threads[i], NULL);
    }

    // Time end of Requests
    gettimeofday(&end, NULL);

    // CLose Control
    string reply4 = chan.send_request("quit");
    cout << "Control Reply to request 'quit' is '" << reply4 << "'" << endl;

    usleep(1000000);

    // Print the Histograms
    for (int i = JOHN; i <= JOE; ++i) {
        print_histogram(names[i], HISTOGRAM[i]);
    }
    cout << endl << "Total request time: " << end.tv_sec - begin.tv_sec << " sec " << end.tv_usec - begin.tv_usec
         << " musec" << endl;
}


void *request_thread_func(void *req_args) {
    REQ_PARAMS *params = (REQ_PARAMS *) req_args;
    for (int i = 0; i < params->n; i++) {
        string req = "data " + params->patient_name;
        cout << "Depositing Request: " << req << endl;
        params->buff->Deposit(req);
    }
    return 0;
}

void *build_hist(void *func_params) {
    STATS_PARAMS *params = (STATS_PARAMS *) func_params;
    string item;
    cout << "Building Histograms for " << names[params->patient_index] << endl;
    int bin_index;
    int value;
    for (int i = 0; i < params->n; i++) {
        item = params->buff->Remove();
        value = atoi(item.c_str());
        bin_index = (int) (floor(value / 10));
        HISTOGRAM[params->patient_index][bin_index] += 1;
    }
    return 0;
}

void *worker_thread_func(void *func_params) {
    WORK_THREAD_PARAMS *params = (WORK_THREAD_PARAMS *) func_params;
    while (1) {
        string req = params->buff->Remove();
        cout << "Sending Request: " << req << endl;
        string reply = params->chan->send_request(req);
        cout << "Response to : " << req << " --- " << reply << endl;
        if (req == "quit") {
            break;
        }
        BoundedBuffer<string> *sbb = lookup(req, params->SSB_container);
        sbb->Deposit(reply);
    }
    return 0;
}

BoundedBuffer<string> *lookup(string req, BoundedBuffer<string> **SBB_container) {
    for (int i = JOHN; i <= JOE; ++i) {
        size_t found = req.find(names[i]);
        if (found != std::string::npos) {
            cout << "Depositing " << req << " into stat buffer for " << names[i] << endl;
            return SBB_container[i];
        }
    }
}

void print_histogram(string name, int data[]) {
    cout << endl << endl << "Histogram for " << name << endl << endl;
    for (int i = 0; i < 100; i += 10) {
        cout << "[" << i << ", " << i + 9 << "]: " << data[i / 10] << endl;
    }
}