import java.lang.reflect.*;

/**
 * Implements callback functionality for Java. Callbacks are implemented using
 * reflection, so should be avoided if possible.
 * 
 * @author Brian Shields <http://clockworkgear.blogspot.com/>
 */
public class Callback {	
	public Callback(Class<?> type, String methodName, Object obj) {
		this.obj = obj;
		this.method = findMethod(type, methodName);
		this.types = method.getParameterTypes();
	}
	
	public Callback(Class<?> type, String methodName) {
		this.obj = null;
		this.method = findMethod(type, methodName);
		this.types = method.getParameterTypes();
	}
	
	public Callback bind(Object... args) {
		this.args = args;
		return this;
	}

	public Object call(Object... values) throws IllegalArgumentException,
												IllegalAccessException,
												InvocationTargetException {
		if (args == null) {
			args = values;
		}
		
		if (types.length != args.length) {
			throw new IllegalArgumentException("Wrong number of method parameters given. Found " + args.length + ", expected " + types.length);
		}

		return method.invoke(obj, args);
	}
	
	static Method findMethod(Class<?> inClass, String inMethodName) {
		for (Method m : inClass.getMethods()) {
			if (m.getName().equals(inMethodName)){
				return m;
			}
		}
		return null;
	}
	
	private Object obj;
	private Method method;
	private Class<?>[] types;
	private Object[] args = null;
}
