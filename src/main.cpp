#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

int main(int argc, char* argv[])
{
	// FILE HANDLING
	std::string filename{ "config/config.txt" };
	std::ifstream fin(filename);
	std::string section;

	// window variables
	uint16_t windowWidth = 1280;
	uint16_t windowHeight = 720;

	// font variables
	std::string fontFile;
	int fontSize{ 0 };
	int fontRGB[3] = { 0, 0, 0 };

	struct shapeInfo
	{
		std::string name;

		float initialPosX{};
		float initialPosY{};

		float speedX{};
		float speedY{};

		unsigned int rgb[3] = { 255, 255, 255 };
		sf::Color rgbColor;

		float widthHeight[2] = { 0, 0 };
		float radius = 0;
		int segments = 32;

		sf::RectangleShape rectangle;
		sf::CircleShape circle;

		bool hasRectangle = false;
		bool hasCircle = false;

		bool drawShape = true;

		void init(float posx, float posy, sf::Color rgb)
		{
			if (hasRectangle)
			{
				rectangle.setPosition({ posx, posy });
				rectangle.setFillColor(rgb);
			}

			if (hasCircle)
			{
				circle.setPosition({ posx, posy });
				circle.setFillColor(rgb);
			}
		}

		void animate(float speedx, float speedy)
		{
			if (hasRectangle)
				rectangle.setPosition({ rectangle.getPosition().x + speedx, rectangle.getPosition().y + speedy });
			if (hasCircle)
				circle.setPosition({ circle.getPosition().x + speedx, circle.getPosition().y + speedy });
		}

	};

	// container for all shapes
	std::vector<shapeInfo> shapes = {};
	
	if (!fin)
	{
		std::cerr << "could not open file" << '\n' << "current working dir: " << std::filesystem::current_path() << '\n';
		return -1;
	}

	while (fin >> section)
	{
		if (section == "Window")
		{
			fin >> windowWidth >> windowHeight;
		}

		if (section == "Font")
		{
			fin >> fontFile >> fontSize >> fontRGB[0] >> fontRGB[1] >> fontRGB[2];
		}

		if (section == "Rectangle")
		{
			shapeInfo info;
			
			fin >> info.name >> info.initialPosX >> info.initialPosY >> info.speedX >> info.speedY >> info.rgb[0] >> info.rgb[1] >> info.rgb[2] >> info.widthHeight[0] >> info.widthHeight[1];

			sf::RectangleShape rectangle({ info.widthHeight[0], info.widthHeight[1] });
			info.rectangle = rectangle;
			info.hasRectangle = true;

			info.rgbColor = sf::Color(static_cast<uint8_t>(info.rgb[0]), static_cast<uint8_t>(info.rgb[1]), static_cast<uint8_t>(info.rgb[2]));

			info.init(info.initialPosX, info.initialPosY, info.rgbColor);

			shapes.push_back(info);
		}

		if (section == "Circle")
		{
			shapeInfo info;

			fin >> info.name >> info.initialPosX >> info.initialPosY >> info.speedX >> info.speedY >> info.rgb[0] >> info.rgb[1] >> info.rgb[2] >> info.radius;

			sf::CircleShape circle(info.radius, info.segments);
			info.circle = circle;
			info.hasCircle = true;


			info.rgbColor = sf::Color(static_cast<uint8_t>(info.rgb[0]), static_cast<uint8_t>(info.rgb[1]), static_cast<uint8_t>(info.rgb[2]));

			info.init(info.initialPosX, info.initialPosY, info.rgbColor);

			shapes.push_back(info);
		}
	}

	sf::Color fontColor(fontRGB[0], fontRGB[1], fontRGB[2], 255);

	// as of sfml 3, sf::VideoMode now takes in a single object instead of sepeate ints
	auto window = sf::RenderWindow(sf::VideoMode({ windowWidth, windowHeight }), "Assignment 1 | SFML + ImGui");
	window.setFramerateLimit(60); // limit frame rate to 60fps

	// initialize imgui and create a clock used for its internal timing
	if (!ImGui::SFML::Init(window))
	{
		std::cout << "Could not initialize window\n";
		std::exit(-1);
	}

	sf::Clock deltaClock;

	// the imgui color {r, g, b} wheel requires floats from 0-1
	// sfml will require instead of uint8_t from 0-255
	// this is the only really annoying conversion between sfml and imgui
	float c[3] = { 0.0f, 1.0f, 1.0f };

	// lets make a shape that we will draw to the screen
	float circleRadius = 50; // radius to draw the circle
	int circleSegments = 32; // number of segments to draw the circle with
	float circleSpeedX = 1.0f; // we will use this to move the circle later
	float circleSpeedY = 0.5f; // we will read these values from the file
	bool drawCircle = true; // whether or not to draw the circle
	bool drawText = true; // whether or not to draw the text

	// create the sfml circle shape based on our params
	//sf::CircleShape circle(circleRadius, circleSegments);
	//circle.setPosition({ 10.0f, 10.0f });

	// load a font so we can display some text
	sf::Font myFont;

	// attempt to load the font from a file
	if (!myFont.openFromFile(fontFile))
	{
		// if we can't load the font, print an error to the console and exit
		std::cerr << "Could not load font!\n";
		std::exit(-1);
	}

	// set up the text object that will be drawn in the bottom of the screen
	sf::Text text(myFont, "Sample Text", fontSize);

	// position the top-left corner of the text so that the text aligns on the bottom
	// text character size is in pixels, so move the text up from the bottom by its height
	text.setPosition({ 0, windowHeight - (float)text.getCharacterSize() });
	text.setFillColor(fontColor);

	// set up a character array to set the text
	char displayString[255] = "Sample Text";

	// main loop - continues for each frame while window is open
	while (window.isOpen())
	{
		// event handling
		while (const std::optional event = window.pollEvent())
		{
			// pass the event to imgui to be parsed
			ImGui::SFML::ProcessEvent(window, *event);

			// this event triggers when the window is closed
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			// this event is triggered when a key is pressed
			/*
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				// print the key that was pressed to the console
				std::cout << "Key pressed with code " << int(keyPressed->scancode) << '\n';

				if (keyPressed->scancode == sf::Keyboard::Scancode::X)
				{
					circleSpeedX *= -1.0f;
				}
			}
			*/
		}

		// update imgui for this frame with the time that the last frame took
		ImGui::SFML::Update(window, deltaClock.restart());

		// draw the UI
		ImGui::Begin("window title");
		ImGui::Text("window text");

		// with intermediate mode (imgui), we just pass in the name and a pointer to the variable it operates on
		//ImGui::Text("Circle Settings");
		//ImGui::Checkbox("draw circle", &drawCircle);
		//ImGui::SameLine();
		//ImGui::Checkbox("draw text", &drawText);
		//ImGui::SliderFloat("radius", &circleRadius, 0.0f, 300.0f);
		//ImGui::SliderInt("sides", &circleSegments, 3, 64);
		//ImGui::SliderFloat("circle x speed", &circleSpeedX, -15.0f, 15.0f);
		//ImGui::SliderFloat("circle y speed", &circleSpeedY, -15.0f, 15.0f);

		for (int i = 0; i < shapes.size(); ++i)
		{
			if (shapes[i].hasRectangle)
			{
				ImGui::PushID(i); // unique ID for checkbox
				ImGui::Checkbox("draw rectangle", &shapes[i].drawShape);
				ImGui::PopID();
			}

			if (shapes[i].hasCircle)
			{
				ImGui::PushID(i); // unique ID for checkbox
				ImGui::Checkbox("draw circle", &shapes[i].drawShape);
				ImGui::SliderInt("sides", &shapes[i].segments, 3, 64);
				ImGui::SliderFloat("circle x speed", &shapes[i].speedX, -15.0f, 15.0f);
				ImGui::SliderFloat("circle y speed", &shapes[i].speedY, -15.0f, 15.0f);
				ImGui::PopID();
			}
				
		}

		// debug ui
		//ImGui::Checkbox("draw first rectangle in shapes", &shapes[0].drawRectangle);
		//ImGui::Checkbox("draw second rectangle in shapes", &shapes[1].drawRectangle);

		ImGui::ColorEdit3("color circle", c);
		ImGui::InputText("text", displayString, 255);
		if (ImGui::Button("set text"))
		{
			text.setString(displayString);
		}
		ImGui::SameLine();
		/*
		if (ImGui::Button("reset circle"))
		{
			circle.setPosition({ 0,0 });
		}
		*/
		ImGui::End();

		// set the circle properties, because they may have been updated with the ui
		//circle.setPointCount(circleSegments);
		//circle.setRadius(circleRadius);
		//float circleDiameter = circleRadius * 2;

		// imgui uses 0-1 float for colors, sfml uses 0-255 for colors
		// we must convert from the ui floats to sfml uint8_t
		//circle.setFillColor(sf::Color(uint8_t(c[0] * 255), uint8_t(c[1] * 255), uint8_t(c[2] * 255)));

		// basic animation - move the circle each frame if it's still in frame
		//circle.setPosition({ circle.getPosition().x + circleSpeedX, circle.getPosition().y + circleSpeedY });

		for (int i = 0; i < shapes.size(); ++i)
		{
			shapes[i].animate(shapes[i].speedX, shapes[i].speedY);
			
			if (shapes[i].hasCircle)
			{
				shapes[i].circle.setPointCount(shapes[i].segments);
			}
		}


		// COLLISION HANDLING
		// circle.getPosition() + circleDiameter for bottom and right
		// circle.getPosition() for top and left
		/*
		if (circle.getPosition().x < 0 || circle.getPosition().x + circleDiameter > windowWidth)
		{
			circleSpeedX *= -1;
		}

		if (circle.getPosition().y < 0 || circle.getPosition().y + circleDiameter > windowHeight)
		{
			circleSpeedY *= -1;
		}
		*/

		for (int i = 0; i < shapes.size(); ++i)
		{
			if (shapes[i].hasRectangle)
			{
				if (shapes[i].rectangle.getPosition().x < 0 || shapes[i].rectangle.getPosition().x + shapes[i].widthHeight[0] > windowWidth)
					shapes[i].speedX *= -1;

				if (shapes[i].rectangle.getPosition().y < 0 || shapes[i].rectangle.getPosition().y + shapes[i].widthHeight[1] > windowHeight)
					shapes[i].speedY *= -1;
			}

			if (shapes[i].hasCircle)
			{
				if (shapes[i].circle.getPosition().x < 0 || shapes[i].circle.getPosition().x + (2 * shapes[i].radius) > windowWidth)
					shapes[i].speedX *= -1;

				if (shapes[i].circle.getPosition().y < 0 || shapes[i].circle.getPosition().y + (2 * shapes[i].radius) > windowHeight)
					shapes[i].speedY *= -1;
			}
		}

		// RENDERING
		window.clear(); // clear the window of anything previously drawn
		/*
		if (drawCircle) // draw the circle if the boolean is true
		{
			window.draw(circle);
		}
		*/

		if (drawText)
		{
			window.draw(text);
		}

		// iterate through the vector and draw the shapes
		for (int i = 0; i < shapes.size(); ++i)
		{
			if (shapes[i].hasRectangle)
			{
				if (shapes[i].drawShape)
					window.draw(shapes[i].rectangle);
			}

			if (shapes[i].hasCircle)
			{
				if (shapes[i].drawShape)
					window.draw(shapes[i].circle);
			}
		}

		ImGui::SFML::Render(window); // draw the ui last so it's on top
		window.display(); // call the window display function
	}

	ImGui::SFML::Shutdown();
}