
#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

/*
 * The commands supported by the server
 */
typedef enum _Command {
    CmdAdd = 0x00,     // Adds the two message parameters
    CmdQuit            // Stops the server
} Command;

/*
 * The message format to be sent to the server.
 */
typedef struct _Message {
    /*
     * One of the command constants.
     */
    Command command;
    /*
     * Used as operand 1 (if required)
     */
    int parameter1;
    /*
     * Used as operand 2 (if required)
     */
    int parameter2;
} Message;

#define FORMAT_STRING_ADD      "Calc: %d + %d = %d\n"
#define FORMAT_STRING_SUB      "Calc: %d - %d = %d\n"
#define FORMAT_STRING_MUL      "Calc: %d * %d = %d\n"
#define FORMAT_STRING_DIV      "Calc: %d / %d = %d\n"

mqd_t startClient(void)
{
    // TODO: Open the message queue previously created by the server
    mqd_t client = mq_open(QUEUE_NAME, O_WRONLY);
    if (client == -1) {
        perror("startClient: mq_open failed");
        return -1;
    }

    return client;
}

int sendQuitTask(mqd_t client)
{
    // (void)client;
    // TODO: Send the exit command to the server.
    Message message;
    message.command = CmdQuit;
    message.parameter1 = 0;
    message.parameter2 = 0;

    return mq_send(client, (char*)&message, sizeof(message), 0);
}

int sendAddTask(mqd_t client, int operand1, int operand2)
{
    // (void)client;
    // (void)operand1;
    // (void)operand2;

    // TODO: Send the add command with the operands
    Message message;
    message.command = CmdAdd;
    message.parameter1 = operand1;
    message.parameter2 = operand2;
    
    return mq_send(client, (char*)&message, sizeof(message), 0);
}

int stopClient(mqd_t client)
{
    // (void)client;

    // TODO: Clean up anything on the client-side
    return mq_close(client);
}

int runServer(void)
{
    int didQuit = 0, hadError = 0;
    Message msg;

    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;           // Maximum number of messages in the queue
    attr.mq_msgsize = sizeof(msg);  // Maximum message size
    attr.mq_curmsgs = 0;
    (void) attr;

    // TODO:
    // Create and open the message queue. Server only needs to read from it.
    // Clients only need to write to it, allow for all users.
    mqd_t server = mq_open(QUEUE_NAME, O_EXCL | O_CREAT | O_RDONLY, 0222, &attr);
    if(server == -1) {
	    return -1;
    }

    // This is the implementation of the server part, already completed:
    // TODO: You may have to make minor extensions in order to satisfy all requirements
    do {
        // Attempt to receive a message from the queue.
        ssize_t received = mq_receive(server, (char*)&msg, sizeof(msg), NULL);
        if (received != sizeof(msg)) {
            // This implicitly also checks for error (i.e., -1)
            hadError = 1;
            continue;
        }

        switch (msg.command)
        {
            case CmdQuit:
                // End this loop.
                didQuit = 1;
                break;

            case CmdAdd:
                // Print the required output.
                printf(FORMAT_STRING_ADD,
                       msg.parameter1,
                       msg.parameter2,
                       msg.parameter1 + msg.parameter2);
                break;

            default:
                hadError = 1;
                break;
        }
    } while (!didQuit && !hadError);

    // TODO
    // Close the message queue on exit and unlink it

    // closes your process's connection to the queue (like closing a file)

    if (mq_close(server) == -1) { 
        perror("mq_close failed");
        hadError = 1;
    }
    // removes the queue from the system (like deleting a file)
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink failed");
        hadError = 1;
    }

    return hadError ? -1 : 0;
}
