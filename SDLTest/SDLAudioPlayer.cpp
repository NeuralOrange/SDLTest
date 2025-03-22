#include "SDLAudioPlayer.h"

// 构造函数：初始化音频系统、打开设备、创建并绑定音频流
SDLAudioPlayer::SDLAudioPlayer(int sampleRate, int channels) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL 音频初始化失败: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // 设置设备输出规格
    spec.freq = sampleRate;
    spec.format = SDL_AUDIO_F32;  // 32位浮点数格式
    spec.channels = channels;

    // 打开默认播放设备
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (device == 0) {
        std::cerr << "无法打开音频设备: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // 创建音频流，初始时输入格式同输出格式
    stream = SDL_CreateAudioStream(&spec, &spec);
    if (!stream) {
        std::cerr << "无法创建音频流: " << SDL_GetError() << std::endl;
        exit(1);
    }
    SDL_BindAudioStream(device, stream);

    // 将设备置于暂停状态，防止自动播放
    SDL_PauseAudioDevice(device);
}

// 析构函数：释放资源
SDLAudioPlayer::~SDLAudioPlayer() {
    SDL_DestroyAudioStream(stream);
    SDL_CloseAudioDevice(device);
    SDL_Quit();
}

// 内部方法：将缓存中的 WAV 数据推入音频流
bool SDLAudioPlayer::PushWavData() {
    if (wavData.empty()) {
        std::cerr << "未加载 WAV 数据！" << std::endl;
        return false;
    }
    // 设置音频流输入格式为 wav 文件规格，输出保持为设备规格
    if (!SDL_SetAudioStreamFormat(stream, &wavSpec, &spec)) {
        std::cerr << "无法设置音频流格式: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!SDL_PutAudioStreamData(stream, wavData.data(), static_cast<int>(wavData.size()))) {
        std::cerr << "写入音频流失败: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

// 加载 WAV 文件，保存音频数据到内部缓存，不立即播放
bool SDLAudioPlayer::LoadWAV(const char* filePath) {
    Uint8* buffer = nullptr;
    Uint32 length = 0;
    SDL_AudioSpec fileSpec;

    if (!SDL_LoadWAV(filePath, &fileSpec, &buffer, &length)) {
        std::cerr << "无法加载 WAV 文件: " << SDL_GetError() << std::endl;
        return false;
    }
    // 保存加载的规格和数据到成员变量
    wavSpec = fileSpec;
    wavData.assign(buffer, buffer + length);
    SDL_free(buffer);
    // 注意：不调用 SDL_PutAudioStreamData，此时仅加载数据，不播放
    return true;
}

// 播放音频（如果音频流为空，则推送 WAV 数据）
void SDLAudioPlayer::Play() {
    // 如果音频流中没有数据，推送 WAV 数据（只播放一次）
    if (SDL_GetAudioStreamQueued(stream) == 0) {
        if (!PushWavData()) {
            std::cerr << "推送 WAV 数据失败！" << std::endl;
            return;
        }
    }
    SDL_ResumeAudioDevice(device);
}

// 重头开始播放：先清空流，再推送数据，然后播放
void SDLAudioPlayer::RePlay() {
    Stop(); // 清空音频流数据
    if (!PushWavData()) {
        std::cerr << "推送 WAV 数据失败！" << std::endl;
        return;
    }
    SDL_ResumeAudioDevice(device);
}

// 暂停播放
void SDLAudioPlayer::Pause() {
    SDL_PauseAudioDevice(device);
}

// 停止播放：清空音频流数据
void SDLAudioPlayer::Stop() {

    SDL_PauseAudioDevice(device);
	SDL_ClearAudioStream(stream);

}

// 生成正弦波数据（不同频率声音）并写入音频流，然后播放
void SDLAudioPlayer::GenerateSineWave(float frequency, int durationMs) {
    int sampleCount = (spec.freq * durationMs) / 1000;
    std::vector<float> buffer(sampleCount);

    // 静态变量保存相位，以保持正弦波连续性
    static float phase = 0.0f;
    float phaseIncrement = 2.0f * M_PI * frequency / spec.freq;

    for (int i = 0; i < sampleCount; i++) {
        buffer[i] = sinf(phase);
        phase += phaseIncrement;
        if (phase > 2.0f * M_PI)
            phase -= 2.0f * M_PI;
    }

    // 将正弦波数据直接推入音频流
    SDL_PutAudioStreamData(stream, buffer.data(), sampleCount * sizeof(float));
    SDL_ResumeAudioDevice(device);
}

//生成钢琴键88键音频频率
float SDLAudioPlayer::PianoKeyFrequency(int keyNumber) {
    if (keyNumber < 1 || keyNumber > 88) {
        std::cerr << "键号必须在1到88之间！" << std::endl;
        return 0.0f;
    }
    return 440.0f * std::pow(2.0f, (keyNumber - 49) / 12.0f);
}
