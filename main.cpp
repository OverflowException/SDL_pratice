#include <iostream>
#include <SDL2/SDL.h>

void show_spec(const SDL_AudioSpec& wav_spec);
void audio_callback(void* userdata, uint8_t* stream, int stream_length);

#define RESAMPLE_RATE 2

struct AudioData {
    uint8_t* pos;
    uint32_t length;
    // For resampling
    uint32_t copy_size;
    uint32_t skip_size;
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <audio filename>" << std::endl;
        return 0;
    }
    const char* filename = argv[1];


    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wav_spec;
    uint8_t* wav_start;
    uint32_t wav_length;

    if (SDL_LoadWAV(filename, &wav_spec, &wav_start, &wav_length) == NULL) {
        std::cout << "Cannot load " << filename << std::endl;
        return 0;
    }
    show_spec(wav_spec);

    // Setup a pointer context to send into audio callback as parameter
    // So that callback knows where audio data comes from, so it could copy to stream
    AudioData audio;
    audio.pos = wav_start;
    audio.length = wav_length;
    audio.copy_size = SDL_AUDIO_BITSIZE(wav_spec.format)  * wav_spec.channels / 8;
    audio.skip_size = audio.copy_size * (RESAMPLE_RATE - 1);
    std::cout << "copy size = " << audio.copy_size << ", skip size = " << audio.skip_size << std::endl;
    
    wav_spec.samples /= RESAMPLE_RATE;
    wav_spec.callback = audio_callback;
    wav_spec.userdata = &audio;
    
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL,
                                                   SDL_AUDIO_ALLOW_ANY_CHANGE);
    show_spec(wav_spec);

    if (device == 0) {
        std::cout << "Cannot open device" << std::endl;
        SDL_FreeWAV(wav_start);
        SDL_Quit();
        return 0;
    }

    show_spec(wav_spec);
    
    SDL_PauseAudioDevice(device, 0);

    while(audio.length > 0) {
        SDL_Delay(1000);
    }

    SDL_CloseAudioDevice(device);
    SDL_FreeWAV(wav_start);
    SDL_Quit();
    
    return 0;
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

void audio_callback(void* userdata, uint8_t* stream, int stream_length) {
    AudioData* ctx = (AudioData*)userdata;
    std::cout << "Bytes remaining: " << ctx->length << std::endl;

    if (ctx->length == 0) {
        // No more data to play
        return;
    }

    uint32_t length = (uint32_t)stream_length;
    length = (length > ctx->length ? ctx->length : length);

    while (true) {
        for (uint32_t copy_idx = 0; copy_idx < ctx->copy_size; ++copy_idx, --stream_length) {
            if (stream_length == 0) {
                return;
            }
            *stream++ = *ctx->pos++;
            std::cout << "copy" << std::endl;
        }
        for (uint32_t skip_idx = 0; skip_idx < ctx->copy_size; ++skip_idx, --stream_length) {
            if (stream_length == 0) {
                return;
            }
            *ctx->pos += 1;
            std::cout << "skip" << std::endl;
        }
    };

    // SDL_memcpy(stream, ctx->pos, length);
    
    // ctx->pos += length;
    // ctx->length -= length;

}
