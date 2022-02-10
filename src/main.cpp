#include "os_finder.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#include "asset_resolver.h"

#ifdef OS_SWITCH
	#include <switch.h>
#endif

extern int main()
{
#ifdef OS_SWITCH
    std::cout << "OS is Switch" << std::endl;
	// Setup NXLink
	socketInitializeDefault();
	nxlinkStdio();

	Result rc = romfsInit();
#endif

#ifdef OS_WINDOWS
    std::cout << "OS is Windows" << std::endl;
#endif

#ifdef OS_MAC
    std::cout << "OS is Mac" << std::endl;
#endif

#ifdef OS_UNIX
    std::cout << "OS is Unix" << std::endl;
#endif

#ifndef OS_SWITCH
	sf::Music music;

	if(!music.openFromFile(asset("greenlife.ogg")))
		return -1;

	sf::SoundBuffer buffer;

	if(!buffer.loadFromFile(asset("jump.ogg")))
		return -1;

	sf::Sound sound;
	sound.setBuffer(buffer);

	music.setVolume(100.0f);
#endif

	sf::Font font;
	if(!font.loadFromFile(asset("sans.ttf")))
	{
		std::cout << "Font not loaded" << std::endl;
		return EXIT_FAILURE;
	}

	sf::VideoMode mode;

#ifdef OS_SWITCH
	mode = sf::VideoMode::getDesktopMode();
#else
	mode = sf::VideoMode(640, 480, 32);
#endif

	sf::Text message("Welcome to SFML valley.", font);

	sf::RenderWindow window(mode, "SFML Game Template Project");
	sf::Clock deltaClock;

	std::cout << "Loading textures" << std::endl;
	sf::Texture playerTex;
	if(!playerTex.loadFromFile(asset("player.png"))) {
		std::cout << "Texture failed to load" << std::endl;
		return EXIT_FAILURE;
	}
	sf::Texture backgroundTex;
	if(!backgroundTex.loadFromFile(asset("background.jpg"))) {
		std::cout << "Texture failed to load" << std::endl;
		return EXIT_FAILURE;
	}
	sf::Sprite player;
	player.setTexture(playerTex);
	player.setOrigin(sf::Vector2f(player.getTextureRect().getSize() / 2));
	player.setPosition(sf::Vector2f(300, 200));
	sf::Sprite background;
	background.setTexture(backgroundTex);

#ifndef OS_SWITCH
	sf::CircleShape circle(40.f);
	circle.setPosition(sf::Vector2f(100., 100.));

	circle.setFillColor(sf::Color::Yellow);
#endif
	float speed = 0.02f;

	bool wasPlayPressed = false;
	bool wasStopPressed = false;

	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect("192.168.0.5", 2020);
	if (status != sf::Socket::Done)
	{
		std::cout << "FUCK!!! NETWORK NOT FOUND !! BOOT UP THE SERVER !!!!" << std::endl;
	}
	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			if(e.type == sf::Event::EventType::Closed) {
				std::cout << "Good bye!" << std::endl;
				window.close();
				break;
			}
		}

		if(!window.isOpen())
			break;

		window.clear();

		sf::Vector2f vec = sf::Vector2f(window.getView().getSize());
		vec.x /= static_cast<float>(background.getTextureRect().getSize().x);
		vec.y /= static_cast<float>(background.getTextureRect().getSize().y);

		background.setScale(vec);
		window.draw(background);
		window.draw(message);
		player.setScale(sf::Vector2f(0.25f, 0.25f));
		window.draw(player);
#ifndef OS_SWITCH
		window.draw(circle);
#endif
		sf::Time dtTime = deltaClock.restart();
		float dt = dtTime.asSeconds();

		sf::Vector2f spritePos = player.getPosition();
		sf::Vector2f msgPos = message.getPosition();

		bool playPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P);
		bool stopPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);

#ifndef OS_SWITCH
		if(sf::Touch::isDown(1) || (playPressed && !wasPlayPressed))
		{
			music.play();
			std::cout << "Playing music!" << std::endl;
		}
		else if(sf::Touch::isDown(2) || (stopPressed && !wasStopPressed))
		{
			music.pause();
			std::cout << "Stopping music!" << std::endl;
		}
#endif

		wasPlayPressed = playPressed;
		wasStopPressed = stopPressed;

		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			auto mousePos = sf::Mouse::getPosition(window);
			spritePos.x = (float) mousePos.x;
			spritePos.y = (float) mousePos.y;
#ifndef OS_SWITCH
			sound.play();
#endif
		}
		else if(sf::Touch::isDown(0))
		{
			auto touchPos = sf::Touch::getPosition(0);
			spritePos.x = (float) touchPos.x;
			spritePos.y = (float) touchPos.y;
#ifndef OS_SWITCH
			sound.play();
#endif
		}
		else
		{
#ifndef OS_SWITCH
			spritePos.x += speed * dt * sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
			spritePos.y += speed * dt * -sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
			msgPos.x += speed * dt * sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U);
			msgPos.y += speed * dt * -sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V);
#endif
		}

		player.setPosition(spritePos);
		message.setPosition(msgPos);

		window.display();
	}

#ifdef OS_SWITCH
	socketExit();
#endif
	return EXIT_SUCCESS;
}
