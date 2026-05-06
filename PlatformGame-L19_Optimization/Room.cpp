//#include "Room.h"
//#include <fstream>
//#include <sstream>
//#include <iostream>
//
//bool Room::loadFromFile(const std::string& path) {
//    std::ifstream file(path);
//    if (!file.is_open()) return false;
//
//    std::string line;
//
//    while (std::getline(file, line)) {
//
//        // Door
//        if (line.find("type=\"door\"") != std::string::npos) {
//
//            DoorData d;
//
//            sscanf(line.c_str(), "%*[^x]x=\"%f\" %*[^y]y=\"%f\"", &d.x, &d.y);
//
//            d.targetRoom = "room_2.tmx";
//            d.spawnX = 100;
//            d.spawnY = 100;
//
//            doors.push_back(d);
//        }
//
//        // Enemy
//        if (line.find("type=\"enemy\"") != std::string::npos) {
//            EnemyData e;
//
//            sscanf(line.c_str(), "%*[^x]x=\"%f\" %*[^y]y=\"%f\"", &e.x, &e.y);
//
//            e.type = "slime";
//            enemies.push_back(e);
//        }
//    }
//
//    return true;
//}
//
//void Room::unload() {
//    enemies.clear();
//    doors.clear();
//}
//
//const std::vector<EnemyData>& Room::getEnemies() const {
//    return enemies;
//}
//
//const std::vector<DoorData>& Room::getDoors() const {
//    return doors;
//}