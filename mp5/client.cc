#include "BoundedBuffer.h"
#include "netreqchannel.H"
#include "reqchannel.H"
#include <cmath>
#include <cstdlib>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <zconf.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

#define NUM_PEOPLE 3
#define NUM_BINS 10
#define MAX_WORKER_THREADS 126

enum people { JOHN, JANE, JOE };
string names[] = { "John Doe", "Jane Smith", "Joe Smith" };

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

typedef struct gen_req_params {
    string patient_name;            // Name of Patient
    int n;                          // Number of request for the patient
    BoundedBuffer<string> *buff;    // Buffer where to deposit requests
} REQ_PARAMS;

typedef struct stats_params {
    int patient_index;              // Patients index in const people
    int n;                          // Number of requests for the patient
    BoundedBuffer<string> *buff;    // Buffer where stats requests are pulled from
} STATS_PARAMS;

typedef struct work_thread_params {
    int num_request_channels;                 // Number of requests
    int num_requests;                         // Number of requests per patient
    BoundedBuffer<string> *buff;              // Buffer where requests are pulled from
    BoundedBuffer<string> **SSB_container;    // Buffer where stats requests are deposited
    string host_name;
    string port_num;
} WORK_THREAD_PARAMS;

int HISTOGRAM[NUM_PEOPLE][NUM_BINS];    // Store the histograms for the patients

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

int main(int argc, char **argv)
{
    cout.setf(std::ios::unitbuf);

    extern char *optarg;
    int c                    = 10;             // Value for temp storage of param
    int req_per_person       = 10;             // (-n) number of data requests per person
    int buffer_size          = 10;             // (-b) size of bounded buffer between request and worker threads
    int num_request_channels = 10;             // (-w) number of worker threads
    string port_num          = "12005";        // (-p) port number 9000-15000
    string host_name         = "localhost";    // (-h) host name/"localhost"

    while ((c = getopt(argc, argv, "n:b:w:p:h:")) != -1) {
        std::stringstream in;
        switch (c) {
        case 'n':
            req_per_person = (unsigned int) strtoul(optarg, NULL, 10);
            break;
        case 'b':
            buffer_size = (unsigned int) strtoul(optarg, NULL, 10);
            break;
        case 'w':
            num_request_channels = (unsigned int) strtoul(optarg, NULL, 10);
            break;
        case 'p':
            in << optarg;
            in >> port_num;
            break;
        case 'h':
            in << optarg;
            in >> host_name;
            break;
        default:
            break;
        }
    }

    cout << req_per_person << endl;

    // Initialize the threads - One thread for each person
    pthread_t request_threads[NUM_PEOPLE];
    pthread_t stat_threads[NUM_PEOPLE];

    // Create master worker thread
    pthread_t worker_thread;

    // ------------------------------------------------
    // Start Connection to data server
    // ------------------------------------------------
    cout << "CLIENT STARTED:" << '\n';

    // ------------------------------------------------
    // Create worker bounded buffer
    // ------------------------------------------------
    BoundedBuffer<string> *work_buffer = new BoundedBuffer<string>(buffer_size);

    // ------------------------------------------------
    // Create request threads (1 for each patient)
    // ------------------------------------------------
    REQ_PARAMS *req_params[NUM_PEOPLE];

    // Create Req Params
    for (int i = JOHN; i <= JOE; ++i) {
        cout << "Generating Request thread for " << names[i] << '\n';
        req_params[i] = new REQ_PARAMS{ names[i],          // Patient name
                                        req_per_person,    // Num of Worker Threads
                                        work_buffer };     // Master Worker Buffer
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
        stat_params[i]  = new STATS_PARAMS{
            i,                 // Patient name
            req_per_person,    // Number of request
            stats_buffer[i]    // Stats Buffer
        };
        pthread_create(&stat_threads[i], NULL, build_hist, (void *) stat_params[i]);
    }

    // ------------------------------------------------
    // Start Master worker thread
    // ------------------------------------------------
    WORK_THREAD_PARAMS *w_t_params;

    w_t_params = new WORK_THREAD_PARAMS{ num_request_channels,
                                         req_per_person * 3,
                                         work_buffer,     // BoundedBuffer buff;
                                         stats_buffer,    // SBB_Container
                                         host_name,
                                         port_num };

    pthread_create(&worker_thread, NULL, worker_thread_func, (void *) w_t_params);
    // ------------------------------------------------
    // Wait for request threads to finish
    // ------------------------------------------------
    for (int j = 0; j < NUM_PEOPLE; ++j) {
        pthread_join(request_threads[j], NULL);
    }

    // Wait for worker thread to exit
    pthread_join(worker_thread, NULL);
    cout << "WORKER THREAD DONE";

    // Wait for stat threads to exit
    for (int i = 0; i < NUM_PEOPLE; ++i) {
        pthread_join(stat_threads[i], NULL);
    }

    // ------------------------------------------------
    // Clean up on Aisle 4!
    //------------------------------------------------
    cout << "Cleaning up Request Params\n";
    clean_up(req_params, 3);
    cout << "Cleaning up Stats Buffer\n";
    clean_up(stats_buffer, 3);
    cout << "Cleaning up Stats Params\n";
    clean_up(stat_params, 3);
    delete w_t_params;

    // Print the Histograms
    cout << setfill('-') << setw(40) << '\n'
         << setfill(' ') << setw(20) << "Histograms" << setw(20) << '\n'
         << setfill('-') << setw(40) << '\n'
         << setfill(' ');
    for (int i = JOHN; i <= JOE; ++i) {
        print_histogram(names[i], HISTOGRAM[i]);
    }
    // Print Data
    cout << '\n'
         << setfill('-') << setw(40) << '\n'
         << setfill(' ') << setw(20) << "Statistics" << setw(20) << '\n'
         << setfill('-') << setw(40) << '\n'
         << setfill(' ') << "Requests per Person: " << req_per_person << '\n'
         << "Buffer Size: " << buffer_size << '\n'
         << "Number of Worker Threads: " << num_request_channels << '\n';
    return 0;
}

