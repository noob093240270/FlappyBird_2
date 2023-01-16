#include <iostream>
#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <cstring>
#include <Windows.h>

template <class B, typename T>
bool instOf(T imp)
{
    if (dynamic_cast<B*>(imp) != nullptr)
    {
        return true;
    }
    return false;
}

sf::IntRect getIntRect(sf::RectangleShape& object)
{
    return sf::IntRect(object.getPosition().x, object.getPosition().y, object.getSize().x, object.getSize().y);
}

class Entity
{
public:
    virtual void draw() = 0;
    virtual void process() = 0;
    virtual void processEvent(sf::Event& event) = 0;  // обработка событий
    virtual bool intersect(sf::IntRect& intrect) = 0;
};


class Bird : public Entity
{
public:
    sf::RenderWindow& window;
    std::vector<Entity*>& entities;
    sf::Texture texture;
    sf::RectangleShape object;
    int birdIndex = 0;
    int birdSpeed = 22;

    sf::IntRect getTextureRectForBird(int index)
    {
        int row = index / 3;
        int col = index % 3;
        return sf::IntRect(34 * col, 24 * row, 34, 24);
    }

    void animation()
    {
        object.setTextureRect(getTextureRectForBird(birdIndex / 2));
        birdIndex++;
        if (birdIndex > 3 * 2)
        {
            birdIndex = 0;
        }
    }

    Bird(sf::RenderWindow& _window, std::vector<Entity*>& entities):window(_window),entities(entities)
    {
        texture = sf::Texture();
        texture.loadFromFile("C:\\Users\\facem\\source\\repos\\Flappy Bird\\resources\\resources\\imgs\\flappy.png");
        object = sf::RectangleShape(sf::Vector2f(50, 40));
        object.setTexture(&texture);
        object.setPosition(window.getSize().x / 5 - 25, window.getSize().y / 5 - 20);
        int birdIndex = 0;
        int birdSpeed = 39;
    }

    virtual void draw()
    {
        window.draw(object);
    }
    virtual void process()
    {
        animation();

        for (int i = 0; i < entities.size(); i++)
        {
            if (!instOf<Bird>(entities[i]))
            {
                sf::IntRect r = getIntRect(object);
                if (entities[i]->intersect(r))
                {
                    std::cout << " You have collided!" << std::endl;
                }
            }
        }

    }
    virtual void processEvent(sf::Event& event)
    {
        if (event.KeyPressed and event.key.code == sf::Keyboard::W)
        {
            if (object.getPosition().y > 0)
            {
                object.move(0, -birdSpeed);
            }
            else
            {
                object.setPosition(object.getPosition().x, 0);
            }
        }
        if (event.KeyPressed and event.key.code == sf::Keyboard::S)
        {
            if (object.getPosition().y < window.getSize().y - 265)
            {
                object.move(0, birdSpeed);
            }
            else
            {
                object.setPosition(object.getPosition().x, window.getSize().y - 265);
            }
        }
    }

    virtual bool intersect(sf::IntRect& intrect)
    {
        return getIntRect(object).intersects(intrect);
    }

};


class BackGround :public Entity
{
public:
    sf::RenderWindow& window;
    sf::Texture texture;
    sf::RectangleShape object;
    float coefBgrZoom;
    float bgrW;
    float coefBgrW;
    float bgrSpeed = 3;
    int numBgrW;

    BackGround(sf::RenderWindow& _window) : window(_window)
    {
        texture = sf::Texture();
        texture.loadFromFile("C:\\Users\\facem\\source\\repos\\Flappy Bird\\resources\\resources\\imgs\\1620826823_11-phonoteka_org-p-pikselnii-fon-zemli-18.jpg");
        texture.setRepeated(true);
        object = sf::RectangleShape(sf::Vector2f(window.getSize().x * 2, window.getSize().y));
        object.setTexture(&texture);
        object.setPosition(0, window.getSize().y - object.getSize().y);

        float coefBgrZoom = 1000 / window.getSize().y;
        float bgrW = 3227 / coefBgrZoom;
        float coefBgrW = window.getSize().x * 2 / bgrW;
        int numBgrW = window.getSize().x / bgrW;
        float bgrSpeed = 3;
        object.setTextureRect(sf::IntRect(0, 0, 3227 * coefBgrW, 820));
    }

    virtual void draw()
    {
        window.draw(object);
    }

    virtual void process()
    {
        float a = window.getSize().x;
        if (object.getPosition().x < -a)
        {
            object.setPosition(0, object.getPosition().y);
        }

        object.move(-bgrSpeed, 0);
    }

    virtual void processEvent(sf::Event& event)
    {

    }

    virtual bool intersect(sf::IntRect& intrect)
    {
        return false;
    }

};


class FPS : public Entity
{
    sf::RenderWindow& window;
    sf::Text fps_text;
    sf::Clock clock;
    int fps, count_fps;
public:
    FPS(sf::RenderWindow& _window,sf::Font& font) : window(_window)
    {
        fps = 0;
        count_fps = 0;

        sf::Text fps_text;
        fps_text = sf::Text();
        fps_text.setFont(font);
        fps_text.setFillColor(sf::Color(255, 166, 0));
        fps_text.setCharacterSize(20);
    }

    virtual void draw()
    {
        window.draw(fps_text);
    }

