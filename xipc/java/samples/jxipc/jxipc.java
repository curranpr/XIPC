/*--------------------------------- jxipc.java ---------------------
*
*	Name: jxipc.java
*	----
*
*	Title:	jxipc - java interactive xipc command processor application
*	-----
*
*	Description:
*	-----------
*	Interactive command processor application
*
*
*	Last Revision:
*	-------------
*	$Revision:	$
*	$Date:		$
*	$Author: $
*
*	Revision History:
*	----------------
*
*	Rev	Date		By  Description
*	---	-------- ---  -----------
*
*
* Copyright (C) Envoy Technologies Inc., 2001
*
*-------------------------------------------------------------------*/

import javaxipc30.*;
import java.util.*;
import java.io.*;



 /*-------------------------------------------------------*
  *	class parseError is thrown by the parsing routines
  *-------------------------------------------------------*/

class parseError extends Exception	// thrown by parse methods
{
	String str;
	parseError(String str)	{ this.str = str; }
}



 /*-------------------------------------------------------------------*
  *	class tokenizer takes a string and returns an object with methods
  * for breaking a line of text into simple tokens for the parser
  *-------------------------------------------------------------------*/

class tokenizer
{
	StringTokenizer stk;

	String next;				// the last token obtained with a getNextToken method
	boolean pushed = false; 	// allows 'pushing' that token, so it will be returned once again


	//*  constructor for the class (uses java.util.StringTokenizer class)
	tokenizer(String str)
	{
		stk = new StringTokenizer(str , " ,()\"'\t" , true);
	}


	//*  this method returns next non-blank token, where a token here is any one of the 5 characters
	//*  comma, right paren, left paren, double quote and single quote, i.e.==>  ,()"'
	//*  ADDED: 6th special character $ (dollar sign)
	//*  or any contiguous string of characters containing none of those 5, nor any blank characters
	private String getNextNonBlankToken() throws parseError
	{
		String str;
		boolean more;
		do
		{	more = stk.hasMoreTokens();
			if (more) str = stk.nextToken(" ,()\"'$\t");
			else throw new parseError("unexpected end-of-line");
		} while ( more && (str.equals(" ") || str.equals("\t")) );
		return str;
	}


	//*  the method getNextToken returns next token, where a token is:
	//*
	//*  1. any of the 3 characters:  comma,	open paren,  close paren, i.e.==>  ,()
	//*  2. a double quoted string such as:	"this is a string"   (returned without the quotes)
	//*  3. a single quoted character, as: 'a' or ' '        (returned with the quotes)
	//*  4. any contiguous sequence of chars other than: comma, parens, quotes (doube or single) and blanks
	//*  blanks are seperators (not tokens) except as single quoted chars and within strings
	//*  strings are returned without quotes	:	 "a b" (5 chars)  ==> a b (3 chars)
	//*  quoted chars are returned with quotes:	'a' (3 chars)  ==> 'a' (3 chars)
	//*  NOTE: "'a'" and 'a' return the same string - quoted single chars were added for the sake of MEM_FILL
	//*  option of MemWrite, and it's a nuisance.
	String getNextToken() throws parseError
	{
		if (pushed)
		{
			pushed = false;
			return next;
		}
		String tmp = getNextNonBlankToken();
		if (tmp.equals("\""))
		{
			if (!stk.hasMoreTokens()) throw new parseError("unmatched double quote character");
			next = stk.nextToken("\"");
			if (next.equals("\"")) return "";   // null string;
			if (!stk.hasMoreTokens()) throw new parseError("unmatched double quote character");
			if (!stk.nextToken(" ,()\"'$").equals("\"")) throw new parseError("bad sanity check");
		}
		else if (tmp.equals("'"))
		{
			if (!stk.hasMoreTokens()) throw new parseError("unmatched single quote character");
			next = stk.nextToken("'");
			if (next.length()>1) throw new parseError("single quotes should enclose a single char");
			if (!stk.hasMoreTokens()) throw new parseError("unmatched single quote character");
			if (!stk.nextToken(" ,()\"'$").equals("'")) throw new parseError("bad sanity check");
			next = new String("'" + next + "'");
		}
		else next = tmp;
		return next;
	}


	//*  getNextTokenOrEOL will not throw an error if called at the end of a line - for instance,
	//*  parseSidList ends by getting a token that is then pushed.  If the SidList is legitimately
	//*  the last thing on the line, we don't want that last get (which checks for a comma) to throw
	//*  an error at the end of a line
	String getNextTokenOrEOL() throws parseError
	{
		if (!stk.hasMoreTokens()) return "";   // empty string won't equal anything we'll check for
		else return getNextToken();
	}


	//* push pushes a single token (the last token) back
	void push() throws parseError
	{
		if (pushed) throw new parseError("parser pushed twice"); // this should not be done
		else pushed = true;
	}

}	// end class tokenizer




 /*-----------------------------------------------------------------------------------------*
  *	class lineparser does higher level parsing of a line
  * The methods throw a parseError if they don't find what they are supposed to in the text,
  * otherwise they return some java representation of the text
  *-----------------------------------------------------------------------------------------*/

class lineparser
{
	tokenizer tk;

	lineparser(String str)	// constructor
	{
		tk = new tokenizer(str);  // defined above
	}


	int parseInt() throws parseError
	{
		String next = tk.getNextToken();
		try { return Integer.parseInt(next); }
		catch (NumberFormatException e) { throw new parseError("expected an integer, got " + next); }
	}

	int parseIntOrStar() throws parseError 	// for quecopy length param
	{
		String next = tk.getNextToken();
		if (next.equals("*")) return -1;
		try { return Integer.parseInt(next); }
		catch (NumberFormatException e) { throw new parseError("expected an integer or '*', got " + next); }
	}


	//*  parseToken() parses any token at all
	String parseToken() throws parseError
	{
		return tk.getNextToken();
	}

	//*  parseToken(String str) checks that what it gets is what was expected, namely String str
	void parseToken(String t) throws parseError
	{
		String next = tk.getNextToken();
		if (!next.equals(t)) throw new parseError("expected \"" + t + "\" , got \"" + next + "\"");
	}

	//*  this parses a length specified, comma seperated, tuple; ex (12, 34,56) where n=3
	//*  returns an int array of specified length
	int[] parseCommaIntTuple(int n) throws parseError
	{
		int[] retval = new int[n];
		parseToken("(");
		for (int i=0; i<n-1; i++)
		{
			retval[i]=parseInt();
			parseToken(",");
		}
		retval[n-1]=parseInt();
		parseToken(")");
		return retval;
	}

	//*  length specified, whitespace seperated, tuple; ex (12 34	56) where n=3
	int[] parseIntTuple(int n) throws parseError
	{
		int[] retval = new int[n];
		parseToken("(");
		for (int i=0; i<n; i++)
		{
			retval[i]=parseInt();
		}
		parseToken(")");
		return retval;
	}

	//* ex: 12,34,56,78
	int[] parseIntList() throws parseError
	{
		int[] tmp = new int[50];
		int count=0;
		String str = ",";
		while (str.equals(","))
		{
			tmp[count] = parseInt();
			count++;
			str = tk.getNextTokenOrEOL();
		}
		tk.push();	// we took something that's not part of the IntList, and had better put it back
		int[] retval = new int[count];
		for (int i=0; i<count; i++) retval[i]=tmp[i];
		return retval;
	}


	QidList parsePlainQidList() throws parseError
	{
		int[] qidarr = parseIntList();
		int len = qidarr.length;
		QidList retval = new QidList();
		for (int i=0; i<len; i++)
		{
			retval = Xipc.QueListAdd(retval,qidarr[i]);
			if (retval==null) throw new parseError("QidList too long");
		}
		return retval;
	}