void *request_thread_func(void *req_args)
{
    REQ_PARAMS *params = (REQ_PARAMS *) req_args;
    for (int i = 0; i < params->n; i++) {
        string req = "data " + params->patient_name;
        cout << "Depositing Request: " << req << '\n';
        params->buff->Deposit(req);
    }
    params->buff->Deposit("quit");
    cout << "Deposited (" << params->n << ") Requests\n\n";
    return 0;
}

void *build_hist(void *func_params)
{
    STATS_PARAMS *params = (STATS_PARAMS *) func_params;
    string item;
    cout << "Building Histograms for " << names[params->patient_index] << '\n';
    int bin_index, value;
    // for (int i = 0; i < params->n; i++) {
    for (;;) {
        item  = params->buff->Remove();
        value = atoi(item.c_str());
        cout << value << endl;

        if (value == -1) {
            break;
        }
        bin_index = (int) (floor(value / 10));
        HISTOGRAM[params->patient_index][bin_index] += 1;
    }
    cout << "Histograms built for " << names[params->patient_index] << '\n';
    return 0;
}

void *worker_thread_func(void *func_params)
{
    WORK_THREAD_PARAMS *params      = (WORK_THREAD_PARAMS *) func_params;
    NetReqChannel **requestChannels = new NetReqChannel *[params->num_request_channels];

    string *requests = new string[params->num_request_channels];

    int run        = 1;
    int req_count  = 0;
    int quit_count = 0;

    string reply;
    NetReqChannel *tmp_chan;
    fd_set read_fds;

    for (int i = 0; i < params->num_request_channels; ++i) {
        // params->ctl_chan->cwrite("newthread");
        // reply              = params->ctl_chan->cread();
        requestChannels[i] = new NetReqChannel(params->host_name, params->port_num);

        cout << "Creating newthread " << '\n';
        // cout << "Response to newthread: "
        //      << " --- " << reply << '\n';

        requestChannels[i]->cwrite("hello");
        requests[i] = "null";
    }

    while (run) {
        FD_ZERO(&read_fds);
        for (int i = 0; i < params->num_request_channels; ++i) {
            FD_SET(requestChannels[i]->read_fd(), &read_fds);
        }

        // Wait until a file descriptor is changed
        select(params->num_request_channels, &read_fds, nullptr, nullptr, nullptr);

        // See what file descriptor is changed
        for (int i = 0; i < params->num_request_channels; ++i) {
            // Check if the FD was changed
            if (FD_ISSET(requestChannels[i]->read_fd(), &read_fds) != 0) {
                tmp_chan     = requestChannels[i];
                string reply = tmp_chan->cread();

                cout << "Response to : " << requests[i] << " --- " << reply << '\n';

                if (requests[i].find("null") == std::string::npos) {
                    req_count++;
                    BoundedBuffer<string> *sbb = lookup(requests[i], params->SSB_container);
                    sbb->Deposit(reply);
                }

                string req = params->buff->Remove();

                if (req.find("quit") != string::npos) {
                    cout << "Ending the Select Watcher" << endl;
                    quit_count++;
                    cout << "QUIT COUNT " << quit_count << '\n';

                    if (quit_count == 3) {
                        // End the stats bounded buffer
                        for (int n = 0; n < 3; n++) {
                            BoundedBuffer<string> *sbb = lookup(names[n], params->SSB_container);
                            sbb->Deposit("-1");
                        }
                        run = 0;
                        cout << "ENDING\n";
                    }
                } else {
                    string reply = tmp_chan->send_request(req);
                    tmp_chan->cwrite(req);
                    requests[i] = req;
                }
            }
        }
    }

    // Clean up
    cout << "Cleaning up request channels" << endl;

    for (int i = 0; i < params->num_request_channels; ++i) {
        cout << "Quiting" << endl;
        requestChannels[i]->cwrite("quit");
        // reply = requestChannels[i]->cread();
        // cout << reply << endl;
        delete requestChannels[i];
    }
    delete[] requestChannels;
    cout << "Ending Worker Thread\n";
    return 0;
}

BoundedBuffer<string> *lookup(string req, BoundedBuffer<string> **SBB_container)
{
    for (int i = JOHN; i <= JOE; ++i) {
        size_t found = req.find(names[i]);
        if (found != std::string::npos) {
            cout << "Depositing " << req << " into stat buffer for " << names[i] << '\n';
            return SBB_container[i];
        }
    }
    throw range_error("No SBB found");
}

void print_histogram(string name, int data[])
{
    cout << '\n' << "Histogram for " << name << '\n' << '\n';
    for (int i = 0; i < 100; i += 10) {
        cout << setw(7) << "[" << setw(2) << setfill('0') << i << ", " << setw(2) << setfill('0') << i + 9
             << "]: " << setfill(' ');
        for (int j = 0; j < data[i / 10]; ++j) {
            cout << '*';
        }
        cout << '\n';
    }
}

template<typename T>
void clean_up(T *array, int _size)
{
    for (int i = 0; i < _size; ++i) {
        delete array[i];
    }
}
