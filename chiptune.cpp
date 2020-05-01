#include <iostream>
#include <SDL2/SDL.h>

#include "stream_buffer.h"

const int8_t amplitude = 127;
const int sample_rate = 44100;
const int samples_per_buffer = 1024;
const float freq = 441.0;

// This is implemented as circular buffer,
// size of 2 makes it ping-pong buffer
chiptune::StreamBuffer<std::shared_ptr<std::string>> buf(2);

void show_spec(const SDL_AudioSpec& wav_spec);
void gen_buf();
void audio_callback(void* userdata, uint8_t* stream, int len);

int main(int argc, char** argv) {

    SDL_Init(SDL_INIT_AUDIO);

    int sample_count;
    
    SDL_AudioSpec want_spec;
    want_spec.freq = 44100;
    want_spec.format = AUDIO_S8;
    want_spec.channels = 1;
    want_spec.samples = samples_per_buffer;
    want_spec.callback = audio_callback;
    want_spec.userdata = NULL;

    SDL_AudioSpec get_spec;    
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &want_spec, &get_spec,
                                                   SDL_AUDIO_ALLOW_ANY_CHANGE);


    if (device == 0) {
        std::cout << "Cannot open device" << std::endl;
        SDL_Quit();
        return 0;
    }

    if (want_spec.format != get_spec.format) {
        std::cout << "Cannot get desired specs" << std::endl;

        SDL_CloseAudioDevice(device);
        SDL_Quit();
        return 0;
    }

    std::cout << "Want spec" << std::endl;
    show_spec(want_spec);
    std::cout << "Get spec" << std::endl;
    show_spec(get_spec);

    SDL_PauseAudioDevice(device, 0);

    gen_buf();

    SDL_CloseAudioDevice(device);
    SDL_Quit();
    
    return 0;
}

void gen_buf() {
    int sample_count = 0;
    double t;
    
    while (true) {
        std::string data;
        data.resize(samples_per_buffer);
        for (auto& ele : data) {
            t = (double)sample_count++ / (double)sample_rate;
            ele = (int8_t)amplitude * sin(2 * M_PI * freq * t);
            int num = ele;
            std::cout << num << " ";
        }
        
        auto ptr = std::make_shared<std::string>(data);
        buf << ptr;
    }
}

void audio_callback(void* userdata, uint8_t* stream, int len) {

    std::shared_ptr<std::string> ptr;
    buf >> ptr;
    SDL_memcpy(stream, ptr->c_str(), len);
}

void show_spec(const SDL_AudioSpec& wav_spec) {
    std::cout << "============ AudioSpec ==========" << std::endl;

    std::cout << "freq = " << wav_spec.freq << std::endl;
    std::cout << "format = " << std::hex << wav_spec.format << std::dec << std::endl;
    std::cout << "channels = " << int(wav_spec.channels) << std::endl;
    std::cout << "silence = " << int(wav_spec.silence) << std::endl;
    std::cout << "samples = " << wav_spec.samples << std::endl;
    std::cout << "size = " << wav_spec.size << std::endl;

    std::cout << "=================================" << std::endl;
}
