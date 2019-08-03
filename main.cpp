#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Audio.hpp>

class gold {
private:
    int quantity;
    const std::vector<int> location;
    bool collected = false;

public:
    gold (int Q, std::vector<int> L) : quantity(Q), location(L) {}

    std::vector<int> getPosition() {
        return location;
    }

    int getValue() {
        return quantity;
    }

    void collect() {
        collected = true;
        quantity = 0;
    }

    bool isCollected() {
        return collected;
    }

};

class player {
private:
    const int maxHP = 100;
    int HP = maxHP;
    bool isDead = false;
    int goldBalance = 0;

public:
    void heal(int health) {
        HP += health;
        if (HP > maxHP) {
            HP = maxHP;
        }
    }

    void damage(int damage) {
        for (HP; (HP<maxHP) && (goldBalance > 0); heal(5) ) goldBalance -= 1;

        HP -= damage;
        if (HP <= 0) {
            isDead = true;
        }
    }

    bool isAlive() {
        return !isDead;
    }

    int getHealth() {
        return HP;
    }

    int getGold() {
        return goldBalance;
    }

    void giveGold(int value) {
        if (HP < maxHP) {
            heal(value*5);
        } else {
            goldBalance += value;
        }
    }

};

class enemy {
private:
    const int maxHP;
    int HP = maxHP;
    bool isDead = false;
    int goldBalance;
    const int attackValue;

    std::vector<int> location;
    const int locationUpdateTimerDefault = 60; //30 FPS
    int locationUpdateTimer = locationUpdateTimerDefault;

public:
    enemy(int health, int attack, std::vector<int> L, int initialGoldBalance=0) : maxHP(health), attackValue(attack), location(L), goldBalance(initialGoldBalance) {}

    void heal(int health) {
        HP += health;
        if (HP > maxHP) {
            HP = maxHP;
        }
    }

    void damage(int damage) {
        HP -= damage;
        if (HP <= 0) {
            isDead = true;
        }
    }

    bool isAlive() {
        return !isDead;
    }

    int getHealth() {
        return HP;
    }

    int getGold() {
        return goldBalance;
    }

    void giveGold(int value) {
        goldBalance += value;
    }

    int getAttackValue() {
        return attackValue;
    }

    std::vector<int> getPosition() {
        return location;
    }

    void updatePosition(int position[2]) {
        if (locationUpdateTimer <= 0) {
            locationUpdateTimer = locationUpdateTimerDefault;

            if (position[0] > location[0]) location[0] += 1;
            else if (position[0] < location[0]) location[0] -=  1;
            if (position[1] > location[1]) location[1] += 1;
            else if (position[1] < location[1]) location[1] -= 1;

        } else {
            locationUpdateTimer -= 1;
        }
    }

};

class dungeon {
private:
    sf::RenderWindow* windowReference;
    player* playerReference;

    int dungeonNumber;

    const float dungeonSize[2] = {520, 260};
    const int columnSize = 20;
    const int dungeonScreenOffset = 40;
    const int goldSize = 5;

    sf::Color backgroundColor = sf::Color(100,100,100);
    sf::Color playerColor = sf::Color(30, 50, 255);
    sf::Color goldColor = sf::Color(255, 220, 50);
    sf::Color enemyColor = sf::Color(195, 5, 5);

    int playerPosition[2] = {0, 0}; //in columns

    std::vector<gold> goldList;
    const int maxGoldQuantity = 14; //14 is a good number
    const int minGoldQuantity = 1;

    std::vector<enemy> enemyList;
    const int maxEnemyQuantity = 5;
    const int minEnemyQuantity = 1;

    sf::SoundBuffer buffer;
    sf::Sound sound;

public:
    dungeon(sf::RenderWindow& W, player& P) {
        srand(time(0));

        windowReference = &W;
        playerReference = &P;
        dungeonNumber = 1;

        buffer.loadFromFile("../soundeffect.wav");
        sound.setBuffer(buffer);

        //initialize the dungeon with random amounts of gold
        fillGoldList();

        //initialize the dungeon with random enemies
        fillEnemyList();
    }

    void fillEnemyList() {
        enemyList.clear();
        //random enemy generator!
        int enemyQuantity = rand()%(maxEnemyQuantity+dungeonNumber-1)+(minEnemyQuantity+dungeonNumber-1);
        for (int i = 0; i < enemyQuantity; i++) {
            enemyList.push_back(enemy(20, 5, std::vector<int> {rand()%26, rand()%13}));
        }
    }

    void fillGoldList() {
        goldList.clear();
        //random gold generator!
        int goldQuantity = rand()%(maxGoldQuantity)+minGoldQuantity;
        for (int i = 0; i < goldQuantity; i++) {
            goldList.push_back(gold(1, std::vector<int> {rand()%26, rand()%13}));
        }
    }

