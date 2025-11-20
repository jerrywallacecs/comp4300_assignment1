#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

enum class ShapeType
{
	Rectangle,
	Circle
};

struct shapeInfo
{
	std::string name;

	float initialPosition[2] = {};

	float velocity[2] = {};

	float sfmlRGB[3] = { 255, 255, 255 };
	float guiRGB[3] = {};
	sf::Color rgbColor;

	float widthHeight[2] = { 0, 0 };
	float scale = 1;
	float radius = 0;
	int segments = 32;

	sf::RectangleShape rectangle;
	sf::CircleShape circle;

	ShapeType type;

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
			
			fin >> info.name >> info.initialPosition[0] >> info.initialPosition[1] >> info.velocity[0] >> info.velocity[1] >> info.sfmlRGB[0] >> info.sfmlRGB[1] >> info.sfmlRGB[2] >> info.widthHeight[0] >> info.widthHeight[1];

			sf::RectangleShape rectangle({ info.widthHeight[0], info.widthHeight[1] });
			info.rectangle = rectangle;
			info.hasRectangle = true;
			info.type = ShapeType::Rectangle;

			info.rgbColor = sf::Color(static_cast<uint8_t>(info.sfmlRGB[0]), static_cast<uint8_t>(info.sfmlRGB[1]), static_cast<uint8_t>(info.sfmlRGB[2]));
			
			info.guiRGB[0] = info.sfmlRGB[0] / 255;
			info.guiRGB[1] = info.sfmlRGB[1] / 255;
			info.guiRGB[2] = info.sfmlRGB[2] / 255;

			info.init(info.initialPosition[0], info.initialPosition[1], info.rgbColor);

