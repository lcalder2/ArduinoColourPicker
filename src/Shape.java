import java.awt.Color;
import java.awt.Point;

public class Shape {
	Point point;
	Color colour;
	public Shape(Point p, Color c) {
		point = p;
		colour = c;
	}
	public void setColour(Color value) {
		colour = value;
	}
	
	public void setPoint(Point value) {
		point = value;
	}
	
	public Color getColour() {
		return colour;
	}
	
	public Point getPoint() {
		return point;
	}
}
