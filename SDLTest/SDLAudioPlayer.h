#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#define M_PI 3.14159265358979323846
class SDLAudioPlayer {
public:
    SDLAudioPlayer(int sampleRate = 44100, int channels = 1);
    ~SDLAudioPlayer();

    // ���� WAV �ļ������������ݣ�������
    bool LoadWAV(const char* filePath);
    // ��ʼ������Ƶ�����֮ǰδ���Ź������ͷ��ʼ���ţ�
    void Play();
    // ���²��ţ���ͷ��ʼ������Ƶ��
    void RePlay();
    // ��ͣ����
    void Pause();
    // ֹͣ���ţ������Ƶ������
    void Stop();
    // �������Ҳ����ݣ���ͬƵ����������д����Ƶ��
    void GenerateSineWave(float frequency, int durationMs);
	//���ɸ��ټ�88����ƵƵ��
    static float PianoKeyFrequency(int keyNumber);

private:
    SDL_AudioDeviceID device;
    SDL_AudioStream* stream;
    SDL_AudioSpec spec;   // �豸�����ʽ
    // ������ص� WAV ���ݺͶ�Ӧ����
    std::vector<Uint8> wavData;
    // ���� WAV �ļ�����Ƶ������ڸ�ʽת����
    SDL_AudioSpec wavSpec;

    // �ڲ��������������е� WAV ����������
    bool PushWavData();
};

