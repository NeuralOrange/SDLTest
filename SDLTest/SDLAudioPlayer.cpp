#include "SDLAudioPlayer.h"

// ���캯������ʼ����Ƶϵͳ�����豸������������Ƶ��
SDLAudioPlayer::SDLAudioPlayer(int sampleRate, int channels) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL ��Ƶ��ʼ��ʧ��: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // �����豸������
    spec.freq = sampleRate;
    spec.format = SDL_AUDIO_F32;  // 32λ��������ʽ
    spec.channels = channels;

    // ��Ĭ�ϲ����豸
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (device == 0) {
        std::cerr << "�޷�����Ƶ�豸: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // ������Ƶ������ʼʱ�����ʽͬ�����ʽ
    stream = SDL_CreateAudioStream(&spec, &spec);
    if (!stream) {
        std::cerr << "�޷�������Ƶ��: " << SDL_GetError() << std::endl;
        exit(1);
    }
    SDL_BindAudioStream(device, stream);

    // ���豸������ͣ״̬����ֹ�Զ�����
    SDL_PauseAudioDevice(device);
}

// �����������ͷ���Դ
SDLAudioPlayer::~SDLAudioPlayer() {
    SDL_DestroyAudioStream(stream);
    SDL_CloseAudioDevice(device);
    SDL_Quit();
}

// �ڲ��������������е� WAV ����������Ƶ��
bool SDLAudioPlayer::PushWavData() {
    if (wavData.empty()) {
        std::cerr << "δ���� WAV ���ݣ�" << std::endl;
        return false;
    }
    // ������Ƶ�������ʽΪ wav �ļ�����������Ϊ�豸���
    if (!SDL_SetAudioStreamFormat(stream, &wavSpec, &spec)) {
        std::cerr << "�޷�������Ƶ����ʽ: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!SDL_PutAudioStreamData(stream, wavData.data(), static_cast<int>(wavData.size()))) {
        std::cerr << "д����Ƶ��ʧ��: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

// ���� WAV �ļ���������Ƶ���ݵ��ڲ����棬����������
bool SDLAudioPlayer::LoadWAV(const char* filePath) {
    Uint8* buffer = nullptr;
    Uint32 length = 0;
    SDL_AudioSpec fileSpec;

    if (!SDL_LoadWAV(filePath, &fileSpec, &buffer, &length)) {
        std::cerr << "�޷����� WAV �ļ�: " << SDL_GetError() << std::endl;
        return false;
    }
    // ������صĹ������ݵ���Ա����
    wavSpec = fileSpec;
    wavData.assign(buffer, buffer + length);
    SDL_free(buffer);
    // ע�⣺������ SDL_PutAudioStreamData����ʱ���������ݣ�������
    return true;
}

// ������Ƶ�������Ƶ��Ϊ�գ������� WAV ���ݣ�
void SDLAudioPlayer::Play() {
    // �����Ƶ����û�����ݣ����� WAV ���ݣ�ֻ����һ�Σ�
    if (SDL_GetAudioStreamQueued(stream) == 0) {
        if (!PushWavData()) {
            std::cerr << "���� WAV ����ʧ�ܣ�" << std::endl;
            return;
        }
    }
    SDL_ResumeAudioDevice(device);
}

// ��ͷ��ʼ���ţ�������������������ݣ�Ȼ�󲥷�
void SDLAudioPlayer::RePlay() {
    Stop(); // �����Ƶ������
    if (!PushWavData()) {
        std::cerr << "���� WAV ����ʧ�ܣ�" << std::endl;
        return;
    }
    SDL_ResumeAudioDevice(device);
}

// ��ͣ����
void SDLAudioPlayer::Pause() {
    SDL_PauseAudioDevice(device);
}

// ֹͣ���ţ������Ƶ������
void SDLAudioPlayer::Stop() {

    SDL_PauseAudioDevice(device);
	SDL_ClearAudioStream(stream);

}

// �������Ҳ����ݣ���ͬƵ����������д����Ƶ����Ȼ�󲥷�
void SDLAudioPlayer::GenerateSineWave(float frequency, int durationMs) {
    int sampleCount = (spec.freq * durationMs) / 1000;
    std::vector<float> buffer(sampleCount);

    // ��̬����������λ���Ա������Ҳ�������
    static float phase = 0.0f;
    float phaseIncrement = 2.0f * M_PI * frequency / spec.freq;

    for (int i = 0; i < sampleCount; i++) {
        buffer[i] = sinf(phase);
        phase += phaseIncrement;
        if (phase > 2.0f * M_PI)
            phase -= 2.0f * M_PI;
    }

    // �����Ҳ�����ֱ��������Ƶ��
    SDL_PutAudioStreamData(stream, buffer.data(), sampleCount * sizeof(float));
    SDL_ResumeAudioDevice(device);
}

//���ɸ��ټ�88����ƵƵ��
float SDLAudioPlayer::PianoKeyFrequency(int keyNumber) {
    if (keyNumber < 1 || keyNumber > 88) {
        std::cerr << "���ű�����1��88֮�䣡" << std::endl;
        return 0.0f;
    }
    return 440.0f * std::pow(2.0f, (keyNumber - 49) / 12.0f);
}
