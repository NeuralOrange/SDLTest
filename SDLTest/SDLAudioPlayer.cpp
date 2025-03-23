#include "SDLAudioPlayer.h"


///////////////////////
// SDLAudioPlayer ʵ��
///////////////////////

SDLAudioPlayer::SDLAudioPlayer(int sampleRate, int channels) {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL ��Ƶ��ʼ��ʧ��: " << SDL_GetError() << std::endl;
		exit(1);
	}
	// �趨ͳһ��������
	spec.freq = sampleRate;
	spec.format = SDL_AUDIO_F32;  // ʹ�� 32 λ�����ʽ
	spec.channels = channels;

	// ��Ĭ�ϲ����豸
	device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
	if (device == 0) {
		std::cerr << "�޷�����Ƶ�豸: " << SDL_GetError() << std::endl;
		exit(1);
	}
	// ��ʼʱ��ͣ�豸����ֹ�����Զ�����
	SDL_PauseAudioDevice(device);
}

SDLAudioPlayer::~SDLAudioPlayer() {
	// �ͷ�������Ƶ�����
	for (auto& pair : audioMap) {
		if (pair.second.stream) {
			SDL_DestroyAudioStream(pair.second.stream);
		}
	}
	SDL_CloseAudioDevice(device);
	SDL_Quit();
}

