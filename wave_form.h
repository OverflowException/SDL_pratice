#pragma once

namespace chiptune {

template <typename T>
class WaveForm {
public:
    WaveForm() {};

    virtual ~WaveForm() {};

    virtual T next_sample();

public:
    double _freq;
};

template <typename T>
class SquareWave : public WaveForm<T> {
public:
    virtual T next_sample () override {
        return _value;
    }

private:
    T _value;
    
};

}

