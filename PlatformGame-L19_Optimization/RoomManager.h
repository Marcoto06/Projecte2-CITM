#pragma once
#include "Room.h"

class RoomManager {
public:
    bool loadRoom(const std::string& path);
    void changeRoom(const std::string& path);

    Room* getCurrentRoom();

private:
    Room currentRoom;
    std::string currentPath;
};