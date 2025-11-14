# COMP 4300 - Assignment 1

## Program Specification
This is a c++ program using the sfml library which will read
descriptions of shapes from a configuration file, and then draw those shapes
to the screen. Each shape will have the following:
- name
- position
- speed
- color
- width
- height
- radius (circle)

The shapes should "bounce" off the sides of the window which contains them.
Each shape's name should be drawn on the exact center of the shape in the given
font size and color specified in the file.

We are also using an ImGui user interface which is able to:
- list all shapes and select any one of them to edit their properties
- toggle whether or not the selected shape will be drawn
- change the scale of the shape (0 to 4)
- change the x and y velocity of the shape (-8 to 8)
- change the color of the shape
- change the name of the shape

## Configuration File Specification
Each line of the configuration file specifies one of the config settings
of the program, with the first string in each line being the type of setting
that the rest of the line specifies. Lines in the config file can be one of
the following types, and lines can appear in any order in the file.

Window W H
- This line declares that the SFML Window must be constructed with width W
and height H, each of which will be integers

Font F S R G B
- This line defines the font which is used to draw text for this program.
The format of the line is as follows:
	Font File			F			std::string (it will have no spaces)
	Font Size			S			int
	RGB Color			(R,G,B)		int, int, int

Rectangle N X Y SX SY R G B W H
- Defines a RectangleShape with:
	Shape Name			Name		std::string
	Initial Position	(X,Y)		float, float
	Initial Speed		(SX,SY)		float, float
	RGB Color			(R,G,B)		int, int, int
	Size				(W,H)		float, float

Circle N X Y SX SY R G B R
- Defines a RectangleShape with:
	Shape Name			Name		std::string
	Initial Position	(X,Y)		float, float
	Initial Speed		(SX,SY)		float, float
	RGB Color			(R,G,B)		int, int, int
	Radius				R			float

## Assignment Hints
- In order to store an arbitrary number of shapes from the configuration file, you must store them in a container - std::vector is recommended
- Consider creating a custom class or struct which stores all of the properties of a shape that are not stored within the sf::Shape class itself
example: velocity, name, etc
- A shape will 'touch' the side of the window if its bounding box makes contact with it. Each shape's bounding rectange can be obtained via:
```
shape.getLocalBounds(); // .top, .left, .width, .height
```
- gives the LOCAL position of the (top, left) of the rectange
- LOCAL pos means it is relative to shape.getPosition(), not the window
- as well as the (width, height) size of the rectangle

```
shape.getGlobalBounds
```
will take into account any scale, translation, etc that has been applied to the shape. May be useful for the ui scaling

- Similarly, a sf::Text element's bounding rectangle can also be retrieved via text.getLocalBounds(), which will be needed to center the text properly within a shape.
- Use c++ file reading via std::ifstream