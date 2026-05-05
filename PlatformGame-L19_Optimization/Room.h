#pragma once
#include <string>
#include <vector>

struct EnemyData {
    float x, y;
    std::string type;
};

struct DoorData {
    float x, y;
    std::string targetRoom;
    float spawnX, spawnY;
};

class Room {
public:
    bool loadFromFile(const std::string& path);
    void unload();

    const std::vector<EnemyData>& getEnemies() const;
    const std::vector<DoorData>& getDoors() const;

private:
    std::vector<EnemyData> enemies;
    std::vector<DoorData> doors;
};