#include "SDLAudioPlayer.h"


///////////////////////
// SDLAudioPlayer 实现
///////////////////////

SDLAudioPlayer::SDLAudioPlayer(int sampleRate, int channels) {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL 音频初始化失败: " << SDL_GetError() << std::endl;
		exit(1);
	}
	// 设定统一的输出规格
	spec.freq = sampleRate;
	spec.format = SDL_AUDIO_F32;  // 使用 32 位浮点格式
	spec.channels = channels;

	// 打开默认播放设备
	device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
	if (device == 0) {
		std::cerr << "无法打开音频设备: " << SDL_GetError() << std::endl;
		exit(1);
	}
	// 初始时暂停设备，防止数据自动播放
	SDL_PauseAudioDevice(device);
}

SDLAudioPlayer::~SDLAudioPlayer() {
	// 释放所有音频项的流
	for (auto& pair : audioMap) {
		if (pair.second.stream) {
			SDL_DestroyAudioStream(pair.second.stream);
		}
	}
	SDL_CloseAudioDevice(device);
	SDL_Quit();
}

// 内部函数：将 item.wavData 推入 item.stream，先设置输入格式（wavSpec）转换到输出规格（spec）
bool SDLAudioPlayer::PushWavData(AudioItem& item) {
	if (item.wavData.empty()) {
		std::cerr << "WAV 数据为空！" << std::endl;
		return false;
	}
	// 设置音频流格式转换：输入为 item.wavSpec，输出为 spec
	if (!SDL_SetAudioStreamFormat(item.stream, &item.wavSpec, &spec)) {
		std::cerr << "无法设置音频流格式: " << SDL_GetError() << std::endl;
		return false;
	}
	if (!SDL_PutAudioStreamData(item.stream, item.wavData.data(), static_cast<int>(item.wavData.size()))) {
		std::cerr << "写入音频流失败: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

// LoadWAV: 加载 WAV 文件，保存数据到内部 map 中，不立即播放
bool SDLAudioPlayer::LoadWAV(const std::string& name, const char* filePath) {
	Uint8* buffer = nullptr;
	Uint32 length = 0;
	SDL_AudioSpec fileSpec;
	if (!SDL_LoadWAV(filePath, &fileSpec, &buffer, &length)) {
		std::cerr << "无法加载 WAV 文件: " << SDL_GetError() << std::endl;
		return false;
	}
	AudioItem item;
	// 保存 WAV 数据和规格
	item.wavData.assign(buffer, buffer + length);
	item.wavSpec = fileSpec;
	SDL_free(buffer);
	// 创建一个新的音频流：输入格式为 fileSpec，输出为统一的 spec
	item.stream = SDL_CreateAudioStream(&item.wavSpec, &spec);
	if (!item.stream) {
		std::cerr << "无法创建音频流: " << SDL_GetError() << std::endl;
		return false;
	}
	// 保存到 map 中
	audioMap[name] = item;
	return true;
}

// Play: 播放指定名称的音频
void SDLAudioPlayer::Play(const std::string& name) {
	auto it = audioMap.find(name);
	if (it == audioMap.end()) {
		std::cerr << "音频项不存在: " << name << std::endl;
		return;
	}
	// 如果流中没有数据，则推送 WAV 数据
	if (SDL_GetAudioStreamQueued(it->second.stream) == 0) {
		if (!PushWavData(it->second)) {
			std::cerr << "推送音频数据失败: " << name << std::endl;
			return;
		}
	}
	SDL_BindAudioStream(device, it->second.stream);
	SDL_ResumeAudioDevice(device);
}

// RePlay: 清空流并从头开始播放
void SDLAudioPlayer::RePlay(const std::string& name) {
	auto it = audioMap.find(name);
	if (it == audioMap.end()) {
		std::cerr << "音频项不存在: " << name << std::endl;
		return;
	}
	SDL_PauseAudioDevice(device);
	SDL_ClearAudioStream(it->second.stream);
	SDL_FlushAudioStream(it->second.stream);
	if (!PushWavData(it->second)) {
		std::cerr << "重新推送音频数据失败: " << name << std::endl;
		return;
	}
	SDL_ResumeAudioDevice(device);
}

// Stop: 停止指定音频（清空流并暂停设备）
void SDLAudioPlayer::Stop(const std::string& name) {
	auto it = audioMap.find(name);
	if (it == audioMap.end()) {
		std::cerr << "音频项不存在: " << name << std::endl;
		return;
	}
	SDL_FlushAudioStream(it->second.stream);
	SDL_PauseAudioDevice(device);
}

// GenerateSineWave: 生成正弦波数据作为一个新的音频项，并以 name 存储播放
void SDLAudioPlayer::GenerateSineWave(const std::string& name, float frequency, int durationMs) {
	if (audioMap.find(name) != audioMap.end())
	{
		std::cerr << "同名音频流已存在: " << SDL_GetError() << std::endl;
		return;
	}
	AudioItem item;
	// 对于合成音频，直接使用输出规格，不需要格式转换
	item.wavSpec = spec;
	item.stream = SDL_CreateAudioStream(&spec, &spec);
	if (!item.stream) {
		std::cerr << "无法创建合成音频流: " << SDL_GetError() << std::endl;
		return;
	}
	int sampleCount = (spec.freq * durationMs) / 1000;
	std::vector<float> buffer(sampleCount);
	// 用局部相位生成正弦波（此处每次生成独立的波形）
	float phase = 0.0f;
	float phaseIncrement = 2.0f * M_PI * frequency / spec.freq;
	for (int i = 0; i < sampleCount; i++) {
		buffer[i] = sinf(phase);
		phase += phaseIncrement;
		if (phase > 2.0f * M_PI)
			phase -= 2.0f * M_PI;
	}
	item.wavData.resize(sampleCount * sizeof(float));
	memcpy(item.wavData.data(), buffer.data(), sampleCount * sizeof(float));
	SDL_PutAudioStreamData(item.stream, buffer.data(), sampleCount * sizeof(float));
	// 保存到 map
	audioMap[name] = item;
}

void SDLAudioPlayer::GenerateChord(const std::string& name, const std::vector<float>& frequencies, int durationMs)
{
	if (audioMap.find(name) != audioMap.end())
	{
		std::cerr << "同名音频流已存在: " << SDL_GetError() << std::endl;
		return;
	}
	AudioItem item;
	item.wavSpec = spec;
	item.stream = SDL_CreateAudioStream(&spec, &spec);
	if (!item.stream) {
		std::cerr << "无法创建合成音频流: " << SDL_GetError() << std::endl;
		return;
	}
	// 计算采样点数
	int sampleCount = (spec.freq * durationMs) / 1000;
	// 创建缓冲区，初始为 0
	std::vector<float> buffer(sampleCount, 0.0f);

	// 对于每个频率，生成正弦波并累加到缓冲区
	for (auto frequency : frequencies) {
		float phase = 0.0f;
		float phaseIncrement = 2.0f * M_PI * frequency / spec.freq;
		for (int i = 0; i < sampleCount; i++) {
			buffer[i] += sinf(phase);
			phase += phaseIncrement;
			if (phase > 2.0f * M_PI)
				phase -= 2.0f * M_PI;
		}
	}

	// 归一化混合结果（除以和弦中正弦波个数）
	int count = frequencies.size();
	for (int i = 0; i < sampleCount; i++) {
		buffer[i] /= count;
	}
	item.wavData.resize(sampleCount * sizeof(float));
	memcpy(item.wavData.data(), buffer.data(), sampleCount * sizeof(float));
	// 将混合后的数据推入音频流，并启动播放
	SDL_PutAudioStreamData(item.stream, buffer.data(), sampleCount * sizeof(float));
	audioMap[name] = item;
}



// C大调和弦（C4, E4, G4）
std::vector<float> SpecialAudio::C_Major = { 261.63f, 329.63f, 392.00f };

// D大调和弦（D4, F#4, A4）
std::vector<float> SpecialAudio::D_Major = { 293.66f, 369.99f, 440.00f };

// E大调和弦（E4, G#4, B4）
std::vector<float> SpecialAudio::E_Major = { 329.63f, 415.30f, 493.88f };

// F大调和弦（F4, A4, C5）
std::vector<float> SpecialAudio::F_Major = { 349.23f, 440.00f, 523.25f };

// G大调和弦（G4, B4, D5）
std::vector<float> SpecialAudio::G_Major = { 392.00f, 493.88f, 587.33f };

// A大调和弦（A4, C#5, E5）
std::vector<float> SpecialAudio::A_Major = { 440.00f, 554.37f, 659.26f };

// B大调和弦（B4, D#5, F#5）
std::vector<float> SpecialAudio::B_Major = { 493.88f, 622.25f, 739.99f };

// A小调和弦（A4, C5, E5）
std::vector<float> SpecialAudio::A_Minor = { 440.00f, 523.25f, 659.26f };

// D小调和弦（D4, F4, A4）
std::vector<float> SpecialAudio::D_Minor = { 293.66f, 349.23f, 440.00f };

// E小调和弦（E4, G4, B4）
std::vector<float> SpecialAudio::E_Minor = { 329.63f, 392.00f, 493.88f };

// C7（C Dominant Seventh: C4, E4, G4, Bb4）
std::vector<float> SpecialAudio::C_Dom7 = { 261.63f, 329.63f, 392.00f, 466.16f };

// G7（G Dominant Seventh: G4, B4, D5, F5）
std::vector<float> SpecialAudio::G_Dom7 = { 392.00f, 493.88f, 587.33f, 698.46f };