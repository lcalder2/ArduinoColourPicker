import java.io.BufferedReader;
import java.io.FileReader; 
import java.util.Iterator; 
import java.util.Map;

import javax.swing.JFrame;

import org.json.simple.JSONArray; 
import org.json.simple.JSONObject; 
import org.json.simple.parser.*; 
import java.io.InputStreamReader;
import java.io.OutputStream;
import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 
import java.util.Enumeration;


public class SerialReader implements SerialPortEventListener {
	
	private final ColourWatcher callback;
	
	SerialPort serialPort;
	private static final String PORT_NAMES[] = { 
			"COM3"
	};
	private BufferedReader input;
	private OutputStream output;
	private static final int TIME_OUT = 2000;
	private static final int DATA_RATE = 115200;
	private int r = 100;
	private int g = 0;
	private int b = 70;
	
	

	public void initialize() {

		CommPortIdentifier portId = null;
		Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

		//First, Find an instance of serial port as set in PORT_NAMES.
		while (portEnum.hasMoreElements()) {
			CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
			for (String portName : PORT_NAMES) {
				if (currPortId.getName().equals(portName)) {
					portId = currPortId;
					break;
				}
			}
		}
		if (portId == null) {
			System.out.println("Could not find COM port.");
			return;
		}

		try {
			// open serial port, and use class name for the appName.
			serialPort = (SerialPort) portId.open(this.getClass().getName(),
					TIME_OUT);

			// set port parameters
			serialPort.setSerialPortParams(DATA_RATE,
					SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);

			// open the streams
			input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
			output = serialPort.getOutputStream();

			// add event listeners
			serialPort.addEventListener(this);
			serialPort.notifyOnDataAvailable(true);
		} catch (Exception e) {
			System.err.println(e.toString());
		}
	}

	public synchronized void close() {
		if (serialPort != null) {
			serialPort.removeEventListener();
			serialPort.close();
		}
	}

	public synchronized void serialEvent(SerialPortEvent oEvent) {
		if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
			try {
				System.out.println("Trying to read: ");
				String inputLine=input.readLine();
				System.out.println(inputLine);
				
				// parsing JSON
		        Object obj = new JSONParser().parse(inputLine); 
		        JSONObject jo = (JSONObject) obj; 
		          
		        long r_1 = (long) jo.get("r"); 
		        long g_1 = (long) jo.get("g");  
		        long b_1 = (long) jo.get("b"); 

				r = (int)r_1;
				g = (int)g_1;
				b = (int)b_1;
				
				callback.callback(r,g,b);
				
			} catch (Exception e) {
				System.out.println("Didnt work");
				System.err.println(e.toString());
			}
		}
	}
	
	public SerialReader(ColourWatcher callback) {

        this.callback = callback;
    }

}
