#include <SFML/Graphics.hpp>
#include <ctime>
#include <list>
#include <cmath>

using namespace sf;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

float DEGTORAD = 0.017453f;

class Animation {
public:
    float Frame, speed;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation() {}

    Animation(Texture &textureRef, int xPosition, int yPosition, int width, int height, int count, float Speed) {
        Frame = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(xPosition + i * width, yPosition, width, height));

        sprite.setTexture(textureRef);
        sprite.setOrigin(width / 2, height / 2);
        sprite.setTextureRect(frames[0]);
    }


    void update() {
        Frame += speed;
        int n = frames.size();
        if (Frame >= n) Frame -= n;
        if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
    }

    bool isEnd() {
        return Frame + speed >= frames.size();
    }

};


class Entity {
public:
    float xPosition, yPosition, dx, dy, R, angle;
    bool life;
    std::string name;
    Animation anim;

    Entity() {
        life = 1;
    }

    void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1) {
        anim = a;
        xPosition = X;
        yPosition = Y;
        angle = Angle;
        R = radius;
    }

    virtual void update() {};

    void draw(RenderWindow &app) {
        anim.sprite.setPosition(xPosition, yPosition);
        anim.sprite.setRotation(angle + 90);
        app.draw(anim.sprite);

        CircleShape circle(R);
        circle.setFillColor(Color(255, 0, 0, 170));
        circle.setPosition(xPosition, yPosition);
        circle.setOrigin(R, R);
    }

    virtual ~Entity() {};
};


class Asteroid : public Entity {
public:
    Asteroid() {
        dx = rand() % 8 - 4;
        dy = rand() % 8 - 4;
        name = "Asteroid";
    }

    void update() {
        xPosition += dx;
        yPosition += dy;

        if (xPosition > WINDOW_WIDTH) xPosition = 0;
        if (xPosition < 0) xPosition = WINDOW_WIDTH;
        if (yPosition > WINDOW_HEIGHT) yPosition = 0;
        if (yPosition < 0) yPosition = WINDOW_HEIGHT;
    }

};


class Bullet : public Entity {
public:
    Bullet() {
        name = "Bullet";
    }

    void update() {
        dx = cos(angle * DEGTORAD) * 6;
        dy = sin(angle * DEGTORAD) * 6;
        angle+=rand()%7-3;  /*try this*/
        xPosition += dx;
        yPosition += dy;

        if (xPosition > WINDOW_WIDTH || xPosition < 0 || yPosition > WINDOW_HEIGHT || yPosition < 0) life = 0;
    }

};


class Hamster : public Entity {
public:
    bool thrust;

    Hamster() {
        name = "Hamster";
    }

    void update() {
        if (thrust) {
            dx += cos(angle * DEGTORAD) * 0.2;
            dy += sin(angle * DEGTORAD) * 0.2;
        } else {
            dx *= 0.99;
            dy *= 0.99;
        }

        int maxSpeed = 15;
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed) {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        xPosition += dx;
        yPosition += dy;

        if (xPosition > WINDOW_WIDTH) xPosition = 0;
        if (xPosition < 0) xPosition = WINDOW_WIDTH;
        if (yPosition > WINDOW_HEIGHT) yPosition = 0;
        if (yPosition < 0) yPosition = WINDOW_HEIGHT;
    }

};


bool isCollide(Entity *a, Entity *b) {
    return (b->xPosition - a->xPosition) * (b->xPosition - a->xPosition) +
           (b->yPosition - a->yPosition) * (b->yPosition - a->yPosition) <
           (a->R + b->R) * (a->R + b->R);
}


int main() {
    srand(time(0));

    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hamster vs asteroids");
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7;
    t1.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/hamster.png");
    t2.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/background.jpg");
    t3.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/explosions/type_C.png");
    t4.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/rock.png");
    t5.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/fire_blue.png");
    t6.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/rock_small.png");
    t7.loadFromFile("/home/a-krupa/CLionProjects/little-shooter/images/explosions/type_B.png");

    t1.setSmooth(true);
    t2.setSmooth(true);
    t2.setRepeated(false);

    sf::Vector2f targetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    Sprite background(t2);
    background.setScale(
            targetSize.x / background.getLocalBounds().width,
            targetSize.y / background.getLocalBounds().height
    );

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


    std::list<Entity *> entities;

    for (int i = 0; i < 15; i++) {
        Asteroid *a = new Asteroid();
        a->settings(sRock, rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT, rand() % 360, 25);
        entities.push_back(a);
    }

    Hamster *p = new Hamster();
    p->settings(sPlayer, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0, 20);
    entities.push_back(p);

    /////main loop/////
    while (app.isOpen()) {
        Event event;
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space) {
                    Bullet *b = new Bullet();
                    b->settings(sBullet, p->xPosition, p->yPosition, p->angle, 10);
                    entities.push_back(b);
                }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) p->angle += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) p->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) p->thrust = true;
        else p->thrust = false;


        for (auto a:entities)
            for (auto b:entities) {
                if (a->name == "Asteroid" && b->name == "Bullet")
                    if (isCollide(a, b)) {
                        a->life = false;
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion, a->xPosition, a->yPosition);
                        e->name = "explosion";
                        entities.push_back(e);


                        for (int i = 0; i < 2; i++) {
                            if (a->R == 15) continue;
                            Entity *e = new Asteroid();
                            e->settings(sRock_small, a->xPosition, a->yPosition, rand() % 360, 15);
                            entities.push_back(e);
                        }

                    }

                if (a->name == "Hamster" && b->name == "Asteroid")
                    if (isCollide(a, b)) {
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion_ship, a->xPosition, a->yPosition);
                        e->name = "explosion";
                        entities.push_back(e);

                        p->settings(sPlayer, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0, 20);
                        p->dx = 0;
                        p->dy = 0;
                    }
            }


        if (p->thrust) p->anim = sPlayer_go;
        else p->anim = sPlayer;


        for (auto e:entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;

        if (rand() % 150 == 0) {
            Asteroid *a = new Asteroid();
            a->settings(sRock, 0, rand() % WINDOW_HEIGHT, rand() % 360, 25);
            entities.push_back(a);
        }

        for (auto i = entities.begin(); i != entities.end();) {
            Entity *e = *i;

            e->update();
            e->anim.update();

            if (e->life == false) {
                i = entities.erase(i);
                delete e;
            }
            else i++;
        }

        //////draw//////
        app.draw(background);
        for (auto i:entities) i->draw(app);
        app.display();
    }

    return 0;
}