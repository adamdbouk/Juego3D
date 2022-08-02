#include "audio.h"
#include <cassert>

std::map<std::string, Audio*> Audio::sLoadedAudios;

Audio::Audio() {
	
	sample = 0;
	channelSample = 0;
}

Audio::~Audio() {
}


void Audio::Stop(HCHANNEL channel) {
	BASS_ChannelStop(channel);
}

Audio* Audio::Get(const char* filename) {
	assert(filename);

	//check if loaded
	auto it = sLoadedAudios.find(filename);
	if (it != sLoadedAudios.end())
		return it->second;

	//load it
	Audio* audio = new Audio();
	audio->sample = BASS_SampleLoad(false, filename, 0, 0, 3, 0);
	sLoadedAudios[filename] = audio;
	if (audio->sample == 0)
	{
		delete audio;
		return NULL;
	}
	return audio;
}

HCHANNEL* Audio::Play(const char* filename) {
	assert(filename);

	Audio* audio = Audio::Get(filename);
	if (audio == NULL) {
		return NULL;
	}
	HCHANNEL channelSample;
	channelSample = BASS_SampleGetChannel(audio->sample, false);
	
	BASS_ChannelPlay(channelSample, true);
	return &channelSample;
}
