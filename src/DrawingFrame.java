import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

class DrawingFrame extends JFrame implements ActionListener
{
    final int SMALL = 4;
    final int MEDIUM = 8;
    final int LARGE = 10;

    private DrawingPanel drawPanel = new DrawingPanel();
    Container con = getContentPane();
    BorderLayout layout = new BorderLayout();
    JMenuBar mainMenuBar = new JMenuBar();


    DrawingFrame()
    {
        con.setLayout(layout);
        drawPanel = new DrawingPanel();
        drawPanel.setBackground( Color.WHITE );
        drawPanel.setVisible(true);
        this.add(drawPanel);
        drawPanel.setPreferredSize(new Dimension(1000,600));

    }


    @Override
    public void actionPerformed(ActionEvent e) {
       String arg = e.getActionCommand();
        if(arg.equals("Exit") )
        {
            System.exit(0);
        }
        if(arg.equals("Clear"))
        {
           drawPanel.clear();
        }
    }
    
    public void setColour(int r, int g, int b) {
    	drawPanel.setColour(r,g,b);
    }
}