	QidList parseMSCQidList() throws parseError
	{
		String next = ",";
		QidList retval = new QidList();
		int[] parm;
		int count=0;
		while (next.equals(","))
		{
			next = tk.getNextToken();
			if (next.equals("ea"))
			{	parm=parseCommaIntTuple(1);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_EA(parm[0]));
			}
			else if (next.equals("la"))
			{	parm=parseCommaIntTuple(1);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_LA(parm[0]));
			}
			else if (next.equals("hp"))
			{	parm=parseCommaIntTuple(1);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_HP(parm[0]));
			}
			else if (next.equals("lp"))
			{	parm=parseCommaIntTuple(1);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_LP(parm[0]));
			}
			else if (next.equals("preq"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PREQ(parm[0],parm[1]));
			}
			else if (next.equals("prne"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PRNE(parm[0],parm[1]));
			}
			else if (next.equals("prgt"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PRGT(parm[0],parm[1]));
			}
			else if (next.equals("prge"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PRGE(parm[0],parm[1]));
			}
			else if (next.equals("prlt"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PRLT(parm[0],parm[1]));
			}
			else if (next.equals("prle"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PRLE(parm[0],parm[1]));
			}
			else if (next.equals("prrng"))
			{	parm=parseCommaIntTuple(3);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_PRRNG(parm[0],parm[1],parm[2]));
			}
			else if (next.equals("seqeq"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_SEQEQ(parm[0],parm[1]));
			}
			else if (next.equals("seqgt"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_SEQGT(parm[0],parm[1]));
			}
			else if (next.equals("seqge"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_SEQGE(parm[0],parm[1]));
			}
			else if (next.equals("seqlt"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_SEQLT(parm[0],parm[1]));
			}
			else if (next.equals("seqle"))
			{	parm=parseCommaIntTuple(2);
				retval=Xipc.QueListAdd(retval, Xipc.QUE_M_SEQLE(parm[0],parm[1]));
			}
			else try
			{
				retval=Xipc.QueListAdd(retval, Integer.parseInt(next));
			}
			catch (NumberFormatException e) { throw new parseError(next + " in QidList"); }
			if (retval==null) throw new parseError("QidList too long");
			next = tk.getNextTokenOrEOL();
			count++;
		}	// end while(next.equals(","))
		tk.push();	// token after last Qid was not a comma - put it back
		return retval;
	}


	SidList parseSidList() throws parseError
	{
		int[] sidarr = parseIntList();
		int len = sidarr.length;
		SidList retval = new SidList();
		for (int i=0; i<len; i++)
		{
			retval = Xipc.SemListAdd(retval,sidarr[i]);
			if (retval==null) throw new parseError("SidList too long");
		}

		return retval;
	}


	MidList parseMidList() throws parseError
	{
		MidList retval = new MidList();
		String next = ",";
		int count=0;
		while (next.equals(","))
		{
			retval = Xipc.MemListAdd(retval, parseSection());
			if (retval==null) throw new parseError("MidList too long");
			next = tk.getNextTokenOrEOL();
			count++;
		}
		tk.push();	// token after last section was not a comma - put it back
		return retval;
	}


	Section parseSection() throws parseError
	{
		String tmp = tk.getNextToken();
		if (tmp.equals("("))
		{
			tk.push();
			int[] sec = parseIntTuple(3);
			return Xipc.MemSectionBuild(sec[0], sec[1], sec[2]);
		}
		else
		{
			Section retval = (Section)jxipc.sectionTable.get(tmp);
			if (retval==null)
				throw new parseError("variable \"" + tmp + "\" has not been assigned a section (type: ? memsection for help)");
			else return retval;
		}
	}


	MsgHdr parseExistingMsgHdr() throws parseError
	{
		String tmp = tk.getNextToken();
		if ("abcdefghijklmnopqrstuvwxyz".indexOf(tmp.charAt(0))<0)
			throw new parseError("variable \""+ tmp + "\" : MsgHdr variable should begin with lowercase letter");
		MsgHdr retval = (MsgHdr)jxipc.msgHdrTable.get(tmp);
		if (retval==null)
			throw new parseError("variable \"" + tmp + "\" has not been assigned a MsgHdr");
		else return retval;
	}


	MsgHdr parseMsgHdr() throws parseError
	{
		String tmp = tk.getNextToken();
		if ("abcdefghijklmnopqrstuvwxyz".indexOf(tmp.charAt(0))<0)
			throw new parseError("variable \""+ tmp + "\" : MsgHdr variable should begin with lowercase letter");
		MsgHdr retval = (MsgHdr)jxipc.msgHdrTable.get(tmp);
		if (retval==null)
		{
			retval = new MsgHdr();
			jxipc.msgHdrTable.put(tmp,retval);
		}
		return retval;
	}


	int parseAnyAllAtomic(String sys) throws parseError
	{
		if (!sys.equals("mem")  &&  !sys.equals("sem"))
			throw new parseError("parseAnyAllAtomic called with sys = " + sys);
		String str = tk.getNextToken();
		if(str.equals("any"))
		{
			if (sys.equals("mem")) return Xipc.MEM_ANY;
			else return Xipc.SEM_ANY;
		}
		else if (str.equals("all"))
		{
			if (sys.equals("mem")) return Xipc.MEM_ALL;
			else return Xipc.SEM_ALL;
		}
		else if (str.equals("atomic"))
		{
			if (sys.equals("mem")) return Xipc.MEM_ATOMIC;
			else return Xipc.SEM_ATOMIC;
		}
		else throw new parseError("expected one of  any|all|atomic  , got " + str);
	}


	int parseWaitNoWaitTimeOut(String sys) throws parseError
	{
		if (!sys.equals("mem")  &&  !sys.equals("sem") &&  !sys.equals("que"))
			throw new parseError("parseWaitNoWaitTimeOut called with sys = " + sys);
		String str = tk.getNextToken();
		if(str.equals("wait"))
		{
			if (sys.equals("mem")) return Xipc.MEM_WAIT;
			if (sys.equals("que")) return Xipc.QUE_WAIT;
			else return Xipc.SEM_WAIT;
		}
		else if (str.equals("nowait"))
		{
			if (sys.equals("mem")) return Xipc.MEM_NOWAIT;
			if (sys.equals("que")) return Xipc.QUE_NOWAIT;
			else return Xipc.SEM_NOWAIT;
		}
		else if (str.equals("timeout"))
		{
			parseToken("(");
			int t=parseInt();
			parseToken(")");
			if (sys.equals("mem")) return Xipc.MEM_TIMEOUT(t);
			if (sys.equals("que")) return Xipc.QUE_TIMEOUT(t);
			else return Xipc.SEM_TIMEOUT(t);
		}
		else throw new parseError("expected one of  wait|nowait|timeout(<int>)  , got " + str);
	}


	int parseSemWaitBlockopt() throws parseError
	{
		String str = tk.getNextToken();
		if (str.equals("clear"))
		{
			parseToken(",");
			return Xipc.SEM_CLEAR|parseWaitNoWaitTimeOut("sem");
		}
		else
		{
			tk.push();	// we'll just try again
			return parseWaitNoWaitTimeOut("sem");
		}
	}


	int parseQueGetBlockopt() throws parseError		// also for QueReceive
	{
		String str = tk.getNextToken();
		if (str.equals("noremove"))
		{
			parseToken(",");
			return Xipc.QUE_NOREMOVE|parseWaitNoWaitTimeOut("que");
		}
		else
		{
			tk.push();	// we'll just try again
			return parseWaitNoWaitTimeOut("que");
		}
	}


	int parseQuePutBlockopt() throws parseError		// also for QueSend
	{
		String str = tk.getNextToken();
		if (str.equals("replace_ea"))
			return Xipc.QUE_REPLACE_EA;
		else if (str.equals("replace_la"))
			return Xipc.QUE_REPLACE_LA;
		else if (str.equals("replace_hp"))
			return Xipc.QUE_REPLACE_HP;
		else if (str.equals("replace_lp"))
			return Xipc.QUE_REPLACE_LP;
		else if (str.equals("replicate"))
			return Xipc.QUE_REPLICATE;
		else
		{
			tk.push();	// we'll just try again
			return parseWaitNoWaitTimeOut("que");
		}
	}


	int parseDirection() throws parseError 	// for QueBrowse
	{
		String str = tk.getNextToken();
		if (str.equals("time-"))
			return Xipc.QUE_TIME_NEXT;
		else if (str.equals("time+"))
			return Xipc.QUE_TIME_PREV;
		else if (str.equals("prio-"))
			return Xipc.QUE_PRIO_NEXT;
		else if (str.equals("prio+"))
			return Xipc.QUE_PRIO_PREV;
		else throw new parseError("expected time+|time-|prio+|prio-, got " + str);
	}


	int parsePrivelege() throws parseError 	// for MemSecPriv
	{
		String str = tk.getNextToken();
		if (str.equals("rw")) return Xipc.MEM_RW;
		else if (str.equals("ro")) return Xipc.MEM_RO;
		else if (str.equals("wo")) return Xipc.MEM_WO;
		else if (str.equals("na")) return Xipc.MEM_NA;
		else if (str.equals("+r")) return Xipc.MEM_ADD_R;
		else if (str.equals("+w")) return Xipc.MEM_ADD_W;
		else if (str.equals("-r")) return Xipc.MEM_RMV_R;
		else if (str.equals("-w")) return Xipc.MEM_RMV_W;
		else if (str.equals("nc")) return Xipc.MEM_NC;
		else throw new parseError("expected rw|ro|wo|na|+r|+w|-r|-w|nc , got " + str);
	}


	MemTriggerSpec parseMemTriggerCode() throws parseError		// for MemSecPriv
	{
		String str = tk.getNextToken();
		if (str.equals("read"))
			return Xipc.MEM_T_READ(parseInt(),parseInt(),parseInt());
		else if (str.equals("write"))
			return Xipc.MEM_T_WRITE(parseInt(),parseInt(),parseInt());
		else if (str.equals("lock"))
			return Xipc.MEM_T_LOCK(parseInt(),parseInt(),parseInt());
		else if (str.equals("unlock"))
			return Xipc.MEM_T_UNLOCK(parseInt(),parseInt(),parseInt());
		else if (str.equals("user_read"))
			return Xipc.MEM_T_USER_READ(parseInt(),parseInt(),parseInt(),parseInt());
		else if (str.equals("user_write"))
			return Xipc.MEM_T_USER_WRITE(parseInt(),parseInt(),parseInt(),parseInt());
		else if (str.equals("user_lock"))
			return Xipc.MEM_T_USER_LOCK(parseInt(),parseInt(),parseInt(),parseInt());
		else if (str.equals("user_unlock"))
			return Xipc.MEM_T_USER_UNLOCK(parseInt(),parseInt(),parseInt(),parseInt());
		else if (str.equals("pool_high"))
			return Xipc.MEM_T_POOL_HIGH(parseInt());
		else if (str.equals("pool_low"))
			return Xipc.MEM_T_POOL_LOW(parseInt());
		else if (str.equals("section_high"))
			return Xipc.MEM_T_SECTION_HIGH(parseInt());
		else if (str.equals("section_low"))
			return Xipc.MEM_T_SECTION_LOW(parseInt());
		else throw new parseError("expected mem trigger code , got " + str);
	}


	QueTriggerSpec parseQueTriggerCode() throws parseError		// for MemSecPriv
	{
		String str = tk.getNextToken();
		if (str.equals("bytes_high"))
			return Xipc.QUE_T_BYTES_HIGH(parseInt(),parseInt());
		else if (str.equals("bytes_low"))
			return Xipc.QUE_T_BYTES_LOW(parseInt(),parseInt());
		else if (str.equals("msgs_high"))
			return Xipc.QUE_T_MSGS_HIGH(parseInt(),parseInt());
		else if (str.equals("msgs_low"))
			return Xipc.QUE_T_MSGS_LOW(parseInt(),parseInt());
		else if (str.equals("put"))
			return Xipc.QUE_T_PUT(parseInt());
		else if (str.equals("get"))
			return Xipc.QUE_T_GET(parseInt());
		else if (str.equals("put_preq"))
			return Xipc.QUE_T_PUT_PREQ(parseInt(), parseInt());
		else if (str.equals("get_preq"))
			return Xipc.QUE_T_GET_PREQ(parseInt(), parseInt());
		else if (str.equals("put_prgt"))
			return Xipc.QUE_T_PUT_PRGT(parseInt(), parseInt());
		else if (str.equals("get_prgt"))
			return Xipc.QUE_T_GET_PRGT(parseInt(), parseInt());
		else if (str.equals("put_prlt"))
			return Xipc.QUE_T_PUT_PRLT(parseInt(), parseInt());
		else if (str.equals("get_prlt"))
			return Xipc.QUE_T_GET_PRLT(parseInt(), parseInt());
		else if (str.equals("user_put"))
			return Xipc.QUE_T_USER_PUT(parseInt(), parseInt());
		else if (str.equals("user_get"))
			return Xipc.QUE_T_USER_GET(parseInt(), parseInt());
		else if (str.equals("pool_high"))
			return Xipc.QUE_T_POOL_HIGH(parseInt());
		else if (str.equals("pool_low"))
			return Xipc.QUE_T_POOL_LOW(parseInt());
		else if (str.equals("header_high"))
			return Xipc.QUE_T_HEADER_HIGH(parseInt());
		else if (str.equals("header_low"))
			return Xipc.QUE_T_HEADER_LOW(parseInt());
		else throw new parseError("expected que trigger code , got " + str);
	}


	String parseSpoolFileName() throws parseError
	{
		String str = parseToken();
		if (str.equals("off"))
			return Xipc.QUE_SPOOL_OFF;
		else return str;
	}


	int parseQLimit() throws parseError
	{
		String str = parseToken();
		if (str.equals("nolimit")) return Xipc.QUE_NOLIMIT;
		else try
		{
			return Integer.parseInt(str);
		}
		catch (NumberFormatException e)
		{
			throw new parseError("Queue Limit should be  <int>|nolimit ,  got " + str);
		}
	}


	int parseSemCreateValue() throws parseError
	{
		String str = parseToken();
		if (str.equals("set")) return Xipc.SEM_SET;
		else if (str.equals("clear")) return Xipc.SEM_CLEAR;
		else try
		{
			return Integer.parseInt(str);
		}
		catch (NumberFormatException e)
		{
			throw new parseError("Sem CreateValue should be  <int>|set|clear ,  got " + str);
		}
	}


	int parseQSelCd() throws parseError
	{
		String str = parseToken();
		if (str.equals("ea")) return Xipc.QUE_Q_EA;
		else if (str.equals("la")) return Xipc.QUE_Q_LA;
		else if (str.equals("hp")) return Xipc.QUE_Q_HP;
		else if (str.equals("lp")) return Xipc.QUE_Q_LP;
		else if (str.equals("lnq")) return Xipc.QUE_Q_LNQ;
		else if (str.equals("shq")) return Xipc.QUE_Q_SHQ;
		else if (str.equals("hpq")) return Xipc.QUE_Q_HPQ;
		else if (str.equals("lpq")) return Xipc.QUE_Q_LPQ;
		else if (str.equals("eaq")) return Xipc.QUE_Q_EAQ;
		else if (str.equals("laq")) return Xipc.QUE_Q_LAQ;
		else if (str.equals("any")) return Xipc.QUE_Q_ANY;
		else throw new parseError("'" + str + "' is not a valid Queue Select Code");
	}

}	// end class lineparser



 /*-----------------------------------------------------------------------------------------*
  * public class jxipc has the main method - the body of the command processor
  *-----------------------------------------------------------------------------------------*/

