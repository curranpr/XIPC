import java.io.IOException;

import com.envoytech.xipc3.*;
//import com.fasterxml.jackson.databind.ObjectMapper;

public class Main {
	
	@SuppressWarnings("deprecation")
	public static void main(String[] args) throws IOException {
		
		// XIPC instance
		Xipc myXIPC = new Xipc();
		
		// XIPC Login
		myXIPC.XipcLogin(args[0], "Requester");
		
		// Create 1 queue (Requester queue)
		int rcvq = myXIPC.QueCreate("rQ", 5, 128);
		
		if (rcvq < 0) {
			rcvq = myXIPC.QueAccess("rQ");
		}
		
		// Access Addition Service queue
		int sndq = myXIPC.QueAccess("Sa");
		
		if(sndq < 0) {
			sndq = myXIPC.QueCreate("Sa", 5, 128);
		}

		// Create 1 QidList for rQ
		QidList qListRcv = myXIPC.QueListBuild(rcvq);
		
		// Create 1 QidList for Sa
		QidList qListSend = myXIPC.QueListBuild(sndq);

		System.out.println("Requester running: ");
		
		// Data Input Stream
		//DataInputStream inputStream = new DataInputStream(System.in);
		
		// Create object
		//xipcobj obj1 = new xipcobj(sndq, 2,2,0);
		
		// JSON ObjectMapper
		//ObjectMapper objMapper = new ObjectMapper();
		
		// Object to JSON string
		//String jsonStr = objMapper.writeValueAsString(obj1);
		
		// Print JSON string
		//System.out.println("JSON string");
		//System.out.println(jsonStr);
		
//		// try catch serialize
//		try {
//			// Create ByteArrayOutputStream and ObjectOutputStream
//			ByteArrayOutputStream baos1 = new ByteArrayOutputStream();
//			ObjectOutputStream oos1 = new ObjectOutputStream(baos1);
//			
//			// Serialize
//			oos1.writeObject(obj1);
//			
//			// Get byte data from ByteArrayOutputStream
//			//byte outbuffer[] = new byte[256];
//			//outbuffer = baos1.toByteArray();
//			byte outbuffer[] = baos1.toByteArray();
//			System.out.println("Length b/a" + outbuffer.length);
//			
//			XintRef retqid = new XintRef(0);
//			
//			myXIPC.QueSend(Xipc.QUE_Q_ANY, qListSend, outbuffer, outbuffer.length, 1000, retqid, Xipc.QUE_NOWAIT);
//			
//		}
//		catch(Exception e) {
//			System.out.println("Exception- " + e);
//		}
		
//		while (true) {
//			// Sending data
//			byte outbuffer[] = new byte[80];
//			int length = 0;
//			String userInput = "";
//
//			// Get a message from the user.
//			userInput = inputStream.readLine();
//			userInput.getBytes(0, userInput.length(), outbuffer, 0);
//			length = userInput.length();
//			
//			XintRef retqid = new XintRef(0);
//
//			 myXIPC.QueSend(Xipc.QUE_Q_ANY, qListSend, outbuffer, length,
//							 1000, retqid, Xipc.QUE_NOWAIT);
//			
//			
//			// Echo message sent back from AddService
//
//			// Echo message
//			byte buf[]		 = new byte[80];
//			XintRef retprio = new XintRef(0);
//			length		 = myXIPC.QueReceive(Xipc.QUE_Q_EA, qListRcv, buf,
//													 80, retprio, retqid,
//													 Xipc.QUE_WAIT);
//			
//			String msg = new String(buf, 0, 0, length);
//			System.out.println("From m/s received msg back- " + msg);
//			
//			
//		} // end of while true
		
	} // end of main
	
}