    bool isRectCollided(sf::RectangleShape rect1, sf::RectangleShape rect2) {

        sf::FloatRect boundingBox = rect1.getGlobalBounds();
        sf::FloatRect otherBox = rect2.getGlobalBounds();

        if (boundingBox.intersects(otherBox))
        {
            return true;
        }

        return false;
    }

    void render() {
        //draw background color
        sf::RectangleShape background(sf::Vector2f(dungeonSize[0], dungeonSize[1]));
        background.setPosition(dungeonScreenOffset, dungeonScreenOffset);
        background.setFillColor(backgroundColor);
        windowReference->draw(background);

        //draw player
        sf::RectangleShape player(sf::Vector2f((float) columnSize, (float) columnSize));
        player.setPosition(dungeonScreenOffset+playerPosition[0]*columnSize, dungeonScreenOffset+playerPosition[1]*columnSize);
        player.setFillColor(playerColor);
        windowReference->draw(player);

        //draw enemies
        sf::RectangleShape enemyRect(sf::Vector2f((float) columnSize, (float) columnSize));
        enemyRect.setFillColor(enemyColor);
        for (class enemy& E : enemyList) {
            enemyRect.setPosition(dungeonScreenOffset+E.getPosition()[0]*columnSize, dungeonScreenOffset+E.getPosition()[1]*columnSize);
            if (isRectCollided(player, enemyRect)) {
                playerReference->damage(E.getAttackValue());
            }
            E.updatePosition(playerPosition);

            windowReference->draw(enemyRect);
        }

        //draw gold + handle collision detection
        sf::RectangleShape goldRect(sf::Vector2f((float) goldSize,(float) goldSize));
        goldRect.setFillColor(goldColor);
        for (class gold& G : goldList) {
            goldRect.setPosition(dungeonScreenOffset+G.getPosition()[0]*columnSize+(columnSize-goldSize)/2, dungeonScreenOffset+G.getPosition()[1]*columnSize+(columnSize-goldSize)/2);
            if (isRectCollided(player, goldRect)) {
                playerReference->giveGold(G.getValue());
                if (!G.isCollected()) sound.play();
                G.collect();
            }

            if (!G.isCollected())
                windowReference->draw(goldRect);
        }

        //if all gold has been collected go to next dungeon
        int numGoldCollected = 0;
        for (class gold G : goldList) {
            if (G.isCollected()) numGoldCollected += 1;
        }
        if (numGoldCollected == goldList.size()) evolveDungeon();
    };

    void movePlayer(int dir) {
        switch (dir) {
            case 1: //up
                if (playerPosition[1] != 0) playerPosition[1] -= 1;
                break;
            case 2: //down
                if (playerPosition[1] != dungeonSize[1]/columnSize-1) playerPosition[1] += 1;
                break;
            case 3: //left
                if (playerPosition[0] != 0) playerPosition[0] -= 1;
                break;
            case 4: //right
                if (playerPosition[0] != dungeonSize[0]/columnSize-1) playerPosition[0] += 1;
                break;
            default:
                //do nothing
                break;
        }
    }

    int getNumber() {
        return dungeonNumber;
    }

    void evolveDungeon() {
        playerPosition[0] = 0;
        playerPosition[1] = 0;

        dungeonNumber += 1;

        fillGoldList();
        fillEnemyList();
    }

};

class hud {
private:
    sf::RenderWindow* windowReference;
    player* playerReference;
    dungeon* dungeonReference;

    sf::Font font;

public:
    hud(sf::RenderWindow& W, player& P, dungeon& D) {
        windowReference = &W;
        playerReference = &P;
        dungeonReference = &D;

        font.loadFromFile("../font.ttf");
    }

    void render() {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);

        text.setPosition(40, 0);
        std::string dungeonName = "dungeon: " + std::to_string(dungeonReference->getNumber());
        text.setString(dungeonName);
        windowReference->draw(text);

        text.setPosition(40, 320);
        std::string playerHealth = "HP: " + std::to_string(playerReference->getHealth());
        text.setString(playerHealth);
        windowReference->draw(text);

        text.setPosition(200, 320);
        std::string playerGold = "Gold: " + std::to_string(playerReference->getGold());
        text.setString(playerGold);
        windowReference->draw(text);
    }

};

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 400), "Dungeon Crawler");
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(30);

    sf::Music music;
    music.openFromFile("../music.wav");
    music.setLoop(true);
    music.play();

    player Dude;

    dungeon Dungeon = dungeon(window, Dude);

    hud HUD(window, Dude, Dungeon);

    while (window.isOpen() && Dude.isAlive())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W) Dungeon.movePlayer(1);
                if (event.key.code == sf::Keyboard::S) Dungeon.movePlayer(2);
                if (event.key.code == sf::Keyboard::A) Dungeon.movePlayer(3);
                if (event.key.code == sf::Keyboard::D) Dungeon.movePlayer(4);
            }
        }

        window.clear();
        Dungeon.render();
        HUD.render();
        window.display();
    }

    return 0;
}