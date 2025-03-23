#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <map>
#define M_PI 3.14159265358979323846

struct AudioItem {
    SDL_AudioStream* stream;
    std::vector<Uint8> wavData; // ���ص� WAV ���ݣ������ⲿ��Դ��
    SDL_AudioSpec wavSpec;      // WAV ���ݵĸ�ʽ
};

class SDLAudioPlayer {
public:
    SDLAudioPlayer(int sampleRate = 44100, int channels = 1);
    ~SDLAudioPlayer();

    // ���� WAV �ļ����� name ���б�ʶ�������أ������ţ�
    bool LoadWAV(const std::string& name, const char* filePath);
    // ����ָ�����Ƶ���Ƶ�����δ���������򲻲��ţ�
    void Play(const std::string& name);
    // ��ͷ����ָ�����Ƶ���Ƶ
    void RePlay(const std::string& name);
    // ָֹͣ�����Ƶ���Ƶ����ն�Ӧ��Ƶ����
    void Stop(const std::string& name);
    // �������Ҳ������� name ��ʶ�洢�벥��
    void GenerateSineWave(const std::string& name, float frequency, int durationMs);
	// ���ɺ�����Ƶ
	void GenerateChord(const std::string& name,const std::vector<float>& frequencies, int durationMs);
private:
    SDL_AudioDeviceID device;
    SDL_AudioSpec spec;  // �豸��������ͳһ��
    std::map<std::string, AudioItem> audioMap; // ����ͬ���Ƶ���Ƶ��

    // �ڲ�����������Ƶ��� WAV ������������Ƶ��
    bool PushWavData(AudioItem& item);
};

class SpecialAudio
{
public:
    static float PianoKeyFrequency(int keyNumber)
    {
        if (keyNumber < 1 || keyNumber > 88) {
            std::cerr << "���ű�����1��88֮�䣡" << std::endl;
            return 0.0f;
        }
        return 440.0f * std::pow(2.0f, (keyNumber - 49) / 12.0f);
    }

    // C������ң�C4, E4, G4��
    static std::vector<float> C_Major;

    // D������ң�D4, F#4, A4��
    static std::vector<float> D_Major;

    // E������ң�E4, G#4, B4��
    static std::vector<float> E_Major;

    // F������ң�F4, A4, C5��
    static std::vector<float> F_Major;

    // G������ң�G4, B4, D5��
    static std::vector<float> G_Major;

    // A������ң�A4, C#5, E5��
    static std::vector<float> A_Major;

    // B������ң�B4, D#5, F#5��
    static std::vector<float> B_Major;

    // AС�����ң�A4, C5, E5��
    static std::vector<float> A_Minor;

    // DС�����ң�D4, F4, A4��
    static std::vector<float> D_Minor;

    // EС�����ң�E4, G4, B4��
    static std::vector<float> E_Minor;

    // C7��C Dominant Seventh: C4, E4, G4, Bb4��
    static std::vector<float> C_Dom7;

    // G7��G Dominant Seventh: G4, B4, D5, F5��
    static std::vector<float> G_Dom7;
};