			shapes.push_back(info);
		}

		if (section == "Circle")
		{
			shapeInfo info;

			fin >> info.name >> info.initialPosition[0] >> info.initialPosition[1] >> info.velocity[0] >> info.velocity[1] >> info.sfmlRGB[0] >> info.sfmlRGB[1] >> info.sfmlRGB[2] >> info.radius;

			sf::CircleShape circle(info.radius, info.segments);
			info.circle = circle;
			info.hasCircle = true;
			info.type = ShapeType::Circle;


			info.rgbColor = sf::Color(static_cast<uint8_t>(info.sfmlRGB[0]), static_cast<uint8_t>(info.sfmlRGB[1]), static_cast<uint8_t>(info.sfmlRGB[2]));

			info.guiRGB[0] = info.sfmlRGB[0] / 255;
			info.guiRGB[1] = info.sfmlRGB[1] / 255;
			info.guiRGB[2] = info.sfmlRGB[2] / 255;

			info.init(info.initialPosition[0], info.initialPosition[1], info.rgbColor);

			shapes.push_back(info);
		}
	}

	sf::Color fontColor(fontRGB[0], fontRGB[1], fontRGB[2], 255);

	auto window = sf::RenderWindow(sf::VideoMode({ windowWidth, windowHeight }), "Assignment 1 | SFML + ImGui");
	window.setFramerateLimit(60);

	// initialize imgui and create a clock used for its internal timing
	if (!ImGui::SFML::Init(window))
	{
		std::cout << "Could not initialize window\n";
		std::exit(-1);
	}

	sf::Clock deltaClock;

	bool drawText = true; // whether or not to draw the text

	sf::Font myFont;

	// attempt to load the font from a file
	if (!myFont.openFromFile(fontFile))
	{
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

	static int currentItem = 0; // index of currently selected item
	static const char* currentItemLabel = shapes[currentItem].name.c_str(); // we need the std::string to be a null-terminated c-style string to use the combo box

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

			// since the keypressed is an int under the hood, we can use a switch statement

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				// print the key that was pressed to the console
				std::cout << "Key pressed with code " << int(keyPressed->scancode) << '\n';

				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
				{
					window.close();
				}
			}
		}

		// update imgui for this frame with the time that the last frame took
		ImGui::SFML::Update(window, deltaClock.restart());

		// draw the UI
		ImGui::Begin("Shape Properties");

		// with intermediate mode (imgui), we just pass in the name and a pointer to the variable it operates on
		//ImGui::Text("Circle Settings");
		//ImGui::Checkbox("draw circle", &drawCircle);
		//ImGui::SameLine();
		//ImGui::Checkbox("draw text", &drawText);
		//ImGui::SliderFloat("radius", &circleRadius, 0.0f, 300.0f);
		//ImGui::SliderInt("sides", &circleSegments, 3, 64);
		//ImGui::SliderFloat("circle x speed", &circleSpeedX, -15.0f, 15.0f);
		//ImGui::SliderFloat("circle y speed", &circleSpeedY, -15.0f, 15.0f);

		/*
		for (int i = 0; i < shapes.size(); ++i)
		{
			if (shapes[i].hasRectangle)
			{
				ImGui::PushID(i); // unique ID for checkbox
				ImGui::Checkbox("draw rectangle", &shapes[i].drawShape);
				ImGui::SliderFloat2("rectangle velocity", shapes[i].velocity, -5.0f, 5.0f);

				ImGui::ColorEdit3("rectangle color", shapes[i].guiRGB);

				if (ImGui::Button("reset rectangle"))
				{
					shapes[i].rectangle.setPosition({ shapes[i].initialPosition[0], shapes[i].initialPosition[1] });
				}
				ImGui::PopID();
			}

			if (shapes[i].hasCircle)
			{
				ImGui::PushID(i); // unique ID for checkbox
				ImGui::Checkbox("draw circle", &shapes[i].drawShape);
				ImGui::SliderInt("sides", &shapes[i].segments, 3, 64);
				ImGui::SliderFloat2("circle velocity", shapes[i].velocity, -5.0f, 5.0f);

				ImGui::ColorEdit3("circle color", shapes[i].guiRGB);

				if (ImGui::Button("reset circle"))
				{
					shapes[i].circle.setPosition({ shapes[i].initialPosition[0], shapes[i].initialPosition[1] });
				}
				ImGui::PopID();
			}
		ImGui::PushID(i); // unique ID for checkbox
		ImGui::Checkbox("draw rectangle", &shapes[i].drawShape);
		ImGui::SliderFloat2("rectangle velocity", shapes[i].velocity, -5.0f, 5.0f);

		ImGui::ColorEdit3("rectangle color", shapes[i].guiRGB);

		if (ImGui::Button("reset rectangle"))
		{
			shapes[i].rectangle.setPosition({ shapes[i].initialPosition[0], shapes[i].initialPosition[1] });
		}
		ImGui::PopID();
		}

		*/

		/*
		ImGui::InputText("text", displayString, 255);
		if (ImGui::Button("set text"))
		{
			text.setString(displayString);
		}
		*/

		// preview shows the currently selected item
		if (ImGui::BeginCombo("Shape", currentItemLabel))
		{
			for (int i = 0; i < shapes.size(); i++)
			{
				ImGui::PushID(i);
				const bool isSelected = (currentItem == i);

				if (ImGui::Selectable(shapes[i].name.c_str(), isSelected))
				{
					currentItem = i;
					currentItemLabel = shapes[i].name.c_str(); // update preview
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}

		switch (shapes[currentItem].type)
		{
		case ShapeType::Rectangle:
			ImGui::Checkbox("Draw Shape", &shapes[currentItem].drawShape);
			ImGui::SliderFloat("Scale", &shapes[currentItem].scale, 0.1f, 5.0f);
			ImGui::SliderFloat2("Velocity", shapes[currentItem].velocity, -5.0f, 5.0f);
			ImGui::ColorEdit3("Color", shapes[currentItem].guiRGB);
			break;
		case ShapeType::Circle:
			ImGui::Checkbox("Draw Shape", &shapes[currentItem].drawShape);
			ImGui::SliderFloat("Radius", &shapes[currentItem].radius, 0.1f, 500.0f);
			ImGui::SliderInt("Sides", &shapes[currentItem].segments, 3, 64);
			ImGui::SliderFloat2("Velocity", shapes[currentItem].velocity, -5.0f, 5.0f);
			ImGui::ColorEdit3("Color", shapes[currentItem].guiRGB);
			break;
		}

		ImGui::End();
		
		// handle velocity & collision
		for (int i = 0; i < shapes.size(); ++i)
		{
			if (shapes[i].hasRectangle)
			{
				if (shapes[i].rectangle.getPosition().x < 0 || shapes[i].rectangle.getPosition().x + (shapes[i].widthHeight[0] * shapes[i].scale) > windowWidth)
					shapes[i].velocity[0] *= -1;

				if (shapes[i].rectangle.getPosition().y < 0 || shapes[i].rectangle.getPosition().y + (shapes[i].widthHeight[1] * shapes[i].scale) > windowHeight)
					shapes[i].velocity[1] *= -1;
			}

			if (shapes[i].hasCircle)
			{
				if (shapes[i].circle.getPosition().x < 0 || shapes[i].circle.getPosition().x + (2 * shapes[i].radius) > windowWidth)
					shapes[i].velocity[0] *= -1;

				if (shapes[i].circle.getPosition().y < 0 || shapes[i].circle.getPosition().y + (2 * shapes[i].radius) > windowHeight)
					shapes[i].velocity[1] *= -1;
			}
		}

		// set the circle properties, because they may have been updated with the ui
		for (int i = 0; i < shapes.size(); ++i)
		{
			// basic animation - move the circle each frame if it's still in frame
			shapes[i].animate(shapes[i].velocity[0], shapes[i].velocity[1]);
			
			// imgui uses 0-1 float for colors, sfml uses 0-255 for colors
			// we must convert from the ui floats to sfml uint8_t
			if (shapes[i].hasCircle)
			{
				shapes[i].circle.setPointCount(shapes[i].segments);
				shapes[i].circle.setFillColor(sf::Color(uint8_t(shapes[i].guiRGB[0] * 255), uint8_t(shapes[i].guiRGB[1] * 255), uint8_t(shapes[i].guiRGB[2] * 255)));
				shapes[i].circle.setRadius(shapes[i].radius);
			}

			if (shapes[i].hasRectangle)
			{
				shapes[i].rectangle.setFillColor(sf::Color(uint8_t(shapes[i].guiRGB[0] * 255), uint8_t(shapes[i].guiRGB[1] * 255), uint8_t(shapes[i].guiRGB[2] * 255)));
				shapes[i].rectangle.setScale({ shapes[i].scale, shapes[i].scale });
			}
		}

		

		// RENDERING
		window.clear();

		if (drawText)
		{
			window.draw(text);
		}

		// iterate through the vector and draw the shapes
		for (int i = 0; i < shapes.size(); ++i)
		{
			sf::Text label(myFont, shapes[i].name, fontSize);
			if (shapes[i].hasRectangle)
			{
				if (shapes[i].drawShape)
				{
					window.draw(shapes[i].rectangle);

					sf::FloatRect labelBounds = label.getLocalBounds();
					sf::FloatRect shapeBounds = shapes[i].rectangle.getGlobalBounds();

					sf::Vector2f shapeCenter = shapeBounds.position + (shapeBounds.size * 0.5f);
					sf::Vector2f labelCenter = labelBounds.position + (labelBounds.size * 0.5f);
					label.setOrigin(labelCenter);

					label.setPosition(shapeCenter);
					label.setFillColor(fontColor);
					window.draw(label);
				}
			}

			if (shapes[i].hasCircle)
			{
				if (shapes[i].drawShape)
				{
					window.draw(shapes[i].circle);

					sf::FloatRect labelBounds = label.getLocalBounds();
					sf::FloatRect shapeBounds = shapes[i].circle.getGlobalBounds();

					sf::Vector2f shapeCenter = shapeBounds.position + (shapeBounds.size * 0.5f);
					sf::Vector2f labelCenter = labelBounds.position + (labelBounds.size * 0.5f);

					label.setOrigin(labelCenter);
					label.setPosition(shapeCenter);
					label.setFillColor(fontColor);
					window.draw(label);
				}
			}

		}

		ImGui::SFML::Render(window); // draw the ui last so it's on top
		window.display(); // call the window display function
	}

	ImGui::SFML::Shutdown();
}