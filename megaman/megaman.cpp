#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>

// Game Constants
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -15.0f;
const float PLAYER_SPEED = 5.0f;
const float PLAYER_WIDTH = 30.0f;
const float PLAYER_HEIGHT = 40.0f;
const float ENEMY_WIDTH = 30.0f;
const float ENEMY_HEIGHT = 30.0f;
const float BULLET_SPEED = 8.0f;
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;

// Bullet class
class Bullet {
public:
    sf::CircleShape shape;
    float velocityX;
    
    Bullet(float x, float y, float vx) : velocityX(vx) {
        shape.setRadius(5.0f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(x, y);
    }
    
    void update() {
        shape.move(velocityX, 0);
    }
    
    bool isOutOfBounds() {
        return shape.getPosition().x < -50 || shape.getPosition().x > WINDOW_WIDTH + 50;
    }
};

// Enemy class
class Enemy {
public:
    sf::RectangleShape shape;
    float velocityX;
    float velocityY;
    int health;
    
    Enemy(float x, float y) : velocityX(2.0f), velocityY(0), health(1) {
        shape.setSize({ENEMY_WIDTH, ENEMY_HEIGHT});
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y);
    }
    
    void update() {
        velocityY += GRAVITY;
        shape.move(velocityX, velocityY);
        
        // Bounce off walls
        if (shape.getPosition().x < 0 || shape.getPosition().x + ENEMY_WIDTH > WINDOW_WIDTH) {
            velocityX = -velocityX;
        }
        
        // Ground collision
        if (shape.getPosition().y + ENEMY_HEIGHT >= WINDOW_HEIGHT - 20) {
            shape.setPosition(shape.getPosition().x, WINDOW_HEIGHT - 20 - ENEMY_HEIGHT);
            velocityY = 0;
        }
    }
    
    bool isHit(const sf::CircleShape& bullet) {
        return shape.getGlobalBounds().intersects(bullet.getGlobalBounds());
    }
    
    bool isAlive() {
        return health > 0;
    }
};

// Player class
class Player {
public:
    sf::RectangleShape shape;
    float velocityY;
    bool isJumping;
    int health;
    int ammo;
    int score;
    
    Player(float x, float y) : velocityY(0), isJumping(false), health(3), ammo(100), score(0) {
        shape.setSize({PLAYER_WIDTH, PLAYER_HEIGHT});
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(x, y);
    }
    
    void handleInput(sf::Keyboard::Key key, bool pressed) {
        // Movement is handled in update with real-time key checking
    }
    
    void update(const sf::RenderWindow& window) {
        velocityY += GRAVITY;
        
        // Real-time keyboard input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            shape.move(-PLAYER_SPEED, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            shape.move(PLAYER_SPEED, 0);
        }
        
        // Boundary collision
        if (shape.getPosition().x < 0) {
            shape.setPosition(0, shape.getPosition().y);
        }
        if (shape.getPosition().x + PLAYER_WIDTH > WINDOW_WIDTH) {
            shape.setPosition(WINDOW_WIDTH - PLAYER_WIDTH, shape.getPosition().y);
        }
        
        // Vertical movement
        shape.move(0, velocityY);
        
        // Ground collision
        if (shape.getPosition().y + PLAYER_HEIGHT >= WINDOW_HEIGHT - 20) {
            shape.setPosition(shape.getPosition().x, WINDOW_HEIGHT - 20 - PLAYER_HEIGHT);
            velocityY = 0;
            isJumping = false;
        }
    }
    
    void jump() {
        if (!isJumping) {
            velocityY = JUMP_FORCE;
            isJumping = true;
        }
    }
    
    void shoot(std::vector<Bullet>& bullets) {
        if (ammo > 0) {
            bullets.push_back(Bullet(shape.getPosition().x + PLAYER_WIDTH, 
                                   shape.getPosition().y + PLAYER_HEIGHT / 2, 
                                   BULLET_SPEED));
            ammo--;
        }
    }
    
    bool checkEnemyCollision(const sf::RectangleShape& enemy) {
        return shape.getGlobalBounds().intersects(enemy.getGlobalBounds());
    }
    
    void takeDamage() {
        health--;
    }
    
    void addScore(int points) {
        score += points;
    }
};

