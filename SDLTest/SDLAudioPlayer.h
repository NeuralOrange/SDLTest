#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#define M_PI 3.14159265358979323846
class SDLAudioPlayer {
public:
    SDLAudioPlayer(int sampleRate = 44100, int channels = 1);
    ~SDLAudioPlayer();

    // 加载 WAV 文件，仅保存数据，不播放
    bool LoadWAV(const char* filePath);
    // 开始播放音频（如果之前未播放过，则从头开始播放）
    void Play();
    // 重新播放（重头开始播放音频）
    void RePlay();
    // 暂停播放
    void Pause();
    // 停止播放，清空音频流数据
    void Stop();
    // 生成正弦波数据（不同频率声音）并写入音频流
    void GenerateSineWave(float frequency, int durationMs);
	//生成钢琴键88键音频频率
    static float PianoKeyFrequency(int keyNumber);

private:
    SDL_AudioDeviceID device;
    SDL_AudioStream* stream;
    SDL_AudioSpec spec;   // 设备输出格式
    // 保存加载的 WAV 数据和对应长度
    std::vector<Uint8> wavData;
    // 保存 WAV 文件的音频规格（用于格式转换）
    SDL_AudioSpec wavSpec;

    // 内部方法：将缓存中的 WAV 数据推入流
    bool PushWavData();
};

