/**********************************************************************
 *
 * Copyright (C) Momentum Software Corporation, 1997
 *
 * Program    : server.c
 *
 * Description: Receives a message from the client, displays the echoed
 *              message on the console, and echos the message back to the
 *              client. This program must be used together with the client 
 *              program.
 *
 * Usuage     : server <@HostName:InstanceName>
 *
 **********************************************************************/

#include <stdio.h>
#include "xipc.h"

void main(int argc, char **argv)
{
    XINT qid1;
    XINT qid2;
    QIDLIST qlist1;
    QIDLIST qlist2;
    unsigned char buf[80];
    XINT retprio;
    XINT retqid;
    XINT length;

    if (argc != 2)
    {
        printf("server usuage: server <instance name>\n");
        return;
    }


    // Log in to an Xipc instance with user name "server".
    XipcLogin(argv[1], "server");


    // Create 2 message queues named "que1" and "que2".
    qid1 = QueCreate("que1", 1, 128);
    qid2 = QueCreate("que2", 1, 128);


    // Build 2 separate queue ID lists named "qlist1" and "qlist2".
    // qlist1 will have qid1 and qlist2 will have qid2 as the 
    // first elements in their list respectively.
    // qlist1 will be used by the QueReceive operation and
    // qlist2 will be used by the QueSend operation.
    QueListBuild(qlist1, qid1, QUE_EOL);
    QueListBuild(qlist2, qid2, QUE_EOL);

    printf ("server: running.\n");

    while (1)
    {
        // Wait for a msg on que1. When the msg comes in, 
        // display it on the console and send it back on que2.

        length = QueReceive(QUE_Q_EA, qlist1, (ANY *)buf,
                            80, &retprio, &retqid, QUE_WAIT);
        buf[length] = 0;	

        // If the msg is "quit", destroy the queues, logout of
        // the Xipc instance, and exit the program.
    
        printf("server: length = %d | message = %s\n", length, buf);

        if (strncmp(buf, "quit", 4) == 0)
        {
            printf("server: received quit message.\n");
            printf("server: exiting program.\n");
            QueDestroy(qid1);
            QueDestroy(qid2);
            XipcLogout();
            break;
        }

        QueSend(QUE_Q_ANY, qlist2, (ANY *)buf, length, 
                1000, &retqid, QUE_NOWAIT);

    } // end of while (1)

} // end of main()
