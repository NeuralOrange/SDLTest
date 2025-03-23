#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <map>
#define M_PI 3.14159265358979323846

struct AudioItem {
    SDL_AudioStream* stream;
    std::vector<Uint8> wavData; // 加载的 WAV 数据（对于外部音源）
    SDL_AudioSpec wavSpec;      // WAV 数据的格式
};

class SDLAudioPlayer {
public:
    SDLAudioPlayer(int sampleRate = 44100, int channels = 1);
    ~SDLAudioPlayer();

    // 加载 WAV 文件并以 name 进行标识（仅加载，不播放）
    bool LoadWAV(const std::string& name, const char* filePath);
    // 播放指定名称的音频（如果未加载数据则不播放）
    void Play(const std::string& name);
    // 重头播放指定名称的音频
    void RePlay(const std::string& name);
    // 停止指定名称的音频（清空对应音频流）
    void Stop(const std::string& name);
    // 生成正弦波，并以 name 标识存储与播放
    void GenerateSineWave(const std::string& name, float frequency, int durationMs);
	// 生成和弦音频
	void GenerateChord(const std::string& name,const std::vector<float>& frequencies, int durationMs);
private:
    SDL_AudioDeviceID device;
    SDL_AudioSpec spec;  // 设备的输出规格（统一）
    std::map<std::string, AudioItem> audioMap; // 管理不同名称的音频项

    // 内部函数：将音频项的 WAV 数据推入其音频流
    bool PushWavData(AudioItem& item);
};

class SpecialAudio
{
public:
    static float PianoKeyFrequency(int keyNumber)
    {
        if (keyNumber < 1 || keyNumber > 88) {
            std::cerr << "键号必须在1到88之间！" << std::endl;
            return 0.0f;
        }
        return 440.0f * std::pow(2.0f, (keyNumber - 49) / 12.0f);
    }

    // C大调和弦（C4, E4, G4）
    static std::vector<float> C_Major;

    // D大调和弦（D4, F#4, A4）
    static std::vector<float> D_Major;

    // E大调和弦（E4, G#4, B4）
    static std::vector<float> E_Major;

    // F大调和弦（F4, A4, C5）
    static std::vector<float> F_Major;

    // G大调和弦（G4, B4, D5）
    static std::vector<float> G_Major;

    // A大调和弦（A4, C#5, E5）
    static std::vector<float> A_Major;

    // B大调和弦（B4, D#5, F#5）
    static std::vector<float> B_Major;

    // A小调和弦（A4, C5, E5）
    static std::vector<float> A_Minor;

    // D小调和弦（D4, F4, A4）
    static std::vector<float> D_Minor;

    // E小调和弦（E4, G4, B4）
    static std::vector<float> E_Minor;

    // C7（C Dominant Seventh: C4, E4, G4, Bb4）
    static std::vector<float> C_Dom7;

    // G7（G Dominant Seventh: G4, B4, D5, F5）
    static std::vector<float> G_Dom7;
};

