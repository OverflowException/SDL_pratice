#pragma once

#include <list>
#include <mutex>
#include <condition_variable>

namespace chiptune {

template <typename T>
class StreamBuffer {
public:
    explicit StreamBuffer(size_t size = 1024) :
        _buf(size),
        _effective_length(0) {
        _head = _buf.begin();
        _tail = _buf.begin();
    };

    StreamBuffer& operator >> (T& ele) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv_not_empty.wait(lock, [&](){return _effective_length > 0;});
        
        ele = *_head++;
        if (_head == _buf.end()) {
            _head = _buf.begin();
        }
        --_effective_length;
        
        _cv_not_full.notify_all();
        return *this;
    }

    StreamBuffer& operator << (const T& ele) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv_not_full.wait(lock, [&](){return _effective_length < _buf.size();});

        *_tail++ = ele;
        if (_tail == _buf.end()) {
            _tail = _buf.begin();
        }
        ++_effective_length;
        
        _cv_not_empty.notify_all();
        return *this;
    }

private:
    std::list<T>                     _buf;
    typename std::list<T>::iterator  _head;
    typename std::list<T>::iterator  _tail;    
    size_t                           _effective_length;

    std::mutex                      _mutex;
    std::condition_variable         _cv_not_full;
    std::condition_variable         _cv_not_empty;
    
};

}
