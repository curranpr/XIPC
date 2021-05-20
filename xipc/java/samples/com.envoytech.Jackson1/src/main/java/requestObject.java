public class requestObject implements java.io.Serializable {

	int qId;
	int op1;
	int op2;
	int res;
	
	public requestObject(int qId, int op1, int op2, int res) {
		this.qId = qId;
		this.op1 = op1;
		this.op2 = op2;
		this.res = res;
	}
	
}
