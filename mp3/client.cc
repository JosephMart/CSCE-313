#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <bits/unique_ptr.h>
#include "reqchannel.H"


using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

typedef struct gen_req_params {
    string patient_name;
    int n;
//    BoundedBuffer buff;
} REQ_PARAMS;

//typedef struct stats_params {
//    string patient_name;
//    BoundedBuffer buff;
//} STATS_PARAMS;
//
typedef struct work_thread_params {
//    BoundedBuffer buff;
    // SBB_container;
    RequestChannel chan;
} WORK_THREAD_PARAMS;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

#define NUM_PEOPLE 3
#define MAX_WORKER_THREADS 126

enum people {
    JOHN, JANE, JOE
};
string names[] = {"John Doe", "Jane Smith", "Joe Smith"};

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/
void* request_thread_func(void *req_args);

void build_hist(void *func_params);

void* worker_thread_func(void *func_params);


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

    // Initialize the threads
    // One thread for each person
    pthread_t request_threads[NUM_PEOPLE];
    pthread_t stat_threads[NUM_PEOPLE];

    // Create threads passed on passed param
    pthread_t worker_threads[num_worker_threads];


    // Start the dataserver
    // pid_t id = fork();
    // if (id < 0) {
    //         cerr << "Failed to fork." << '\n';
    //         exit(1);
    // }
    //
    // if (id == 0) {
    //         execv("./dataserver", 0);
    //         _exit(1);
    // }

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

    // Create worker bounded buffer
    // ------------------------------------------------
    // ------------------------------------------------

    // Start w worker threads
    // ------------------------------------------------
    // Create num_worker_threads Request Channels
    unique_ptr<RequestChannel> req_channels[num_worker_threads];
    string reply;
    WORK_THREAD_PARAMS* w_t_params;

    for (int i = 0; i < num_worker_threads; ++i) {
        reply = chan.send_request("newthread");
        cout << "Reply to request for new thread is '" << reply << "'" << endl;

        req_channels[i] = unique_ptr<RequestChannel>(
                new RequestChannel(reply, RequestChannel::CLIENT_SIDE)
        );

        w_t_params = new WORK_THREAD_PARAMS{
                //    BoundedBuffer buff;
                // SBB_container;
                *req_channels[i] // RequestChannel;
        };

        pthread_create(&worker_threads[i], NULL, worker_thread_func, (void*) w_t_params);
    }
    // ------------------------------------------------


    // Start request threads (1 for each patient)
    // ------------------------------------------------
    unique_ptr<REQ_PARAMS> req_params[NUM_PEOPLE];

    // Create Req Params
    for (int i = JOHN; i <= JOE; ++i) {
        cout << "Generating Request thread for " << names[i] << endl;
        req_params[i] = unique_ptr<REQ_PARAMS> (new REQ_PARAMS{
                names[i],           // Patient name
                num_worker_threads// Num of Worker Threads
        });
        pthread_create(&request_threads[i], NULL, request_thread_func, (void*) req_params);
    }
    // ------------------------------------------------

//    pthread_create(req_thread[1], generate_requests, (void *) req_args[1])

    // Start statistic threads (1 for each patient)
    // Create Statistic bounded buffer

    // Close Req Channels
    for (int j = 0; j < NUM_PEOPLE; ++j) {
        string reply = req_channels[j]->send_request("quit");
        cout << "Reply to request 'quit' is '" << reply << "'" << endl;
    }

    // CLose Control
    string reply4 = chan.send_request("quit");
    cout << "Reply to request 'quit' is '" << reply4 << "'" << endl;

    usleep(1000000);

}


void* request_thread_func(void *req_args) {
    REQ_PARAMS *params = (REQ_PARAMS *) req_args;
    for (int i = 0; i < params->n; i++) {
        string req = "data" + params->patient_name;
//        params->buff->deposit(req);
    }
    return 0;
}

// statistics thread
//void build_hist(void *func_params) {
//    STATS_PARAMS *params = (STATS_PARAMS *) func_params;
//    for (int i = 0; i < params->n; i++) {
//        string req = "data" + params->patient_name;
//        params->buff->deposit(req);
//    }
//}

void* worker_thread_func(void *func_params) {
    WORK_THREAD_PARAMS *params = (WORK_THREAD_PARAMS *) func_params;
//    while (1) {
//        string req = params->buff->withdraw();
//        string reply = params->chan->send_request(req);
//        SBB *sbb = lookup(req, SSB_container);
//        ssb->deposit(reply);
//    }
    return 0;
}