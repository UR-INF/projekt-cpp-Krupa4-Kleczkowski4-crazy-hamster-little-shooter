#include <SFML/Graphics.hpp>
#include <ctime>
#include <list>
#include <cmath>

using namespace sf;

const int windowWidth = 1000;
const int windowHeight = 800;

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


class asteroid : public Entity {
public:
    asteroid() {
        dx = rand() % 8 - 4;
        dy = rand() % 8 - 4;
        name = "asteroid";
    }

    void update() {
        xPosition += dx;
        yPosition += dy;

        if (xPosition > windowWidth) xPosition = 0;
        if (xPosition < 0) xPosition = windowWidth;
        if (yPosition > windowHeight) yPosition = 0;
        if (yPosition < 0) yPosition = windowHeight;
    }

};


class bullet : public Entity {
public:
    bullet() {
        name = "bullet";
    }

    void update() {
        dx = cos(angle * DEGTORAD) * 6;
        dy = sin(angle * DEGTORAD) * 6;
        angle+=rand()%7-3;  /*try this*/
        xPosition += dx;
        yPosition += dy;

        if (xPosition > windowWidth || xPosition < 0 || yPosition > windowHeight || yPosition < 0) life = 0;
    }

};


class player : public Entity {
public:
    bool thrust;

    player() {
        name = "player";
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

        if (xPosition > windowWidth) xPosition = 0;
        if (xPosition < 0) xPosition = windowWidth;
        if (yPosition > windowHeight) yPosition = 0;
        if (yPosition < 0) yPosition = windowHeight;
    }

};


bool isCollide(Entity *a, Entity *b) {
    return (b->xPosition - a->xPosition) * (b->xPosition - a->xPosition) +
           (b->yPosition - a->yPosition) * (b->yPosition - a->yPosition) <
           (a->R + b->R) * (a->R + b->R);
}


int main() {
    srand(time(0));

    RenderWindow app(VideoMode(windowWidth, windowHeight), "Hamster vs asteroids");
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

    Sprite background(t2);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


    std::list<Entity *> entities;

    for (int i = 0; i < 15; i++) {
        asteroid *a = new asteroid();
        a->settings(sRock, rand() % windowWidth, rand() % windowHeight, rand() % 360, 25);
        entities.push_back(a);
    }

    player *p = new player();
    p->settings(sPlayer, 200, 200, 0, 20);
    entities.push_back(p);

    /////main loop/////
    while (app.isOpen()) {
        Event event;
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space) {
                    bullet *b = new bullet();
                    b->settings(sBullet, p->xPosition, p->yPosition, p->angle, 10);
                    entities.push_back(b);
                }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left)) p->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
        else p->thrust = false;


        for (auto a:entities)
            for (auto b:entities) {
                if (a->name == "asteroid" && b->name == "bullet")
                    if (isCollide(a, b)) {
                        a->life = false;
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion, a->xPosition, a->yPosition);
                        e->name = "explosion";
                        entities.push_back(e);


                        for (int i = 0; i < 2; i++) {
                            if (a->R == 15) continue;
                            Entity *e = new asteroid();
                            e->settings(sRock_small, a->xPosition, a->yPosition, rand() % 360, 15);
                            entities.push_back(e);
                        }

                    }

                if (a->name == "player" && b->name == "asteroid")
                    if (isCollide(a, b)) {
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion_ship, a->xPosition, a->yPosition);
                        e->name = "explosion";
                        entities.push_back(e);

                        p->settings(sPlayer, windowWidth / 2, windowHeight / 2, 0, 20);
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
            asteroid *a = new asteroid();
            a->settings(sRock, 0, rand() % windowHeight, rand() % 360, 25);
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