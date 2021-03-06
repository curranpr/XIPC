/**********************************************************************
 *
 * Copyright (C) Envoy Technologies Inc., 2001
 *
 * Program	  : server.java
 *
 * Description: Receives a message from the client, displays the echoed
 * 				 message on the console, and echos the message back to
 * 				 the client. This program must be used together with
 * 				 the client program.
 *
 * Usuage	  : java server <@HostName:InstanceName>
 *
 **********************************************************************/

import javaxipc30.*;


class server
{
	 public static void main(String args[])
	 {
		  // Instantiate an Xipc instance.
		  Xipc x = new Xipc();


		  if (args.length != 1)
		  {
				System.out.println("server usuage: java server <instance name>");
				return;
		  }


		  // Log in to an Xipc instance with user name "server".
		  x.XipcLogin(args[0], "server");


		  // Create 2 message queues named "que1" and "que2".
		  int qid1 = x.QueCreate("que1", 1, 128);
		  int qid2 = x.QueCreate("que2", 1, 128);


		  // Build 2 separate queue ID lists named "qlist1" and "qlist2".
		  // qlist1 will have qid1 and qlist2 will have qid2 as the
		  // first elements in their list respectively.
		  // qlist1 will be used by the QueReceive operation and
		  // qlist2 will be used by the QueSend operation.
		  QidList qlist1 = Xipc.QueListBuild(qid1);
		  QidList qlist2 = Xipc.QueListBuild(qid2);

		  System.out.println("server: running.");

		  while (true)
		  {
				// Wait for a msg on que1. When the msg comes in,
				// display it on the console and send it back on que2.

				byte buf[]		 = new byte[80];
				XintRef retprio = new XintRef(0);
				XintRef retqid  = new XintRef(0);
				int length		 = x.QueReceive(Xipc.QUE_Q_EA, qlist1, buf,
														 80, retprio, retqid,
														 Xipc.QUE_WAIT);

				// If the msg is "quit", destroy the queues, logout of
				// the Xipc instance, and exit the program.

				String msg = new String(buf, 0, 0, length);
				System.out.println("server: length = " + length +
										 " | message = " + msg);

				if (msg.equals("quit"))
				{
					 System.out.println("server: received quit message.");
					 System.out.println("server: exiting program.");
					 x.QueDestroy(qid1);
					 x.QueDestroy(qid2);
					 x.XipcLogout();
					 break;
				}

				x.QueSend(Xipc.QUE_Q_ANY, qlist2, buf, length,
				1000, retqid, Xipc.QUE_NOWAIT);

		  } // end of while (true)

	 } // end of main()

} // end of class server


