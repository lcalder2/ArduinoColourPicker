import javax.swing.*;



public class DrawingProgram {

    public static void main(String[] args) {
    	
        DrawingFrame frame = new DrawingFrame();
        frame.setTitle("Drawing Program");
        frame.setSize(700, 600);
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
        
        SerialReader reader = new SerialReader((int r, int g, int b) -> {
        	System.out.println("Changing colour");
        	frame.setColour(r,g,b);
        });
        reader.initialize();

    }
}