public class jxipc {

	static boolean done = false;
	static Xipc xconn = new Xipc();
	static int RetCode;
	static Hashtable sectionTable = new Hashtable(26);
	static Hashtable msgHdrTable = new Hashtable(26);

	public static void main(String[] args)
	{
		int bytesread;
		byte inbuf[] = new byte[200];
		String instr = "error: got no input";

		System.out.println("Xipc Java Interactive Command Processor Version 3.0");
		System.out.println("Copyright (c) Momentum Software Corporation, 1997");

		while (!done)
		{
			try		// try parsing (in processCommandLine) - catch parseError if it occurs
			{
				System.out.print("\njxipc> ");
				System.out.flush();
				bytesread = System.in.read(inbuf);
				// trim trailing whitespace
				while (bytesread>0 && "\n\t\r ".indexOf(inbuf[bytesread-1])>=0) bytesread--;
				display("       ");
				instr = new String(inbuf, 0, 0, bytesread);

				if (instr.equals("")) continue;

				processCommandLine(instr);
			}
			catch (IOException e)
			{
				displayln("IOException " + e);
			}
			catch (parseError e)
			{
				displayln(e.str);
				displayln("type ? for help");
				displayln();
			}
		} // end main interactive command processing loop
	} // end method main

	static void displayln(String str) { System.out.print(str + "\n       "); }

	static void displayln() { System.out.print("\n       "); }

	static void display(String str) { System.out.print(str); }

	static String padl(int num, int len)
	{
		String retval = String.valueOf(num);
		int diff = len-retval.length();
		for (int i=0; i<diff; i++) retval = " "+retval;
		return retval;
	}

	static String padr(int num, int len)
	{
		String retval = String.valueOf(num);
		int diff = len-retval.length();
		for (int i=0; i<diff; i++) retval = retval+" ";
		return retval;
	}


	static void processCommandLine(String cmdline) throws parseError
	{
		String verb;
		lineparser lp;
		lp = new lineparser(cmdline); 	// create a lineparse object from the line that was read
		verb = lp.parseToken(); 	// get verb, pass lineparser to routine to handle that verb

		if (verb.equals("quit") || verb.equals("q")) done=true;
		else if (verb.equals("?") || verb.equals("help")) printHelp(lp);
		else if (verb.equals("xipcabort"))     do_xipcabort(lp);
		else if (verb.equals("xipcfreeze"))    do_xipcfreeze(lp);
		else if (verb.equals("xipclogin"))     do_xipclogin(lp);
		else if (verb.equals("xipclogout"))    do_xipclogout(lp);
		else if (verb.equals("xipcunfreeze"))  do_xipcunfreeze(lp);

		else if (verb.equals("semaccess"))     do_semaccess(lp);
		else if (verb.equals("semacquire"))    do_semacquire(lp);
		else if (verb.equals("semcancel"))     do_semcancel(lp);
		else if (verb.equals("semclear"))      do_semclear(lp);
		else if (verb.equals("semcreate"))     do_semcreate(lp);
		else if (verb.equals("semdelete"))     do_semdelete(lp);
		else if (verb.equals("semdestroy"))    do_semdestroy(lp);
		else if (verb.equals("semfreeze"))     do_semfreeze(lp);
		else if (verb.equals("seminfosem"))    do_seminfosem(lp);
		else if (verb.equals("seminfosys"))    do_seminfosys(lp);
		else if (verb.equals("seminfouser"))   do_seminfouser(lp);
		else if (verb.equals("semrelease"))    do_semrelease(lp);
		else if (verb.equals("semset"))        do_semset(lp);
		else if (verb.equals("semunfreeze"))   do_semunfreeze(lp);
		else if (verb.equals("semwait"))    do_semwait(lp);

		else if (verb.equals("queaccess"))     do_queaccess(lp);
		else if (verb.equals("quebrowse"))     do_quebrowse(lp);
		else if (verb.equals("quecopy"))    do_quecopy(lp);
		else if (verb.equals("quecreate"))     do_quecreate(lp);
		else if (verb.equals("quedelete"))     do_quedelete(lp);
		else if (verb.equals("quedestroy"))    do_quedestroy(lp);
		else if (verb.equals("quefreeze"))     do_quefreeze(lp);
		else if (verb.equals("queget"))        do_queget(lp);
		else if (verb.equals("queinfoque"))    do_queinfoque(lp);
		else if (verb.equals("queinfosys"))    do_queinfosys(lp);
		else if (verb.equals("queinfouser"))   do_queinfouser(lp);
		else if (verb.equals("quemsghdrdup"))  do_quemsghdrdup(lp);
		else if (verb.equals("quepurge"))      do_quepurge(lp);
		else if (verb.equals("queput"))        do_queput(lp);
		else if (verb.equals("queread"))    do_queread(lp);
		else if (verb.equals("quereceive"))    do_quereceive(lp);
		else if (verb.equals("queremove"))     do_queremove(lp);
		else if (verb.equals("quesend"))    do_quesend(lp);
		else if (verb.equals("quespool"))      do_quespool(lp);
		else if (verb.equals("quetrigger"))    do_quetrigger(lp);
		else if (verb.equals("queunfreeze"))   do_queunfreeze(lp);
		else if (verb.equals("queunget"))      do_queunget(lp);
		else if (verb.equals("queuntrigger"))  do_queuntrigger(lp);
		else if (verb.equals("quewrite"))      do_quewrite(lp);
		else if (verb.equals("msghdr"))        do_msghdr(lp);

		else if (verb.equals("memaccess"))     do_memaccess(lp);
		else if (verb.equals("memcreate"))     do_memcreate(lp);
		else if (verb.equals("memdelete"))     do_memdelete(lp);
		else if (verb.equals("memdestroy"))    do_memdestroy(lp);
		else if (verb.equals("memfreeze"))     do_memfreeze(lp);
		else if (verb.equals("meminfomem"))    do_meminfomem(lp);
		else if (verb.equals("meminfosec"))    do_meminfosec(lp);
		else if (verb.equals("meminfosys"))    do_meminfosys(lp);
		else if (verb.equals("meminfouser"))   do_meminfouser(lp);
		else if (verb.equals("memlock"))    do_memlock(lp);
		else if (verb.equals("memread"))    do_memread(lp);
		else if (verb.equals("memsection"))    do_memsection(lp);
		else if (verb.equals("memsecdef"))     do_memsecdef(lp);
		else if (verb.equals("memsecown"))     do_memsecown(lp);
		else if (verb.equals("memsecpriv"))    do_memsecpriv(lp);
		else if (verb.equals("memsecrel"))     do_memsecrel(lp);
		else if (verb.equals("memsecundef"))   do_memsecundef(lp);
		else if (verb.equals("memtrigger"))    do_memtrigger(lp);
		else if (verb.equals("memunfreeze"))   do_memunfreeze(lp);
		else if (verb.equals("memunlock"))     do_memunlock(lp);
		else if (verb.equals("memuntrigger"))  do_memuntrigger(lp);
		else if (verb.equals("memwrite"))      do_memwrite(lp);
		else if (verb.equals("section"))    do_section(lp);

		else if (verb.equals("xipcerror"))     do_xipcerror(lp);

// 	else if (verb.equals("debug"))         do_debug(lp);

		else displayln("I don't know the verb \"" + verb + "\", type ? for help");
	}	// end method processCommandLine



