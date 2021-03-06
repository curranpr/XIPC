import java.io.*;

import com.envoytech.xipc3.*;

public class AddService {
	
	public static void main(String[] args) {
		
		// Create XIPC instance
		Xipc myXIPC2 = new Xipc();
		
		// XIPC Login
		myXIPC2.XipcLogin(args[0], "AddService");
		
		// Create queue for Sa
		int rcvq = myXIPC.QueCreate("Sa", 1, 128);


		// Get qid of requester queue
		int sndq = myXIPC2.QueAccess("rQ");
		
		// Build qidlist for rQ
		QidList qListRcv = myXIPC2.QueListBuild(rcvq);

		// Build qidlist for Sa
		QidList qListSend = myXIPC2.QueListBuild(sndq);


		System.out.println("AddService running: ");
		
		// Echo message
		byte buf[]		 = new byte[80];
		XintRef retprio = new XintRef(0);
		XintRef retqid  = new XintRef(0);
		int length		 = myXIPC2.QueReceive(Xipc.QUE_Q_EA, qListRcv, buf,
												 80, retprio, retqid,
												 Xipc.QUE_WAIT);

		// If the msg is "quit", destroy the queues, logout of
		// the Xipc instance, and exit the program.

		String msg = new String(buf, 0, 0, length);
		System.out.println("AddService received" + " message = " + msg);
		
		// Send message back

		myXIPC2.QueSend(Xipc.QUE_Q_ANY, qListSend, outbuffer, length,
						 1000, retqid, Xipc.QUE_NOWAIT);
		
	}
	
}
