//#include "RoomManager.h"
//
//bool RoomManager::loadRoom(const std::string& path) {
//    currentRoom.unload();
//
//    if (!currentRoom.loadFromFile(path))
//        return false;
//
//    currentPath = path;
//    return true;
//}
//
//void RoomManager::changeRoom(const std::string& path) {
//    loadRoom(path);
//}
//
//Room* RoomManager::getCurrentRoom() {
//    return &currentRoom;
//}