	/*------------------------------------------------------------------------------*
	 * The do_'verb' routines often simply print the result of calling the Xipc api verb, with
	 * calls to parse routines as arguments.	This exploits java's error throwing to handle
	 * parse errors, and also relies on java's left to right evaluation of method parameters
	 * (which is not a given in C).
	 *------------------------------------------------------------------------------*/


	static void do_xipcabort(lineparser lp) throws parseError
	{
		RetCode = xconn.XipcAbort(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_xipcfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.XipcFreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_xipclogin(lineparser lp) throws parseError
	{
		RetCode = xconn.XipcLogin(lp.parseToken(),lp.parseToken());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_xipclogout(lineparser lp) throws parseError
	{
		RetCode = xconn.XipcLogout();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_xipcunfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.XipcUnfreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semaccess(lineparser lp) throws parseError
	{
		RetCode = xconn.SemAccess(lp.parseToken());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semacquire(lineparser lp) throws parseError
	{
		XintRef SidPtr = new XintRef(0);
		RetCode = xconn.SemAcquire(lp.parseAnyAllAtomic("sem"), lp.parseSidList(), SidPtr, lp.parseWaitNoWaitTimeOut("sem"));
		displayln("Sid = " + SidPtr.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semcancel(lineparser lp) throws parseError
	{
		XintRef SidPtr = new XintRef(0);
		RetCode = xconn.SemCancel(lp.parseSidList(), SidPtr);
		displayln("Sid = " + SidPtr.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semclear(lineparser lp) throws parseError
	{
		XintRef SidPtr = new XintRef(0);
		RetCode = xconn.SemClear(lp.parseSidList(), SidPtr);
		displayln("Sid = " + SidPtr.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semcreate(lineparser lp) throws parseError
	{
		RetCode = xconn.SemCreate(lp.parseToken(), lp.parseSemCreateValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semdelete(lineparser lp) throws parseError
	{
		RetCode = xconn.SemDelete(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semdestroy(lineparser lp) throws parseError
	{
		RetCode = xconn.SemDestroy(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.SemFreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_seminfosem(lineparser lp) throws parseError
	{
		SemInfoSem infoSem = new SemInfoSem();
		infoSem.hListOffset=0;
		infoSem.wListOffset=0;
		RetCode = xconn.SemInfoSem(lp.parseInt(), infoSem);
		if (RetCode>=0)
		{
			Date tmpDate = new Date(1000*(long)infoSem.createTime);
			displayln("Sid: " + infoSem.sid + "  Name: '" + infoSem.name
						+ "'  Type: " + ((infoSem.semType==0) ? "Resource" : "Event"));
			displayln("Created by Uid: " + infoSem.createUid + "  at: " + tmpDate.toString());
			displayln("Last Accessed by Uid: " + infoSem.lastUid);
			if (infoSem.semType==0) // resource
				displayln("Maximum Value: " + infoSem.maxValue + "  Current Value: " + infoSem.curValue);
			else
				displayln("Status: " + ((infoSem.curValue==0) ? "Clear" : "Set"));
			display("hList: ");
			for (int i=0; i<infoSem.hListLength; i++)
				display(" " + infoSem.hList[i].uid);
			if (infoSem.hListTotalLength>infoSem.hListLength) displayln(" ..."); else displayln();
			display("wList: ");
			for (int i=0; i<infoSem.wListLength; i++)
				display(" " + infoSem.wList[i].uid);
			if (infoSem.wListTotalLength>infoSem.wListLength) displayln(" ..."); else displayln();
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_seminfosys(lineparser lp) throws parseError
	{
		SemInfoSys infoSys = new SemInfoSys();
		RetCode = xconn.SemInfoSys(infoSys);
		if (RetCode>=0)
		{
			displayln("Configuration: '" + infoSys.name + "'");
			displayln("         Maximum   Current");
			displayln("Users:" + padl(infoSys.maxUsers,10) + padl(infoSys.curUsers,10));
			displayln(" Sems:" + padl(infoSys.maxSems,10) + padl(infoSys.curSems,10));
			displayln("Nodes:" + padl(infoSys.maxNodes,10) + padl(infoSys.maxNodes-infoSys.freeNCnt,10));
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_seminfouser(lineparser lp) throws parseError
	{
		SemInfoUser infoUser = new SemInfoUser();
		infoUser.hListOffset=0;
		infoUser.wListOffset=0;
		infoUser.qListOffset=0;
		RetCode = xconn.SemInfoUser(lp.parseInt(), infoUser);
		if (RetCode>=0)
		{
			Date tmpDate = new Date(1000*(long)infoUser.loginTime);
			displayln("Uid: " + infoUser.uid + "   Name: '" + infoUser.name + "'");
			displayln("Host: '" + infoUser.netLoc + "'  Login Time: " + tmpDate.toString());
			display("Status: ");
			switch (infoUser.waitType)
			{
				case Xipc.SEM_ATOMIC:	displayln("Blocked ATOMIC"); break;
				case Xipc.SEM_ALL:		displayln("Blocked ALL"); break;
				case Xipc.SEM_ANY:		displayln("Blocked ANY"); break;
				default: displayln("Not Blocked");
			}
			if (infoUser.timeOut==0)
				displayln("TimeOut: 0");
			else
				displayln("TimeOut: " + (infoUser.timeOut - (int)( ((new Date()).getTime()) / 1000 )) );
			display("hList: ");
			for (int i=0; i<infoUser.hListLength; i++)
				display(" " + infoUser.hList[i].sid);
			if (infoUser.hListTotalLength>infoUser.hListLength) displayln(" ..."); else displayln();
			display("wList: ");
			for (int i=0; i<infoUser.wListLength; i++)
				display(" " + infoUser.wList[i].sid);
			if (infoUser.wListTotalLength>infoUser.wListLength) displayln(" ..."); else displayln();
			display("qList: ");
			for (int i=0; i<infoUser.qListLength; i++)
				display(" " + infoUser.qList[i].sid);
			if (infoUser.qListTotalLength>infoUser.qListLength) displayln(" ..."); else displayln();
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semrelease(lineparser lp) throws parseError
	{
		XintRef SidPtr = new XintRef(0);
		RetCode = xconn.SemRelease(lp.parseSidList(), SidPtr);
		displayln("Sid = " + SidPtr.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semset(lineparser lp) throws parseError
	{
		XintRef SidPtr = new XintRef(0);
		RetCode = xconn.SemSet(lp.parseSidList(), SidPtr);
		displayln("Sid = " + SidPtr.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semunfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.SemUnfreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_semwait(lineparser lp) throws parseError
	{
		XintRef SidPtr = new XintRef(0);
		RetCode = xconn.SemWait(lp.parseAnyAllAtomic("sem"), lp.parseSidList(), SidPtr, lp.parseSemWaitBlockopt());
		displayln("Sid = " + SidPtr.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queaccess(lineparser lp) throws parseError
	{
		RetCode = xconn.QueAccess(lp.parseToken());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quebrowse(lineparser lp) throws parseError
	{
		MsgHdr msgHdr = lp.parseExistingMsgHdr();
		RetCode = xconn.QueBrowse(msgHdr, lp.parseDirection());
		if (RetCode>=0)
		{
			display("Qid = " + msgHdr.getQid + ", Seq# = " + msgHdr.seqNum
					  + ", Prio = " + msgHdr.priority + ", HdrStatus = ");
			switch (msgHdr.hdrStatus)
			{
				case Xipc.QUE_REMOVED		: displayln("REMOVED"); break;
				case Xipc.QUE_NOTREMOVED	: displayln("NOT REMOVED"); break;
				case Xipc.QUE_REPLICATED	: displayln("REPLICATED"); break;
				case Xipc.QUE_DUPLICATED	: displayln("DUPLICATED"); break;
				default	: displayln("?");
			}
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quecopy(lineparser lp) throws parseError
	{
		byte inbuf[] = new byte[200];
		MsgHdr msgHdr = lp.parseExistingMsgHdr();
		int offset = lp.parseInt();
		int length = lp.parseIntOrStar();
		if (length == -1) length=msgHdr.size-offset;
		RetCode = xconn.QueCopy(msgHdr,offset,length,inbuf);
		if (RetCode>=0)
		{
			String text = new String(inbuf, 0, 0, length);
			displayln("Text = \"" + text + "\"");
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quecreate(lineparser lp) throws parseError
	{
		RetCode = xconn.QueCreate(lp.parseToken(), lp.parseQLimit(), lp.parseQLimit());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quedelete(lineparser lp) throws parseError
	{
		RetCode = xconn.QueDelete(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quedestroy(lineparser lp) throws parseError
	{
		RetCode = xconn.QueDestroy(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quefreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.QueFreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queget(lineparser lp) throws parseError
	{
		MsgHdr msgHdr = lp.parseMsgHdr();
		RetCode = xconn.QueGet(msgHdr, lp.parseQSelCd(), lp.parseMSCQidList(),
								null, null, lp.parseQueGetBlockopt());
		if (RetCode>=0)
		{
			display("Qid = " + msgHdr.getQid + ", Seq# = " + msgHdr.seqNum
					  + ", Prio = " + msgHdr.priority + ", HdrStatus = ");
			switch (msgHdr.hdrStatus)
			{
				case Xipc.QUE_REMOVED		: displayln("REMOVED"); break;
				case Xipc.QUE_NOTREMOVED	: displayln("NOT REMOVED"); break;
				case Xipc.QUE_REPLICATED	: displayln("REPLICATED"); break;
				case Xipc.QUE_DUPLICATED	: displayln("DUPLICATED"); break;
				default	: displayln("?");
			}
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queinfoque(lineparser lp) throws parseError
	{
		QueInfoQue infoQue = new QueInfoQue();
		infoQue.wListOffset=0;
		RetCode = xconn.QueInfoQue(lp.parseInt(), infoQue);
		if (RetCode>=0)
		{
			Date tmpDate = new Date(1000*(long)infoQue.createTime);
			displayln("Qid: " + infoQue.qid + "  Name: '" + infoQue.name + "'");

			displayln("Created by Uid: " + infoQue.createUid + "  at: " + tmpDate.toString());
			displayln("Last Accessed by Uid: " + infoQue.lastUid);
			displayln("             Limit    Count       In      Out");
			displayln("Messages:" + padl(infoQue.limitMessages,9) + padl(infoQue.countMessages,9)
								+ padl(infoQue.countIn ,9) + padl(infoQue.countOut,9));
			displayln("   Bytes:" + padl(infoQue.limitBytes,9) + padl(infoQue.countBytes,9));

			displayln("Last Get Uid:" + padl(infoQue.lastUidGet ,5)
						+ "         Last Put Uid:" + padl(infoQue.lastUidPut,5));
			displayln("Spooling: " + ((infoQue.spoolFlag==0) ? "OFF" : "ON")
						+ "   Messages: " + infoQue.spoolMessages
						+ "   Bytes: " + infoQue.spoolBytes);
			displayln("Spool File: '" + infoQue.spoolFileName + "'");
			displayln("wList: ");
			for (int i=0; i<infoQue.wListLength; i++)
			{
				Que_QueWListItem tmp = infoQue.wList[i];
				if (tmp.opCode==Xipc.QUE_BLOCKEDPUT)
					displayln("       Put, Uid: " + ((Que_QueWListItem.Put)tmp.u).uid);
				else
					displayln("       Get, Uid: " + ((Que_QueWListItem.Get)tmp.u).uid);
			}
			if (infoQue.wListTotalLength>infoQue.wListLength) displayln("       & more ...");
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queinfosys(lineparser lp) throws parseError
	{
		QueInfoSys infoSys = new QueInfoSys();
		RetCode = xconn.QueInfoSys(infoSys);
		if (RetCode>=0)
		{
			displayln("Configuration: '" + infoSys.name + "'");
			displayln("           Maximum   Current");
			displayln("  Users:" + padl(infoSys.maxUsers,10) + padl(infoSys.curUsers,10));
			displayln(" Queues:" + padl(infoSys.maxQueues,10) + padl(infoSys.curQueues,10));
			displayln("  Nodes:" + padl(infoSys.maxNodes,10) + padl(infoSys.maxNodes-infoSys.freeNCnt,10));
			displayln("Headers:" + padl(infoSys.maxHeaders,10) + padl(infoSys.maxHeaders-infoSys.freeHCnt,10));
			displayln("----------------------------");
			displayln(" Message Pool Size:" + padl(infoSys.msgPoolSizeBytes,10)
					+ "      Message Tick Size:" + padl(infoSys.msgTickSize,10));
			displayln("Message Pool Avail:" + padl(infoSys.msgPoolTotalAvail,10)
					+ "          Largest Block:" + padl(infoSys.msgPoolLargestBlk,10));
			displayln("Message Pool Max Blocks: " + infoSys.msgPoolMaxPosBlks
					+ "     Used Blocks: " + infoSys.msgPoolTotalBlks);
			displayln("wList: ");
			for (int i=0; i<infoSys.wListLength; i++)
			{
				displayln("       Uid: " + infoSys.wList[i].uid + "   Msg Size: " + infoSys.wList[i].msgSize);
			}
			if (infoSys.wListTotalLength>infoSys.wListLength) displayln("       & more ...");
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queinfouser(lineparser lp) throws parseError
	{
		QueInfoUser infoUser = new QueInfoUser();
		infoUser.wListOffset=0;
		RetCode = xconn.QueInfoUser(lp.parseInt(), infoUser);
		if (RetCode>=0)
		{
			Date tmpDate = new Date(1000*(long)infoUser.loginTime);
			displayln("Uid: " + infoUser.uid + "   Name: '" + infoUser.name + "'");
			displayln("Host: '" + infoUser.netLoc + "'   Login Time: " + tmpDate.toString());
			display("Status: ");
			switch(infoUser.waitType)
			{
				case Xipc.QUE_BLOCKEDWRITE:	 displayln("Blocked on WRITE"); break;
				case Xipc.QUE_BLOCKEDPUT:	 displayln("Blocked on PUT"); break;
				case Xipc.QUE_BLOCKEDGET:	 displayln("Blocked on GET"); break;
				default: displayln("Not Blocked");
			}
			if (infoUser.timeOut==0)
				displayln("TimeOut: 0");
			else
				displayln("TimeOut: " + (infoUser.timeOut - (int)( ((new Date()).getTime()) / 1000 )) );
			displayln("          Count   Last Qid");
			displayln("Get:" + padl(infoUser.countGet,11) + padl(infoUser.lastQidGet,11));
			displayln("Put:" + padl(infoUser.countPut,11) + padl(infoUser.lastQidPut,11));
			displayln("wList: ");
			for (int i=0; i<infoUser.wListLength; i++)
			{
				Que_UserWListItem tmp = infoUser.wList[i];
				if (tmp.opCode==Xipc.QUE_BLOCKEDPUT)
					displayln("       Put, Qid: " + ((Que_UserWListItem.Put)tmp.u).qid
								+ "  Msg Size: " + ((Que_UserWListItem.Put)tmp.u).msgSize
								+ "  Msg Prio: " + ((Que_UserWListItem.Put)tmp.u).msgPrio );
				else if (tmp.opCode==Xipc.QUE_BLOCKEDGET)
				{
					int parm1 = ((Que_UserWListItem.Get)tmp.u).parm1;
					int parm2 = ((Que_UserWListItem.Get)tmp.u).parm2;
					display("       Get, Qid: " + ((Que_UserWListItem.Get)tmp.u).qid + "  Msg Select: ");
					switch( ((Que_UserWListItem.Get)tmp.u).msgSelCode )
					{
					case Xipc.QUE_M_HP_CODE: displayln("Highest Priority"); break;
					case Xipc.QUE_M_LP_CODE: displayln("Lowest Priority"); break;
					case Xipc.QUE_M_EA_CODE: displayln("Earliest Arrived"); break;
					case Xipc.QUE_M_LA_CODE: displayln("Latest Arrived"); break;
					case Xipc.QUE_M_PREQ_CODE: displayln("Priority = " + parm1); break;
					case Xipc.QUE_M_PRNE_CODE: displayln("Priority != " + parm1); break;
					case Xipc.QUE_M_PRGT_CODE: displayln("Priority > " + parm1); break;
					case Xipc.QUE_M_PRGE_CODE: displayln("Priority >= " + parm1); break;
					case Xipc.QUE_M_PRLT_CODE: displayln("Priority < " + parm1); break;
					case Xipc.QUE_M_PRLE_CODE: displayln("Priority <= " + parm1); break;
					case Xipc.QUE_M_PRRNG_CODE: displayln("Priority in range [" + parm1 + ".." + parm2 + "]"); break;
					case Xipc.QUE_M_SEQEQ_CODE: displayln("Seq# = " + parm1); break;
					case Xipc.QUE_M_SEQGT_CODE: displayln("Seq# > " + parm1); break;
					case Xipc.QUE_M_SEQGE_CODE: displayln("Seq# >= " + parm1); break;
					case Xipc.QUE_M_SEQLT_CODE: displayln("Seq# < " + parm1); break;
					case Xipc.QUE_M_SEQLE_CODE: displayln("Seq# <= " + parm1); break;
					}
				}
				else
					displayln("       Write,  Msg Size: " + ((Que_UserWListItem.Write)tmp.u).msgSize);
			}
			if (infoUser.wListTotalLength>infoUser.wListLength) displayln("       & more ...");
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quemsghdrdup(lineparser lp) throws parseError
	{
		MsgHdr srcMsgHdr = lp.parseExistingMsgHdr();
		MsgHdr destMsgHdr = lp.parseMsgHdr();
		RetCode = xconn.QueMsgHdrDup(srcMsgHdr, destMsgHdr);
		if (RetCode>=0)
		{
			display("Qid = " + destMsgHdr.getQid + ", Seq# = " + destMsgHdr.seqNum
					  + ", Prio = " + destMsgHdr.priority + ", HdrStatus = ");
			switch (destMsgHdr.hdrStatus)
			{
				case Xipc.QUE_REMOVED		: displayln("REMOVED"); break;
				case Xipc.QUE_NOTREMOVED	: displayln("NOT REMOVED"); break;
				case Xipc.QUE_REPLICATED	: displayln("REPLICATED"); break;
				case Xipc.QUE_DUPLICATED	: displayln("DUPLICATED"); break;
				default	: displayln("?");
			}
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quepurge(lineparser lp) throws parseError
	{
		RetCode = xconn.QuePurge(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queput(lineparser lp) throws parseError
	{
		XintRef RetQid = new XintRef(0);
		RetCode = xconn.QuePut(lp.parseExistingMsgHdr(), lp.parseQSelCd(), lp.parsePlainQidList(),
								lp.parseInt(), RetQid, lp.parseQuePutBlockopt() );
		displayln("Qid = " + RetQid.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queread(lineparser lp) throws parseError
	{
		byte inbuf[] = new byte[200];
		MsgHdr msgHdr = lp.parseExistingMsgHdr();
		int len = msgHdr.size;
		RetCode = xconn.QueRead(msgHdr, inbuf, len);
		if (RetCode>=0)
		{
			String text = new String(inbuf, 0, 0, len);
			displayln("Text = \"" + text + "\"");
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quereceive(lineparser lp) throws parseError
	{
		byte inbuf[] = new byte[200];
		XintRef RetPrio = new XintRef(0);
		XintRef RetQid = new XintRef(0);
		RetCode = xconn.QueReceive(lp.parseQSelCd(), lp.parseMSCQidList(), inbuf, 200,
									RetPrio, RetQid, lp.parseQueGetBlockopt());
		if (RetCode < 0)
		{
			displayln("Qid = " + RetQid.getValue());
			displayln("RetCode = " + RetCode);
			displayln(Xipc.XipcError(RetCode));
		}
		else
		{
			displayln("Qid = " + RetQid.getValue());
			displayln("Priority = " + RetPrio.getValue());
			displayln("Length = " + RetCode);
			String text = new String(inbuf, 0, 0, RetCode);
			displayln("Text = \"" + text + "\"");
		}
	}



	static void do_queremove(lineparser lp) throws parseError
	{
		RetCode = xconn.QueRemove(lp.parseExistingMsgHdr());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quesend(lineparser lp) throws parseError
	{
		byte outbuf[] = new byte[200];
		XintRef RetQid = new XintRef(0);
		int QSelCd = lp.parseQSelCd();
		QidList ql = lp.parsePlainQidList();
		int priority = lp.parseInt();
		String msg = lp.parseToken();
		msg.getBytes(0, msg.length(), outbuf, 0);
		RetCode = xconn.QueSend(QSelCd, ql, outbuf, msg.length(), priority, RetQid, lp.parseQuePutBlockopt());
		displayln("Qid = " + RetQid.getValue());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quespool(lineparser lp) throws parseError
	{
		RetCode = xconn.QueSpool(lp.parseInt(), lp.parseSpoolFileName());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quetrigger(lineparser lp) throws parseError
	{
		RetCode = xconn.QueTrigger(lp.parseInt(), lp.parseQueTriggerCode());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queunfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.QueUnfreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queunget(lineparser lp) throws parseError
	{
		RetCode = xconn.QueUnget(lp.parseExistingMsgHdr());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_queuntrigger(lineparser lp) throws parseError
	{
		RetCode = xconn.QueUntrigger(lp.parseInt(), lp.parseQueTriggerCode());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_quewrite(lineparser lp) throws parseError
	{
		byte outbuf[] = new byte[200];
		MsgHdr msgHdr = lp.parseMsgHdr();
		String msg = lp.parseToken();
		msg.getBytes(0, msg.length(), outbuf, 0);
		RetCode = xconn.QueWrite(msgHdr, outbuf, msg.length(), lp.parseWaitNoWaitTimeOut("que"));
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_msghdr(lineparser lp) throws parseError
	{
		MsgHdr msgHdr = lp.parseExistingMsgHdr();
		display("Qid = " + msgHdr.getQid + ", Seq# = " + msgHdr.seqNum
				  + ", Prio = " + msgHdr.priority + ", HdrStatus = ");
		switch (msgHdr.hdrStatus)
		{
			case Xipc.QUE_REMOVED		: displayln("REMOVED"); break;
			case Xipc.QUE_NOTREMOVED	: displayln("NOT REMOVED"); break;
			case Xipc.QUE_REPLICATED	: displayln("REPLICATED"); break;
			case Xipc.QUE_DUPLICATED	: displayln("DUPLICATED"); break;
			default	: displayln("?");
		}
	}


	static void do_memaccess(lineparser lp) throws parseError
	{
		RetCode = xconn.MemAccess(lp.parseToken());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memcreate(lineparser lp) throws parseError
	{
		RetCode = xconn.MemCreate(lp.parseToken(), lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memdelete(lineparser lp) throws parseError
	{
		RetCode = xconn.MemDelete(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memdestroy(lineparser lp) throws parseError
	{
		RetCode = xconn.MemDestroy(lp.parseInt());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.MemFreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_meminfomem(lineparser lp) throws parseError
	{
		MemInfoMem infoMem = new MemInfoMem();
		infoMem.sListOffset=0;
		infoMem.wListOffset=0;
		RetCode = xconn.MemInfoMem(lp.parseInt(), infoMem);
		if (RetCode>=0)
		{
			Date tmpDate = new Date(1000*(long)infoMem.createTime);
			displayln("Mid: " + infoMem.mid + "   Name: '" + infoMem.name + "'");
			displayln("Created by Uid: " + infoMem.createUid + "  at: " + tmpDate.toString());
			displayln("Sections Defined: " + infoMem.numSections + "   Bytes Allocated: " + infoMem.size);

			displayln("            Owned  Locked");
			displayln("Sections:"+ padl(infoMem.numSecOwned,8)   + padl(infoMem.numSecLocked,8));
			displayln("   Bytes:"+ padl(infoMem.numBytesOwned,8) + padl(infoMem.numBytesLocked,8));
			displayln("Last Uid:"+ padl(infoMem.lastUidOwned,8)  + padl(infoMem.lastUidLocked,8));
			displayln("               Count   Last Uid");
			displayln("   Write:" + padl(infoMem.countWrite,11) + padl(infoMem.lastUidWrite,11));
			displayln("    Read:" + padl(infoMem.countRead,11) + padl(infoMem.lastUidRead,11));
			if (infoMem.sListLength==0)
				displayln("sList is empty");
			else
			{
				displayln("sList: Owner Uid  Owner Priv  Other Priv   Section");
				for (int i=0; i<infoMem.sListLength; i++)
				{
					MemInfoMem.Mem_MemSListItem tmp = infoMem.sList[i];
					display(padl(tmp.ownerUid,16));
					switch (tmp.ownerPriv)
					{
						case Xipc.MEM_RW : display("          RW"); break;
						case Xipc.MEM_RO : display("          RO"); break;
						case Xipc.MEM_WO : display("          WO"); break;
						case Xipc.MEM_NA : display("          NA"); break;
					}
					switch (tmp.otherPriv)
					{
						case Xipc.MEM_RW : display("          RW"); break;
						case Xipc.MEM_RO : display("          RO"); break;
						case Xipc.MEM_WO : display("          WO"); break;
						case Xipc.MEM_NA : display("          NA"); break;
					}
					displayln("   ("+tmp.mid+" "+tmp.offset+" "+tmp.size+")");
				}
				if (infoMem.sListTotalLength>infoMem.sListLength) displayln("       & more ...");
			}

			if (infoMem.wListLength==0)
				displayln("wList is empty");
			else
			{
				displayln("wList:    Uid  Blocked Op  Offset    Size");
				for (int i=0; i<infoMem.wListLength; i++)
				{
					MemInfoMem.Mem_MemWListItem tmp = infoMem.wList[i];
					display(padl(tmp.uid,13));
					switch (tmp.opCode)
					{
						case -901 : display("       Write"); break;
						case -902 : display("       Read "); break;
						case -903 : display("       Own  "); break;
						case -904 : display("       Lock "); break;
					}
					displayln(padl(tmp.offset,8)+padl(tmp.size,8));
				}
				if (infoMem.wListTotalLength>infoMem.wListLength) displayln("       & more ...");
			}
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_meminfosec(lineparser lp) throws parseError
	{
		MemInfoSec infoSec = new MemInfoSec();
		RetCode = xconn.MemInfoSec(lp.parseSection(), infoSec);
		if (RetCode>=0)
		{
			displayln("Mid: " + infoSec.mid + ",  Offset: " + infoSec.offset + ",  Size: " + infoSec.size);
			displayln("Owner: " + infoSec.ownerUid);
			display("Owner Privileges: ");
			switch (infoSec.ownerPriv)
			{
				case Xipc.MEM_RW : displayln("RW"); break;
				case Xipc.MEM_RO : displayln("RO"); break;
				case Xipc.MEM_WO : displayln("WO"); break;
				case Xipc.MEM_NA : displayln("NA"); break;
			}
			display("Other Privileges: ");
			switch (infoSec.otherPriv)
			{
				case Xipc.MEM_RW : displayln("RW"); break;
				case Xipc.MEM_RO : displayln("RO"); break;
				case Xipc.MEM_WO : displayln("WO"); break;
				case Xipc.MEM_NA : displayln("NA"); break;
			}
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_meminfosys(lineparser lp) throws parseError
	{
		MemInfoSys infoSys = new MemInfoSys();
		RetCode = xconn.MemInfoSys(infoSys);
		if (RetCode>=0)
		{
			displayln("Configuration: '" + infoSys.name + "'");
			displayln("            Maximum   Current");
			displayln("   Users:" + padl(infoSys.maxUsers,10)    + padl(infoSys.curUsers,10));
			displayln("Segments:" + padl(infoSys.maxSegments,10) + padl(infoSys.curSegments,10));
			displayln("   Nodes:" + padl(infoSys.maxNodes,10)    + padl(infoSys.maxNodes-infoSys.freeNCnt,10));
			displayln("Sections:" + padl(infoSys.maxSections,10) + padl(infoSys.maxSections-infoSys.freeSCnt,10));
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_meminfouser(lineparser lp) throws parseError
	{
		MemInfoUser infoUser = new MemInfoUser();
		infoUser.hListOffset=0;
		infoUser.wListOffset=0;
		infoUser.qListOffset=0;
		RetCode = xconn.MemInfoUser(lp.parseInt(), infoUser);
		if (RetCode>=0)
		{
			Date tmpDate = new Date(1000*(long)infoUser.loginTime);
			displayln("Uid: " + infoUser.uid + "   Name: '" + infoUser.name + "'");
			displayln("Host: '" + infoUser.netLoc + "'   Login Time: " + tmpDate.toString());
			display("Status: ");
			switch (infoUser.waitType)
			{
				case Xipc.MEM_BLOCKEDWRITE:	 displayln("Blocked on WRITE"); break;
				case Xipc.MEM_BLOCKEDREAD:  displayln("Blocked on READ"); break;
				case Xipc.MEM_BLOCKEDOWN:	 displayln("Blocked on OWN"); break;
				case Xipc.MEM_BLOCKEDLOCK:  displayln("Blocked on LOCK"); break;
				default: displayln("Not Blocked");
			}
			if (infoUser.timeOut==0)
				displayln("TimeOut: 0");
			else
				displayln("TimeOut: " + (infoUser.timeOut - (int)( ((new Date()).getTime()) / 1000 )) );
			displayln("hList: ");
			for (int i=0; i<infoUser.hListLength; i++)
			{
				displayln("       (" + infoUser.hList[i].mid + " " + infoUser.hList[i].offset + " "
							+ infoUser.hList[i].size + ")  "
							+ (infoUser.hList[i].opCode==-801 ? "Locked" : "Owned"));
			}
			if (infoUser.hListTotalLength>infoUser.hListLength) displayln("       & more ..."); ;

			displayln("wList: ");
			for (int i=0; i<infoUser.wListLength; i++)
			{
				displayln("       (" + infoUser.wList[i].mid + " " + infoUser.wList[i].offset + " "
							+ infoUser.wList[i].size + ")" );
			}
			if (infoUser.wListTotalLength>infoUser.wListLength) displayln("       & more ...");

			displayln("qList: ");
			for (int i=0; i<infoUser.qListLength; i++)
			{
				displayln("       (" + infoUser.qList[i].mid + " " + infoUser.qList[i].offset + " "
							+ infoUser.qList[i].size + ")" );
			}
			if (infoUser.qListTotalLength>infoUser.qListLength) displayln("       & more ...");
		}
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memlock(lineparser lp) throws parseError
	{
		Section SecPtr = Xipc.MemSectionBuild(0,0,0);
		RetCode = xconn.MemLock(lp.parseAnyAllAtomic("mem"), lp.parseMidList(), SecPtr, lp.parseWaitNoWaitTimeOut("mem"));
		displayln("Section = ("+SecPtr.getMid()+" "+SecPtr.getOffset()+" "+SecPtr.getSize()+ ")");
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memread(lineparser lp) throws parseError
	{
		byte inbuf[] = new byte[200];
		int mid = lp.parseInt();
		int offset = lp.parseInt();
		int len = lp.parseInt();
		RetCode = xconn.MemRead(mid, offset, len, inbuf, lp.parseWaitNoWaitTimeOut("mem"));
		if (RetCode >= 0)
			displayln("Text = \"" + new String(inbuf, 0, 0, len) + "\"");
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memsection(lineparser lp) throws parseError
	{
		String var = lp.parseToken();
		if ("abcdefghijklmnopqrstuvwxyz".indexOf(var.charAt(0))<0)
			throw new parseError("variable \""+ var + "\" : section variable should begin with lowercase letter");
		Section sec = lp.parseSection();
		jxipc.sectionTable.put(var,sec);
		displayln("Section = ("+sec.getMid()+" "+sec.getOffset()+" "+sec.getSize()+ ")");
	}


	static void do_memsecdef(lineparser lp) throws parseError
	{
		RetCode = xconn.MemSecDef(lp.parseSection());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memsecown(lineparser lp) throws parseError
	{
		Section SecPtr = Xipc.MemSectionBuild(0,0,0);
		RetCode = xconn.MemSecOwn(lp.parseAnyAllAtomic("mem"), lp.parseMidList(),
									SecPtr, lp.parseWaitNoWaitTimeOut("mem"));
		displayln("Section = ("+SecPtr.getMid()+" "+SecPtr.getOffset()+" "+SecPtr.getSize()+ ")");
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memsecpriv(lineparser lp) throws parseError
	{
		RetCode = xconn.MemSecPriv(lp.parseSection(), lp.parsePrivelege(), lp.parsePrivelege());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memsecrel(lineparser lp) throws parseError
	{
		Section SecPtr = Xipc.MemSectionBuild(0,0,0);
		RetCode = xconn.MemSecRel(lp.parseMidList(), SecPtr);
		displayln("Section = ("+SecPtr.getMid()+" "+SecPtr.getOffset()+" "+SecPtr.getSize()+ ")");
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memsecundef(lineparser lp) throws parseError
	{
		RetCode = xconn.MemSecUndef(lp.parseSection());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memtrigger(lineparser lp) throws parseError
	{
		RetCode = xconn.MemTrigger(lp.parseInt(), lp.parseMemTriggerCode());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memunfreeze(lineparser lp) throws parseError
	{
		RetCode = xconn.MemUnfreeze();
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memunlock(lineparser lp) throws parseError
	{
		Section SecPtr = Xipc.MemSectionBuild(0,0,0);
		RetCode = xconn.MemUnlock(lp.parseMidList(), SecPtr);
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memuntrigger(lineparser lp) throws parseError
	{
		RetCode = xconn.MemUntrigger(lp.parseInt(), lp.parseMemTriggerCode());
		displayln("RetCode = " + RetCode);
		if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
	}


	static void do_memwrite(lineparser lp) throws parseError
	{
		int mid = lp.parseInt();
		int offset = lp.parseInt();
		int length = lp.parseInt();
		String text = lp.parseToken();
		if (text.length()==3 && text.charAt(0)=='\'' && text.charAt(2)=='\'')   // MEM_FILL option
		{
			byte outbuf[] = new byte[1];
			text.getBytes(1,2,outbuf,0);
			RetCode = xconn.MemWrite(mid,offset,length,outbuf[0],lp.parseWaitNoWaitTimeOut("mem"));
			displayln("RetCode = " + RetCode);
			if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
		}
		else
		{
			byte outbuf[] = new byte[200];
			text.getBytes(0, text.length(), outbuf, 0);
			RetCode = xconn.MemWrite(mid,offset,length,outbuf,lp.parseWaitNoWaitTimeOut("mem"));
			displayln("RetCode = " + RetCode);
			if (RetCode < 0) displayln(Xipc.XipcError(RetCode));
		}
	}


	static void do_section(lineparser lp) throws parseError
	{
		String var = lp.parseToken();
		if ("abcdefghijklmnopqrstuvwxyz".indexOf(var.charAt(0))<0)
			throw new parseError("variable \""+ var + "\" : section variable should begin with lowercase letter");
		Section sec = (Section)jxipc.sectionTable.get(var);
		if (sec==null)
			displayln("variable \"" + var + "\" has not been assigned a section (type: ? memsection for help)");
		else
			displayln("Section = ("+sec.getMid()+" "+sec.getOffset()+" "+sec.getSize()+ ")");
	}


	static void do_xipcerror(lineparser lp) throws parseError
	{
		int code = lp.parseInt();
		if (code>0) code = -code;
		displayln(Xipc.XipcError(code));
	}


// static void do_debug(lineparser lp) throws parseError
// {
// 	String name = lp.parseToken();
// 	if (name.equals("off"))
// 	{
// 		xconn.DEBUG_NAME=null;
// 		displayln("debugging OFF");
// 	}
// 	else
// 	{
// 		xconn.DEBUG_NAME=name;
// 		displayln("debugging ON with name = " + name);
// 	}
// }


	//*  help printing routine (limited, for now)
	static void printHelp(lineparser lp)
	{
		try
		{
			String verb = lp.parseToken();		// get verb, print help msg

			if (verb.equals("quit") || verb.equals("q"))
				displayln("quits the jxipc command processor");
			else if (verb.equals("xipcabort"))
				displayln("xipcabort  <userid>");
			else if (verb.equals("xipcfreeze"))
				displayln("xipcfreeze  (no parameters)");
			else if (verb.equals("xipclogin"))
				displayln("xipclogin  <instance>  <username>");
			else if (verb.equals("xipclogout"))
				displayln("xipclogout  (no parameters)");
			else if (verb.equals("xipcunfreeze"))
				displayln("xipcunfreeze (no parameters)");
			else if (verb.equals("semaccess"))
				displayln("semaccess  <name>");
			else if (verb.equals("semacquire"))
				displayln("semacquire  any|all|atomic  <sidlist>  wait|nowait|timeout(<int>)");
			else if (verb.equals("semcancel"))
				displayln("semcancel  <sidlist>");
			else if (verb.equals("semclear"))
				displayln("semclear  <sidlist>");
			else if (verb.equals("semcreate"))
				displayln("semcreate  <name> <count>|clear|set");
			else if (verb.equals("semdelete"))
				displayln("semdelete  <sid>");
			else if (verb.equals("semdestroy"))
				displayln("semdestroy  <sid>");
			else if (verb.equals("semfreeze"))
				displayln("semfreeze  (no parameters)");
			else if (verb.equals("seminfosem"))
				displayln("seminfosem  <sid>");
			else if (verb.equals("seminfosys"))
				displayln("seminfosys  (no parameters)");
			else if (verb.equals("seminfouser"))
				displayln("seminfouser  <uid>");
			else if (verb.equals("semrelease"))
				displayln("semrelease  <sidlist>");
			else if (verb.equals("semset"))
				displayln("semset  <sidlist>");
			else if (verb.equals("semunfreeze"))
				displayln("semunfreeze  (no parameters)");
			else if (verb.equals("semwait"))
				displayln("semwait  any|all|atomic  <sidlist>  [clear,]wait|nowait|timeout(<int>)");
			else if (verb.equals("queaccess"))
				displayln("queaccess  <name>");
			else if (verb.equals("quebrowse"))
				displayln("quebrowse  <msghdr var>  time+|time-|prio+|prio-");
			else if (verb.equals("quecopy"))
				displayln("quecopy  <msghdr var>  <offset>  <length>");
			else if (verb.equals("quecreate"))
				displayln("quecreate  <name>  <maxmsgs>|nolimit  <maxbytes>|nolimit");
			else if (verb.equals("quedelete"))
				displayln("quedelete  <qid>");
			else if (verb.equals("quedestroy"))
				displayln("quedestroy  <qid>");
			else if (verb.equals("quefreeze"))
				displayln("quefreeze  (no parameters)");
			else if (verb.equals("queget"))
				displayln("queget <msghdr var> <queselcode> <qidlist> [noremove,]wait|nowait|timeout(<int>)");
			else if (verb.equals("queinfoque"))
				displayln("queinfoque  <qid>");
			else if (verb.equals("queinfosys"))
				displayln("queinfosys  (no parameters)");
			else if (verb.equals("queinfouser"))
				displayln("queinfouser  <uid>");
			else if (verb.equals("quemsghdrdup"))
				displayln("quemsghdrdup  <msghdr var>  <msghdr var>");
			else if (verb.equals("quepurge"))
				displayln("quepurge  <qid>");
			else if (verb.equals("queput"))
			{
				displayln("queput <msghdr var> <queselcode> <qidlist> <priority> <blockopt>");
				displayln("<blockopt> = wait|nowait|timeout(<int>)|replace_ea|replace_la");
				displayln("             |replace_hp|replace_lp|replicate");
			}
			else if (verb.equals("queread"))
				displayln("queread  <msghdr var>");
			else if (verb.equals("quereceive"))
				displayln("quereceive  <queselcode>  <qidlist>  wait|nowait|timeout(<int>)");
			else if (verb.equals("queremove"))
				displayln("queremove  <msghdr var>");
			else if (verb.equals("quesend"))
			{
				displayln("quesend <queselcode> <qidlist> <priority> <msgtext> <blockopt>");
				displayln("<blockopt> = wait|nowait|timeout(<int>)|replace_ea|replace_la");
				displayln("             |replace_hp|replace_lp|replicate");
			}
			else if (verb.equals("quespool"))
				displayln("quespool  <qid>  <spoolfilename>");
			else if (verb.equals("quetrigger"))
				displayln("quetrigger  <sid>  <triggercode>  <trigger params...>");
			else if (verb.equals("queunfreeze"))
				displayln("queunfreeze  (no parameters)");
			else if (verb.equals("queunget"))
				displayln("queunget  <msghdr var>");
			else if (verb.equals("queuntrigger"))
				displayln("queuntrigger  <sid>  <trigercode>  <trigger params...>");
			else if (verb.equals("quewrite"))
				displayln("quewrite  <msghdr var>  <msgtext>  wait|nowait|timeout(<int>)");
			else if (verb.equals("msghdr"))
				displayln("msghdr  <msghdr var>");
			else if (verb.equals("memaccess"))
				displayln("memaccess  <name>");
			else if (verb.equals("memcreate"))
				displayln("memcreae  <name>  <size>");
			else if (verb.equals("memdelete"))
				displayln("memdelete  <mid>");
			else if (verb.equals("memdestroy"))
				displayln("memdestroy  <mid>");
			else if (verb.equals("memfreeze"))
				displayln("memfreeze  (no parameters)");
			else if (verb.equals("meminfomem"))
				displayln("meminfomem  <mid>");
			else if (verb.equals("meminfosec"))
				displayln("meminfosec  <section>");
			else if (verb.equals("meminfosys"))
				displayln("meminfosys  (no parameters)");
			else if (verb.equals("meminfouser"))
				displayln("meminfouser  <uid>");
			else if (verb.equals("memlock"))
				displayln("memlock  any|all|atomic  <midlist>  wait|nowait|timeout(<int>)");
			else if (verb.equals("memread"))
				displayln("memread  <mid>  <offset>  <length>  wait|nowait|timeout(<int>)");
			else if (verb.equals("memsection"))
				displayln("memsection  <variable>  <mid>");
			else if (verb.equals("memsecdef"))
				displayln("memsecdef  <section>");
			else if (verb.equals("memsecown"))
				displayln("memsecown  any|all|atomic  <midlist>  wait|nowait|timeout(<int>)");
			else if (verb.equals("memsecpriv"))
			{
				displayln("memsecpriv  <section>  <owner priv>  <other priv>");
				displayln("<priv> = rw|ro|wo|na|+r|+w|-r|-w|nc");
			}
			else if (verb.equals("memsecrel"))
				displayln("memsecrel  <midlist>");
			else if (verb.equals("memsecundef"))
				displayln("memsecundef  <section>");
			else if (verb.equals("memtrigger"))
				displayln("memtrigger  <sid>  <triggercode>  <trigger params...>");
			else if (verb.equals("memunfreeze"))
				displayln("memunfreeze  (no parameters)");
			else if (verb.equals("memunlock"))
				displayln("memunlock  <midlist>");
			else if (verb.equals("memuntrigger"))
				displayln("memuntrigger  <sid>  <triggercode>  <trigger params...>");
			else if (verb.equals("memwrite"))
				displayln("memwrite  <mid>  <offset>  <length>  <text>  wait|nowait|timeout(<int>)");
			else if (verb.equals("section"))
				displayln("section  <variable>");
			else if (verb.equals("xipcerror"))
				displayln("xipcerror  <int>");
			else displayln("I don't know the verb \"" + verb + "\", type ? or help for list");
		}
		catch (parseError e) // end-of-line
		{
			displayln("type: ? <verb>   or   help <verb> , where <verb> is one of:");
			displayln(" xipcabort       semrelease      quereceive      memlock     ");
			displayln(" xipcerror       semset          queremove       memread     ");
			displayln(" xipcfreeze      semunfreeze     quesend         memsecdef   ");
			displayln(" xipclogin       semwait         quespool        memsecown   ");
			displayln(" xipclogout                      quetrigger      memsecpriv  ");
			displayln(" xipcunfreeze    queaccess       queunfreeze     memsecrel   ");
			displayln("                 quebrowse       queunget        memsecundef ");
			displayln(" quit            quecopy         queuntrigger    memtrigger  ");
			displayln("                 quecreate       quewrite        memunfreeze ");
			displayln(" semaccess       quedelete       msghdr          memunlock   ");
			displayln(" semacquire      quedestroy                      memuntrigger");
			displayln(" semcancel       quefreeze       memaccess       memwrite    ");
			displayln(" semclear        queget          memcreate       section     ");
			displayln(" semcreate       queinfoque      memdelete        ");
			displayln(" semdelete       queinfosys      memdestroy       ");
			displayln(" semdestroy      queinfouser     memfreeze        ");
			displayln(" semfreeze       quemsghdrdup    meminfomem       ");
			displayln(" seminfosem      quepurge        meminfosec       ");
			displayln(" seminfosys      queput          meminfosys       ");
			displayln(" seminfouser     queread         meminfouser      ");
		}
	}


}	// end class jxipc