// �ڲ��������� item.wavData ���� item.stream�������������ʽ��wavSpec��ת����������spec��
bool SDLAudioPlayer::PushWavData(AudioItem& item) {
	if (item.wavData.empty()) {
		std::cerr << "WAV ����Ϊ�գ�" << std::endl;
		return false;
	}
	// ������Ƶ����ʽת��������Ϊ item.wavSpec�����Ϊ spec
	if (!SDL_SetAudioStreamFormat(item.stream, &item.wavSpec, &spec)) {
		std::cerr << "�޷�������Ƶ����ʽ: " << SDL_GetError() << std::endl;
		return false;
	}
	if (!SDL_PutAudioStreamData(item.stream, item.wavData.data(), static_cast<int>(item.wavData.size()))) {
		std::cerr << "д����Ƶ��ʧ��: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

// LoadWAV: ���� WAV �ļ����������ݵ��ڲ� map �У�����������
bool SDLAudioPlayer::LoadWAV(const std::string& name, const char* filePath) {
	Uint8* buffer = nullptr;
	Uint32 length = 0;
	SDL_AudioSpec fileSpec;
	if (!SDL_LoadWAV(filePath, &fileSpec, &buffer, &length)) {
		std::cerr << "�޷����� WAV �ļ�: " << SDL_GetError() << std::endl;
		return false;
	}
	AudioItem item;
	// ���� WAV ���ݺ͹��
	item.wavData.assign(buffer, buffer + length);
	item.wavSpec = fileSpec;
	SDL_free(buffer);
	// ����һ���µ���Ƶ���������ʽΪ fileSpec�����Ϊͳһ�� spec
	item.stream = SDL_CreateAudioStream(&item.wavSpec, &spec);
	if (!item.stream) {
		std::cerr << "�޷�������Ƶ��: " << SDL_GetError() << std::endl;
		return false;
	}
	// ���浽 map ��
	audioMap[name] = item;
	return true;
}

// Play: ����ָ�����Ƶ���Ƶ
void SDLAudioPlayer::Play(const std::string& name) {
	auto it = audioMap.find(name);
	if (it == audioMap.end()) {
		std::cerr << "��Ƶ�����: " << name << std::endl;
		return;
	}
	// �������û�����ݣ������� WAV ����
	if (SDL_GetAudioStreamQueued(it->second.stream) == 0) {
		if (!PushWavData(it->second)) {
			std::cerr << "������Ƶ����ʧ��: " << name << std::endl;
			return;
		}
	}
	SDL_BindAudioStream(device, it->second.stream);
	SDL_ResumeAudioDevice(device);
}

// RePlay: ���������ͷ��ʼ����
void SDLAudioPlayer::RePlay(const std::string& name) {
	auto it = audioMap.find(name);
	if (it == audioMap.end()) {
		std::cerr << "��Ƶ�����: " << name << std::endl;
		return;
	}
	SDL_PauseAudioDevice(device);
	SDL_ClearAudioStream(it->second.stream);
	SDL_FlushAudioStream(it->second.stream);
	if (!PushWavData(it->second)) {
		std::cerr << "����������Ƶ����ʧ��: " << name << std::endl;
		return;
	}
	SDL_ResumeAudioDevice(device);
}

// Stop: ָֹͣ����Ƶ�����������ͣ�豸��
void SDLAudioPlayer::Stop(const std::string& name) {
	auto it = audioMap.find(name);
	if (it == audioMap.end()) {
		std::cerr << "��Ƶ�����: " << name << std::endl;
		return;
	}
	SDL_FlushAudioStream(it->second.stream);
	SDL_PauseAudioDevice(device);
}

// GenerateSineWave: �������Ҳ�������Ϊһ���µ���Ƶ����� name �洢����
void SDLAudioPlayer::GenerateSineWave(const std::string& name, float frequency, int durationMs) {
	if (audioMap.find(name) != audioMap.end())
	{
		std::cerr << "ͬ����Ƶ���Ѵ���: " << SDL_GetError() << std::endl;
		return;
	}
	AudioItem item;
	// ���ںϳ���Ƶ��ֱ��ʹ�������񣬲���Ҫ��ʽת��
	item.wavSpec = spec;
	item.stream = SDL_CreateAudioStream(&spec, &spec);
	if (!item.stream) {
		std::cerr << "�޷������ϳ���Ƶ��: " << SDL_GetError() << std::endl;
		return;
	}
	int sampleCount = (spec.freq * durationMs) / 1000;
	std::vector<float> buffer(sampleCount);
	// �þֲ���λ�������Ҳ����˴�ÿ�����ɶ����Ĳ��Σ�
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
	// ���浽 map
	audioMap[name] = item;
}

void SDLAudioPlayer::GenerateChord(const std::string& name, const std::vector<float>& frequencies, int durationMs)
{
	if (audioMap.find(name) != audioMap.end())
	{
		std::cerr << "ͬ����Ƶ���Ѵ���: " << SDL_GetError() << std::endl;
		return;
	}
	AudioItem item;
	item.wavSpec = spec;
	item.stream = SDL_CreateAudioStream(&spec, &spec);
	if (!item.stream) {
		std::cerr << "�޷������ϳ���Ƶ��: " << SDL_GetError() << std::endl;
		return;
	}
	// �����������
	int sampleCount = (spec.freq * durationMs) / 1000;
	// ��������������ʼΪ 0
	std::vector<float> buffer(sampleCount, 0.0f);

	// ����ÿ��Ƶ�ʣ��������Ҳ����ۼӵ�������
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

	// ��һ����Ͻ�������Ժ��������Ҳ�������
	int count = frequencies.size();
	for (int i = 0; i < sampleCount; i++) {
		buffer[i] /= count;
	}
	item.wavData.resize(sampleCount * sizeof(float));
	memcpy(item.wavData.data(), buffer.data(), sampleCount * sizeof(float));
	// ����Ϻ������������Ƶ��������������
	SDL_PutAudioStreamData(item.stream, buffer.data(), sampleCount * sizeof(float));
	audioMap[name] = item;
}



// C������ң�C4, E4, G4��
std::vector<float> SpecialAudio::C_Major = { 261.63f, 329.63f, 392.00f };

// D������ң�D4, F#4, A4��
std::vector<float> SpecialAudio::D_Major = { 293.66f, 369.99f, 440.00f };

// E������ң�E4, G#4, B4��
std::vector<float> SpecialAudio::E_Major = { 329.63f, 415.30f, 493.88f };

// F������ң�F4, A4, C5��
std::vector<float> SpecialAudio::F_Major = { 349.23f, 440.00f, 523.25f };

// G������ң�G4, B4, D5��
std::vector<float> SpecialAudio::G_Major = { 392.00f, 493.88f, 587.33f };

// A������ң�A4, C#5, E5��
std::vector<float> SpecialAudio::A_Major = { 440.00f, 554.37f, 659.26f };

// B������ң�B4, D#5, F#5��
std::vector<float> SpecialAudio::B_Major = { 493.88f, 622.25f, 739.99f };

// AС�����ң�A4, C5, E5��
std::vector<float> SpecialAudio::A_Minor = { 440.00f, 523.25f, 659.26f };

// DС�����ң�D4, F4, A4��
std::vector<float> SpecialAudio::D_Minor = { 293.66f, 349.23f, 440.00f };

// EС�����ң�E4, G4, B4��
std::vector<float> SpecialAudio::E_Minor = { 329.63f, 392.00f, 493.88f };

// C7��C Dominant Seventh: C4, E4, G4, Bb4��
std::vector<float> SpecialAudio::C_Dom7 = { 261.63f, 329.63f, 392.00f, 466.16f };

// G7��G Dominant Seventh: G4, B4, D5, F5��
std::vector<float> SpecialAudio::G_Dom7 = { 392.00f, 493.88f, 587.33f, 698.46f };