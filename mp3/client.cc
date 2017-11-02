#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <zconf.h>
#include "reqchannel.H"

#define MAX_WORKER_THREADS 126

using namespace std;

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

    // DEBUG parsed params
    printf("Requests per Person: %i\n"
                   "Buffer Size: %i\n"
                   "Number of Worker Threads: %i\n", req_per_person, buffer_size, num_worker_threads);



    cout << "CLIENT STARTED:" << endl;

    cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;;

    /* -- Start sending a sequence of requests */

    string reply1 = chan.send_request("hello");
    cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

    string reply2 = chan.send_request("data Joe Smith");
    cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

    string reply3 = chan.send_request("data Jane Smith");
    cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

    string reply5 = chan.send_request("newthread");
    cout << "Reply to request 'newthread' is " << reply5 << "'" << endl;
    RequestChannel chan2(reply5, RequestChannel::CLIENT_SIDE);

    string reply6 = chan2.send_request("data John Doe");
    cout << "Reply to request 'data John Doe' is '" << reply6 << "'" << endl;

    string reply7 = chan2.send_request("quit");
    cout << "Reply to request 'quit' is '" << reply7 << "'" << endl;

    string reply4 = chan.send_request("quit");
    cout << "Reply to request 'quit' is '" << reply4 << "'" << endl;

    usleep(1000000);

}