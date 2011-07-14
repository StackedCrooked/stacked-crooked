
public class Test {
	
	public String toString() {
		return "Test!";
	}
	
	public static String join(String a, String b) {
		return a + " " + b;
	}
	
	public int timesTwo(int n) {
		return 2 * n;
	}
	
	public int sum(int a, int b) {
		return a + b;
	}
	
	public void run() throws Exception
	{		
		System.out.print("toString: ");
		Callback str = new Callback(Test.class, "toString", this);
		System.out.println(str.call());
		
		System.out.print("timesTwo: ");
		Callback times2 = new Callback(Test.class, "timesTwo", this);		
		System.out.println(times2.call(7));
		
		System.out.print("sum: ");
		Callback summator = new Callback(Test.class, "sum", this).bind(3, 4);
		System.out.println(summator.call());
		
		System.out.print("join: ");
		Callback joiner = new Callback(Test.class, "join").bind("John", "Smith");
		System.out.println(joiner.call());
	}
}
