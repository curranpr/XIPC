import java.io.*;
import com.envoytech.xipc3.*;

public class Requester {
	
	@SuppressWarnings("deprecation")
	public static void main(String[] args) throws IOException {
		
		// XIPC instance
		Xipc myXIPC = new Xipc();
		
		// XIPC Login
		myXIPC.XipcLogin(args[0], "Requester");
		
		// Create 1 queue (Requester queue)
		int rcvq = myXIPC.QueCreate("rQ", 1, 128);
		
		// Access Addition Service queue
		int sndq = myXIPC.QueAccess("Sa");

		// Create 1 QidList for rQ
		QidList qListRcv = myXIPC.QueListBuild(rcvq);
		
		// Create 1 QidList for Sa
		QidList qListSend = myXIPC.QueListBuild(sndq);

		System.out.println("Requester running: ");
		
		// Data Input Stream
		DataInputStream inputStream = new DataInputStream(System.in);
		
		// Sending data
		byte outbuffer[] = new byte[80];
		int length = 0;
		String userInput = "";

		// Get a message from the user.
		userInput = inputStream.readLine();
		userInput.getBytes(0, userInput.length(), outbuffer, 0);
		length = userInput.length();
		
		XintRef retqid = new XintRef(0);

		 myXIPC.QueSend(Xipc.QUE_Q_ANY, qListSend, outbuffer, length,
						 1000, retqid, Xipc.QUE_NOWAIT);
		
		
		// Echo message sent back from AddService

		// Echo message
		byte buf[]		 = new byte[80];
		XintRef retprio = new XintRef(0);
		length		 = myXIPC.QueReceive(Xipc.QUE_Q_EA, qListRcv, buf,
												 80, retprio, retqid,
												 Xipc.QUE_WAIT);
	}
	
}
