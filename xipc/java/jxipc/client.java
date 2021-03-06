/**********************************************************************
 *
 * Copyright (C) Envoy Technologies Inc., 2001
 *
 * Program	  : client.java
 *
 * Description: Sends a message to the server and displays the echoed
 * 				 message on the console. This program must be used
 * 				 together with the server program.
 *
 * Usuage	  : java client <@HostName:InstanceName>
 *
 **********************************************************************/

import java.io.*;
import com.envoytech.xipc3.*;


class client
{
	 @SuppressWarnings("deprecation")
	public static void main(String args[]) throws IOException
	 {
		  // Instantiate an Xipc instance.
		  Xipc x = new Xipc();

		  if (args.length != 1)
		  {
				System.out.println("client usuage: java client <instance name>");
				return;
		  }


		  // Log in to an Xipc instance with user name "client".
		  x.XipcLogin(args[0], "client");


		  // The server should have created 2 queues named "que1" and
		  // "que2". Get the queue IDs of these queues.
		  int qid1 = x.QueAccess("que1");
		  int qid2 = x.QueAccess("que2");


		  // Build 2 separate queue ID lists named "qlist1" and "qlist2".
		  // qlist1 will have qid1 and qlist2 will have qid2 as the
		  // first elements in their lists respectively.
		  // qlist1 will be used by the QueSend operation and
		  // qlist2 will be used by the QueReceive operation.
		  QidList qlist1 = Xipc.QueListBuild(qid1);
		  QidList qlist2 = Xipc.QueListBuild(qid2);

		  System.out.println("client: type 'quit' to exit program.");

		  DataInputStream is = new DataInputStream(System.in);

		  while (true)
		  {
				byte outbuffer[] = new byte[80];
				int length = 0;
				String userInput = "";

				// Get a message from the user.
				userInput = is.readLine();
				userInput.getBytes(0, userInput.length(), outbuffer, 0);
				length = userInput.length();

				if (length > 0)
				{

					 // Send the message to the server.
					 XintRef retqid = new XintRef(0);

					 x.QueSend(Xipc.QUE_Q_ANY, qlist1, outbuffer, length,
									 1000, retqid, Xipc.QUE_NOWAIT);


					 // If msg is "quit", exit the program.
					 if (userInput.equals("quit"))
					 {
						  x.XipcLogout();
						  break;
					 }


					// Wait for a message from the server.

					byte inbuffer[] = new byte[80];
					XintRef retprio = new XintRef(0);

					length = x.QueReceive(Xipc.QUE_Q_EA, qlist2, inbuffer,
												 80, retprio, retqid,
												 Xipc.QUE_WAIT);


					// Display the echoed message on the console.

					System.out.println("client: length = " + length +
											 " | message = " + new String(inbuffer, 0));

				} // end of if (length > 0)

		  } // end of while (true)

	 } // end of method main()

} // end of class client


