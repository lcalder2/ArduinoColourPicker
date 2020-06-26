import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;
import java.util.Iterator;
import java.awt.geom.Line2D;

class DrawingPanel extends JPanel implements MouseMotionListener, MouseListener
{
	private ArrayList <Shape> pointArrayList = new ArrayList<Shape>();
	private ArrayList <ArrayList <Shape>> previousLines = new ArrayList<ArrayList <Shape>>();
	
	private boolean drawing = false;
    Color selectedColour = new Color(0,100,100);
    
    public void setColour(int r, int g, int b) {
    	selectedColour = new Color(r,g,b);
    }
    
    DrawingPanel()
    {
        addMouseMotionListener(this);
        addMouseListener(this);
        
    }

    public void paintComponent(Graphics g)
    {
        super.paintComponent(g);
        
    	Graphics2D g2 = (Graphics2D) g;
        g2.setStroke(new BasicStroke(10));
        
        
        
        for (int i = 0; i < previousLines.size(); i++) {
        	ArrayList <Shape> previous = previousLines.get(i);
        	Iterator<Shape> pointIterator = previous.iterator();
        	
        	while( pointIterator.hasNext() )
            {
                Shape currentPoint = (Shape) pointIterator.next();
                if( pointIterator.hasNext() )
                {
                    Shape nextPoint = (Shape) pointIterator.next();
                    g2.setPaint(nextPoint.getColour());
                    g2.draw(new Line2D.Float(currentPoint.getPoint().x, currentPoint.getPoint().y, nextPoint.getPoint().x, nextPoint.getPoint().y));
                }
                
            }
        }
        
        if (drawing) {
        	Iterator<Shape> pointIterator = pointArrayList.iterator();
	        while( pointIterator.hasNext() )
	        {
	            Shape currentPoint = (Shape) pointIterator.next();
	            if( pointIterator.hasNext() )
	            {
	                Shape nextPoint = (Shape) pointIterator.next();
	                g2.setPaint(nextPoint.getColour());
	                g2.draw(new Line2D.Float(currentPoint.getPoint().x, currentPoint.getPoint().y, nextPoint.getPoint().x, nextPoint.getPoint().y));
	            }
	            
	        }
        }
        
        
    }


void clear() {
    //clear panel
    previousLines.clear();

}

@Override
public void mouseDragged(MouseEvent e) {
    drawing = true;
    Shape newShape = new Shape(e.getPoint(), selectedColour);
    pointArrayList.add( newShape );
    repaint();
    }

@Override
public void mouseMoved(MouseEvent e) {

    }

@Override
public void mouseClicked(MouseEvent e) {
	// TODO Auto-generated method stub
	
}

@Override
public void mouseEntered(MouseEvent e) {
	// TODO Auto-generated method stub
	
}

@Override
public void mouseExited(MouseEvent e) {
	// TODO Auto-generated method stub
	
}

@Override
public void mousePressed(MouseEvent e) {
	// TODO Auto-generated method stub
	
}

@Override
public void mouseReleased(MouseEvent e) {
	previousLines.add(pointArrayList);
	pointArrayList = new ArrayList<Shape>();
	drawing = false;
	repaint();
	
}
}