    virtual void process()
    {
        sf::Time elapsed = clock.getElapsedTime();
        if (elapsed.asMilliseconds() >= 1000)
        {
            fps = count_fps;
            count_fps = -1;
            clock.restart();
        }
        fps_text.setString("FPS: " + std::to_string(fps));
        count_fps++;
    }

    virtual void processEvent(sf::Event& event)
    {

    }

    virtual bool intersect(sf::IntRect& intrect)
    {
        return false;
    }

};


class Column : public Entity
{
public:
    sf::RenderWindow& window;
    sf::Texture texture;
    sf::RectangleShape objectTop;
    sf::RectangleShape objectBot;
    int speed, spaceBetwObj, spaceBetwCol, width;

   

    void animation()
    {
        objectTop.move(-speed, 0);
        objectBot.move(-speed, 0);
        if (objectTop.getPosition().x + objectTop.getSize().x < 0)
        {
            objectTop.setPosition(window.getSize().x + spaceBetwCol, 0);
            objectTop.setSize(sf::Vector2f(width, randomH()));

            objectBot.setSize(sf::Vector2f(width, window.getSize().y - objectTop.getSize().y - spaceBetwObj));
            objectBot.setPosition(objectTop.getPosition().x, objectTop.getSize().y + spaceBetwObj);

            countCol += 1;

        }
    }

    int randomH()
    {
        int n = rand() % (window.getSize().y - 350) +70;
        if (n < 240)
        {
            return n;
        }
    }

    static int countCol;

    Column(sf::RenderWindow& _window, int startPosX) :window(_window)
    {
        texture.loadFromFile("C:\\Users\\facem\\source\\repos\\Flappy Bird\\resources\\resources\\imgs\\column.png");
        spaceBetwObj = 100;
        width = 80;
        spaceBetwCol = 200;
        speed = 2;

        objectTop.setTexture(&texture);
        objectTop.setPosition(startPosX,0);
        objectTop.setSize(sf::Vector2f(width, randomH()-80));

        objectBot.setTexture(&texture);
        objectBot.setPosition(objectTop.getPosition().x, objectTop.getSize().y + spaceBetwObj);
        objectBot.setSize(sf::Vector2f(width,window.getSize().y - objectTop.getSize().y - spaceBetwObj));
    }

    virtual void draw()
    {
        window.draw(objectTop);
        window.draw(objectBot);
    }
    virtual void process()
    {
        animation();
    }
    virtual void processEvent(sf::Event& event)
    {
        
    }
    virtual bool intersect(sf::IntRect& intrect)
    {
        if (getIntRect(objectTop).intersects(intrect) || getIntRect(objectBot).intersects(intrect) == 1)
        {
            Column::countCol = 0;
            return getIntRect(objectTop).intersects(intrect) || getIntRect(objectBot).intersects(intrect);
        }
        return getIntRect(objectTop).intersects(intrect) || getIntRect(objectBot).intersects(intrect);
    }
};


class Statistic : public Entity
{
    sf::RenderWindow& window;
    sf::Text text;

public:
    Statistic(sf::RenderWindow& _window, sf::Font& font) : window(_window)
    {
        text.setFont(font);
        text.setFillColor(sf::Color(255, 166, 0));
        text.setCharacterSize(20);
        text.move(0, 15);
    }

    virtual void draw()
    {
        window.draw(text);
    }

    virtual void process()
    {
        text.setString("COUNT :  " + std::to_string(Column::countCol));
    }

    virtual void processEvent(sf::Event& event)
    {

    }
    virtual bool intersect(sf::IntRect& intrect)
    {
        return false;
    }
};


int Column::countCol = 0;
int main()
{
    const int WIDTH = 1200;
    const int HEIGHT = 800;
    int FPS_MX = 80;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Flappy bird");
    window.setFramerateLimit(FPS_MX);
    sf::Event event;
    std::vector<Entity*> entity;
    sf::Font fontLight;
    fontLight.loadFromFile("C:\\Users\\facem\\source\\repos\\Flappy Bird\\resources\\resources\\fonts\\Roboto-Light-webfont.woff");
    
    entity.push_back(new BackGround(window));
    entity.push_back(new Column(window, WIDTH + 300 * 0));
    entity.push_back(new Column(window, WIDTH + 300 * 1));
    entity.push_back(new Column(window, WIDTH + 300 * 2));
    entity.push_back(new Column(window, WIDTH + 300 * 3));
    entity.push_back(new Column(window, WIDTH + 300 * 4));
    
    /*FPS fps(window, fontLight);
    Bird bird(window);
    BackGround background(window);*/
    entity.push_back(new FPS(window, fontLight));
    entity.push_back(new Statistic(window, fontLight));
    entity.push_back(new Bird(window,entity));


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            //bird.processEvent(event);
            for (int i = 0; i < entity.size(); i++)
            {
                entity[i]->processEvent(event);
            }
        }

        
        /*fps.process();
        bird.process();
        background.process();*/
        for (int i = 0; i < entity.size(); i++)
        {
            entity[i]->process();
        }

        window.clear({ 250,250,250 });
        
        /*background.draw();
        bird.draw();
        fps.draw();*/
        for (int i = 0; i < entity.size(); i++)
        {
            entity[i]->draw();
        }

        window.display();
    }
    

    
}











