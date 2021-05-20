/**********************************************************************
 *
 * Copyright (C) Momentum Software Corporation, 1997
 *
 * Program    : client.c
 *
 * Description: Sends a message to the server and displays the echoed
 *              message on the console. This program must be used 
 *              together with the server program.
 *
 * Usuage     : client <@HostName:InstanceName>
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
    unsigned char outbuffer[80];
    unsigned char inbuffer[80];
    XINT length;
    XINT retqid;
    XINT retprio;

    if (argc != 2)
    {
        printf("client usuage: client <instance name>");
        return;
    }


    // Log in to an Xipc instance with user name "client".
    XipcLogin(argv[1], "client");


    // The server should have created 2 queues named "que1" and
    // "que2". Get the queue IDs of these queues.
    qid1 = QueAccess("que1");
    qid2 = QueAccess("que2");


    // Build 2 separate queue ID lists named "qlist1" and "qlist2".
    // qlist1 will have qid1 and qlist2 will have qid2 as the 
    // first elements in their lists respectively.
    // qlist1 will be used by the QueSend operation and
    // qlist2 will be used by the QueReceive operation.
    QueListBuild(qlist1, qid1, QUE_EOL);
    QueListBuild(qlist2, qid2, QUE_EOL);

    printf("client: type 'quit' to exit program.\n");

    while (1)
    {
        // Get a message from the user.

        gets ((char *)outbuffer);
        length = strlen(outbuffer);

        if (length > 0)
        {
            // Send the message to the server.

            QueSend(QUE_Q_ANY, qlist1, (ANY *)outbuffer, length, 
                    1000, &retqid, QUE_NOWAIT);


            // If msg is "quit", exit the program.

            if (strncmp(outbuffer, "quit", 4) == 0)
            {
                XipcLogout();
                break;
            }

            // Wait for a message from the server.
		
            length = QueReceive(QUE_Q_EA, qlist2, (ANY *)inbuffer, 80, 
                                &retprio, &retqid, QUE_WAIT);
            inbuffer[length] = 0;

            // Display the echoed message on the console. 
            printf("client: length = %d | message = %s\n\n", length, inbuffer);

        } // end of if (length > 0)

    } // end of while (1)

} // end of main()