// Game class
class Game {
private:
    sf::RenderWindow window;
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    sf::Font font;
    int wave;
    int enemiesDefeated;
    
public:
    Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Megaman Clone"),
             player(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100), wave(1), enemiesDefeated(0) {
        window.setFramerateLimit(60);
        spawnEnemies();
    }
    
    void spawnEnemies() {
        int enemyCount = 2 + wave;
        for (int i = 0; i < enemyCount; i++) {
            float x = 100 + (i * 200);
            enemies.push_back(Enemy(x, 100));
        }
    }
    
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    player.jump();
                }
                if (event.key.code == sf::Keyboard::X) {
                    player.shoot(bullets);
                }
            }
        }
    }
    
    void update() {
        if (player.health <= 0) {
            std::cout << "Game Over! Final Score: " << player.score << std::endl;
            window.close();
            return;
        }
        
        player.update(window);
        
        // Update bullets
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->update();
            if (it->isOutOfBounds()) {
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }
        
        // Update enemies
        for (auto& enemy : enemies) {
            enemy.update();
            
            // Check collision with player
            if (player.checkEnemyCollision(enemy.shape)) {
                player.takeDamage();
                enemy.health = 0;
            }
        }
        
        // Check bullet-enemy collisions
        for (auto it = bullets.begin(); it != bullets.end();) {
            bool hit = false;
            for (auto& enemy : enemies) {
                if (enemy.isHit(*it)) {
                    enemy.health--;
                    player.addScore(100);
                    hit = true;
                    break;
                }
            }
            if (hit) {
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }
        
        // Remove dead enemies
        for (auto it = enemies.begin(); it != enemies.end();) {
            if (!it->isAlive()) {
                it = enemies.erase(it);
                enemiesDefeated++;
            } else {
                ++it;
            }
        }
        
        // Check if wave is complete
        if (enemies.empty() && player.health > 0) {
            wave++;
            enemiesDefeated = 0;
            player.ammo = 100;
            spawnEnemies();
        }
    }
    
    void render() {
        window.clear(sf::Color::Black);
        
        // Draw ground
        sf::RectangleShape ground(sf::Vector2f(WINDOW_WIDTH, 20));
        ground.setPosition(0, WINDOW_HEIGHT - 20);
        ground.setFillColor(sf::Color::Green);
        window.draw(ground);
        
        // Draw game objects
        window.draw(player.shape);
        
        for (auto& enemy : enemies) {
            window.draw(enemy.shape);
        }
        
        for (auto& bullet : bullets) {
            window.draw(bullet.shape);
        }
        
        // Draw UI (simple text-based for now)
        sf::RectangleShape uiBackground(sf::Vector2f(WINDOW_WIDTH, 40));
        uiBackground.setFillColor(sf::Color(50, 50, 50));
        window.draw(uiBackground);
        
        window.display();
    }
    
    void printStats() {
        std::cout << "\n=== Wave " << wave << " ===" << std::endl;
        std::cout << "Health: " << player.health << std::endl;
        std::cout << "Ammo: " << player.ammo << std::endl;
        std::cout << "Score: " << player.score << std::endl;
        std::cout << "Enemies Defeated: " << enemiesDefeated << std::endl;
    }
    
    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
};

int main() {
    std::cout << "=== Megaman Clone ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "A - Move Left" << std::endl;
    std::cout << "D - Move Right" << std::endl;
    std::cout << "SPACE - Jump" << std::endl;
    std::cout << "X - Shoot" << std::endl;
    std::cout << "====================" << std::endl;
    
    Game game;
    game.run();
    
    return 0;
}
