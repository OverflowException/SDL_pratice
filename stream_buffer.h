#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace chiptune {

template <typename T>
class StreamBuffer {
public:
    explicit StreamBuffer(size_t size = 1024) : _max_buf_size(size) {};

    StreamBuffer& operator >> (T& ele) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv_not_empty.wait(lock, [&](){return _buf.size() > 0;});
        
        ele = _buf.front();
        _buf.pop();
        
        _cv_not_full.notify_all();
        return *this;
    }

    StreamBuffer& operator << (const T& ele) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv_not_full.wait(lock, [&](){return _buf.size() < _max_buf_size;});

        _buf.push(ele);

        _cv_not_empty.notify_all();
        return *this;
    }

private:
    std::queue<T>                    _buf;
    size_t                           _max_buf_size;

    std::mutex                      _mutex;
    std::condition_variable         _cv_not_full;
    std::condition_variable         _cv_not_empty;
    
};

}
