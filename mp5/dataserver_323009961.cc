/*
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/06/16

    Dataserver main program for MPs in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "netreqchannel.H"
// #include "reqchannel.H"
#include <cassert>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void *handle_process_loop(void *args);
void *temp_func(void *) {}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number)
{
    stringstream ss;    // create a stringstream
    ss << number;       // add number to the stream
    return ss.str();    // return a string with the contents of the stream
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

void *handle_data_requests(void *args)
{
    // NetReqChannel *data_channel = (NetReqChannel *) args;

    // -- Handle client requests on this channel.

    handle_process_loop(args);

    // -- Client has quit. We remove channel.

    // delete data_channel;
    return 0;
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(int new_fd, __attribute__((unused)) const string &_request)
{
    const char *_msg = "hello to you too";
    write(new_fd, _msg, strlen(_msg));
}

void process_data(int new_fd, __attribute__((unused)) const string &_request)
{
    usleep(1000 + (rand() % 5000));
    // _channel.cwrite("here comes data about " + _request.substr(4) + ": " + int2string(random() % 100));

    string _msg = int2string(rand() % 100);
    write(new_fd, _msg.c_str(), strlen(_msg.c_str()));
}

// void process_newthread(RequestChannel &_channel, __attribute__((unused)) const string &_request)
// {
//     int error;
//     nthreads++;

//     // -- Name new data channel

//     string new_channel_name = "data" + int2string(nthreads) + "_";
//     //  cout << "new channel name = " << new_channel_name << endl;

//     // -- Pass new channel name back to client

//     _channel.cwrite(new_channel_name);

//     // -- Construct new data channel (pointer to be passed to thread function)

//     RequestChannel *data_channel = new RequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

//     // -- Create new thread to handle request channel

//     pthread_t thread_id;
//     //  cout << "starting new thread " << nthreads << endl;
//     if ((error = pthread_create(&thread_id, NULL, handle_data_requests, data_channel))) {
//         fprintf(stderr, "p_create failed: %s\n", strerror(error));
//     }
// }

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(int new_fd, const string &_request)
{
    if (_request.compare(0, 5, "hello") == 0) {
        process_hello(new_fd, _request);
    } else if (_request.compare(0, 4, "data") == 0) {
        process_data(new_fd, _request);
        // } else if (_request.compare(0, 9, "newthread") == 0) {
        //     process_newthread(_channel, _request);
    } else {
        const char *notKnown = "unknown request";
        write(new_fd, notKnown, strlen(notKnown));
    }
}

void *handle_process_loop(void *args)
{
    int new_fd = *(int *) args;

    for (;;) {
        char buf[255];
        // cout << "Reading next request from channel (" << _channel.name() << ") ..." << flush;
        read(new_fd, buf, 255);
        // cout << " done (" << _channel.name() << ")." << endl;
        string request = buf;
        cout << "New request is " << request << endl;

        if (request.compare("quit") == 0) {
            const char *bye = "bye";
            write(new_fd, bye, strlen(bye));
            usleep(10000);    // give the other end a bit of time.
            break;            // break out of the loop;
        }

        process_request(new_fd, request);
    }
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    // 2 params, -p port_num, -b backlog (listen function?) size of buffer
    //  cout << "Establishing control channel... " << flush;
    int c           = 0;     // Value for temp storage of param
    int backlog     = 10;    // maybe 195
    string port_num = "12005";

    while ((c = getopt(argc, argv, "p:b:")) != -1) {
        switch (c) {
        case 'p':
            port_num = strtoul(optarg, NULL, 10);
            break;
        default:
            break;
        }
    }
    NetReqChannel nrw = NetReqChannel(port_num, backlog, handle_process_loop);
    cout << "done.\n" << flush;

    // handle_process_loop(control_channel);
}
