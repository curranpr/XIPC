public class xipcobj implements java.io.Serializable {
	
	private static final long serialVersionUID = 1L;
	int qid;
	int op1;
	int op2;
	int res;
	
	public xipcobj(int qid, int op1, int op2, int res) {
		this.qid = qid;
		this.op1 = op1;
		this.op2 = op2;
		this.res = res;
	}
}
