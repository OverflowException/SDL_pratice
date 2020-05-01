#include "track_player.h"

namespace chiptune {

Player::Player(const PlayerOptions& opt) : _opt(opt){
    //////
    // TODO
    ///////

    // TODO: bind mixer

    _mixer_thread = std::thread(mixer_task);
    _running = true;
}

~Player::Player() {
    /////
    // TODO
    /////

    _running = false;
    _mixer_thread.join();
}

bool Player::insert(const std::string& name, std::shared_ptr<Track> tr) {
    std::lock_guard<std::mutex> lock(_track_mtx);
    auto insert_result = _track_map.insert(
        std::pair<std::string, std::shared_ptr<Track>>);

    if (insert_result.second) {
        // Insertion success
        _cv_track_not_empty.notify_all();
    }

    return insert_result.second;
}

bool Player::exist(const std::string& name) {
    std::lock_guard<std::mutex> lock(_track_mtx);    
    return _track_map.find(name) != _track_map.end();
}

bool Player::eject(const std::string& name) {
    std::lock_guard<std::mutex> lock(_track_mtx);
    auto iter = _track_map.find(name);

    if (iter == _track_map.end()) {
        return false;
    } else {
        _track_map.erase(iter);
        return true;
    }
}

void Player::mixer_task() {
    ///////
    // TODO
    ///////

    while (_running) {
        std::unique_lock<std::mutex> lock(_track_mtx);
        _cv_track_not_empty.wait(lock, [&](){return _track_map.size() > 0;});

        // implementation
        
    }
}

}
