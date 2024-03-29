#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sys/wait.h>
#include "reqchannel.H"
#include "BoundedBuffer.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

#define NUM_PEOPLE 3
#define NUM_BINS 10
#define MAX_WORKER_THREADS 126

enum people {
    JOHN, JANE, JOE
};
string names[] = {"John Doe", "Jane Smith", "Joe Smith"};

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

int HISTOGRAM[NUM_PEOPLE][NUM_BINS];

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/
void *request_thread_func(void *req_args);

void *build_hist(void *func_params);

void *worker_thread_func(void *func_params);

BoundedBuffer<string> *lookup(string req, BoundedBuffer<string> **SBB_container);

void print_histogram(string name, int data[]);

template<typename T>
void clean_up(T *array, int _size);

int main(int argc, char **argv) {
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

    // Start Connection to data server
    // ------------------------------------------------
    cout << "CLIENT STARTED:" << '\n';
    cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << '\n';
    // ------------------------------------------------
    // Setup timer
    // ------------------------------------------------
    clock_t begin = clock();
    // ------------------------------------------------
    // Create worker bounded buffer
    // ------------------------------------------------
    BoundedBuffer<string> *work_buffer = new BoundedBuffer<string>(buffer_size);
    // ------------------------------------------------
    // Start request threads (1 for each patient)
    // ------------------------------------------------
    REQ_PARAMS *req_params[NUM_PEOPLE];

    // Create Req Params
    for (int i = JOHN; i <= JOE; ++i) {
        cout << "Generating Request thread for " << names[i] << '\n';
        req_params[i] = new REQ_PARAMS{
                names[i],           // Patient name
                req_per_person,// Num of Worker Threads
                work_buffer
        };
        pthread_create(&request_threads[i], NULL, request_thread_func, (void *) req_params[i]);
    }
    // ------------------------------------------------
    // Create Statistic bounded buffer
    // ------------------------------------------------
    BoundedBuffer<string> *stats_buffer[NUM_PEOPLE];
    // ------------------------------------------------
    // Start statistic threads (1 for each patient)
    // ------------------------------------------------
    STATS_PARAMS *stat_params[NUM_PEOPLE];

    for (int i = JOHN; i <= JOE; ++i) {
        cout << "Generating Statistic thread for " << names[i] << '\n';
        stats_buffer[i] = new BoundedBuffer<string>(req_per_person);
        stat_params[i] = new STATS_PARAMS{
                i,        // Patient name
                req_per_person,  // Number of request
                stats_buffer[i]  // Stats Buffer
        };
        pthread_create(&stat_threads[i], NULL, build_hist, (void *) stat_params[i]);
    }
    // ------------------------------------------------
    // Start w worker threads
    // ------------------------------------------------
    RequestChannel *req_channel[num_worker_threads];
    string reply;
    WORK_THREAD_PARAMS *w_t_params[num_worker_threads];

    for (int i = 0; i < num_worker_threads; ++i) {
        reply = chan.send_request("newthread");
        cout << "Reply to request for new thread is '" << reply << "'" << '\n';

        req_channel[i] = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);

        w_t_params[i] = new WORK_THREAD_PARAMS{
                work_buffer,  // BoundedBuffer buff;
                stats_buffer, // SBB_Container
                req_channel[i]   // RequestChannel;
        };
        pthread_create(&worker_threads[i], NULL, worker_thread_func, (void *) w_t_params[i]);
    }
    // ------------------------------------------------
    // Wait for request threads to finish
    // ------------------------------------------------
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
    clock_t end = clock();
    double elapsed_time = double(end - begin) / CLOCKS_PER_SEC;

    // ------------------------------------------------
    // Clean up on Aisle 4!
    //------------------------------------------------
    // Close Control
    string reply4 = chan.send_request("quit");
    cout << "Control Reply to request 'quit' is '" << reply4 << "'" << '\n';
    clean_up(req_params, 3);
    clean_up(stats_buffer, 3);
    clean_up(stat_params, 3);
    clean_up(w_t_params, num_worker_threads);
    // Loop because templating didn't work for RequestThread :(
    for (int k = 0; k < num_worker_threads; ++k) {
        delete req_channel[k];
    }

    // Wait for Data Server process to end
    wait(NULL);

    // Print the Histograms
    cout << setfill('-') << setw(40) << '\n'
         << setfill(' ')
         << setw(20) << "Histograms" << setw(20) << '\n'
         << setfill('-') << setw(40) << '\n'
         << setfill(' ');
    for (int i = JOHN; i <= JOE; ++i) {
        print_histogram(names[i], HISTOGRAM[i]);
    }
    // Print Data
    cout << '\n' << setfill('-') << setw(40) << '\n'
         << setfill(' ')
         << setw(20) << "Statistics" << setw(20) << '\n'
         << setfill('-') << setw(40) << '\n'
         << setfill(' ')
         << "Requests per Person: " << req_per_person << '\n'
         << "Buffer Size: " << buffer_size << '\n'
         << "Number of Worker Threads: " << num_worker_threads << '\n'
         << "Requests Run Time: " << elapsed_time << " sec " << '\n';
}

void *request_thread_func(void *req_args) {
    REQ_PARAMS *params = (REQ_PARAMS *) req_args;
    for (int i = 0; i < params->n; i++) {
        string req = "data " + params->patient_name;
        cout << "Depositing Request: " << req << '\n';
        params->buff->Deposit(req);
    }
    return 0;
}

void *build_hist(void *func_params) {
    STATS_PARAMS *params = (STATS_PARAMS *) func_params;
    string item;
    cout << "Building Histograms for " << names[params->patient_index] << '\n';
    int bin_index, value;
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
        cout << "Sending Request: " << req << '\n';
        string reply = params->chan->send_request(req);
        cout << "Response to : " << req << " --- " << reply << '\n';
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
            cout << "Depositing " << req << " into stat buffer for " << names[i] << '\n';
            return SBB_container[i];
        }
    }
}

void print_histogram(string name, int data[]) {
    cout << '\n' << "Histogram for " << name << '\n' << '\n';
    for (int i = 0; i < 100; i += 10) {
        cout << setw(7) << "[" << setw(2) << setfill('0') << i << ", "
             << setw(2) << setfill('0') << i + 9 << "]: " << setfill(' ');
        for (int j = 0; j < data[i / 10]; ++j) {
            cout << '*';
        }
        cout << '\n';
    }
}

template<typename T>
void clean_up(T *array, int _size) {
    for (int i = 0; i < _size; ++i) {
        delete array[i];
    }
}