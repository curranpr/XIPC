import java.io.*;

import com.envoytech.xipc3.*;

public class AddService {
	
	public static void main(String[] args) throws IOException, ClassNotFoundException {
		
		// Create XIPC instance
		Xipc myXIPC = new Xipc();
		
		// XIPC Login
		myXIPC.XipcLogin(args[0], "AddService");
		
		// Create queue for Sa
		int rcvq = myXIPC.QueCreate("Sa", 5, 128);
		
		if(rcvq < 0) {
			rcvq = myXIPC.QueAccess("Sa");
		}


		// Get qid of requester queue
		int sndq = myXIPC.QueAccess("rQ");
		
		if (sndq < 0) {
			sndq = myXIPC.QueCreate("rQ", 5, 128);
		}
			
		
		// Build qidlist for rQ
		QidList qListRcv = myXIPC.QueListBuild(rcvq);

		// Build qidlist for Sa
		QidList qListSend = myXIPC.QueListBuild(sndq);


		System.out.println("AddService running: ");
		
//		byte buf[] = new byte[256];
//		XintRef retprio = new XintRef(0);
//		XintRef retqid = new XintRef(0);
//		
//		int length = myXIPC.QueReceive(Xipc.QUE_Q_EA, qListRcv, buf,
//				 80, retprio, retqid,
//				 Xipc.QUE_WAIT);
//		
//		// Create ByteArrayInputStream and ObjectInputStream
//		ByteArrayInputStream bais1 = new ByteArrayInputStream(buf);
//		ObjectInputStream ois1 = new ObjectInputStream(bais1);
//		
//		// Print out attributes of object
//		programObj obj2 = (programObj)(ois1.readObject());
//		System.out.println("qid: " + obj2.qid);
//		System.out.println("op1: " + obj2.op1);
//		System.out.println("op2: " + obj2.op2);
//		
////		int additionRes = obj2.op1 + obj2.op2;
//		System.out.println("Old result: " + obj2.res);
//		System.out.println("Result: " +  additionRes);
		
		
		
//		while (true) {
//			// Echo message
//			byte buf[]		 = new byte[80];
//			XintRef retprio = new XintRef(0);
//			XintRef retqid  = new XintRef(0);
//			int length		 = myXIPC.QueReceive(Xipc.QUE_Q_EA, qListRcv, buf,
//													 80, retprio, retqid,
//													 Xipc.QUE_WAIT);
//
//			// If the msg is "quit", destroy the queues, logout of
//			// the Xipc instance, and exit the program.
//
//			String msg = new String(buf, 0, 0, length);
//			System.out.println("AddService received" + " message = " + msg);
//			
//			// Send message back
//
//			myXIPC.QueSend(Xipc.QUE_Q_ANY, qListSend, buf, length,
//							 1000, retqid, Xipc.QUE_NOWAIT);
//		} // end of while true
		
		
	}
	
}
