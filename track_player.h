#include <condition_variable>
#include <iostream>
#include <map>
#include <memory>
#includex <mutex>


#include "stream_buffer.h"

namespace chiptune {

struct PlayerOptions {
    // Not necessarily maximum track.
    // More tracks playing may result in clipping
    size_t max_tracks;
};

class Player {
public:
    
    explicit Player(const PlayerOptions& opt) : _opt(opt) {};

    ~Player();
    
    // Return false if track name exists
    bool insert(const std::string& name);

    bool eject(const std::string& name);

    bool exist(const std::string& name);

    void pause(const std::string& name);

    void resume(const std::string& name);

private:

    struct StreamBufferNode {
        explicit StreamBufferNode(size_t size) {
            data = ::operator new(size * sizeof(uint8_t));
        };

        ~StreamBufferNode() {
            data = ::operator delete(data);
            data = nullptr;
        }
        
        uint8_t*  data;
        size_t    size;
    };

    void audio_callback(void* userdata, uint8_t* stream, int len);

    void mixer_task();
    void player_task();

    std::thread _mixer_thread;
    
    PlayerOptions            _opt;
    bool                     _running;
    
    std::mutex               _track_mtx;
    std::condition_variable  _cv_track_not_empty;

    // track name : track
    std::map<std::string, std::shared_ptr<Track>>   _track_map;    
    StreamBuffer<std::shared_ptr<StreamBufferNode>> _stream_buf;
};

}
