#include "device.hpp"
#include <math.h>

// Effects
#include "lfo.hpp"
#include "test2.hpp"

Device::Device() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    
    window = SDL_CreateWindow("EMULATOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 300, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);
    
    this->pixels = (bool*) malloc(sizeof(bool) * screenWidth * screenHeight);
    this->points = (SDL_Point*)malloc(sizeof(SDL_Point) * this->screenWidth * this->screenHeight);
}

void Device::updateScreen() {
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);
    SDL_RenderClear(renderer);

    int count = 0;
    for (int i = 0; i < this->screenWidth * this->screenHeight; i++) {
        if (pixels[i] == true) {
            points[count].x = i % this->screenWidth;
            points[count].y = i / this->screenWidth;
            count++;
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoints(renderer, points, count);

    SDL_RenderPresent(renderer);
}

/*Sample* Device::loadSampleFromPath(string path) {
    Sample* sample = new Sample();
    int size = 44100 * 2;
    sample->data->length = size;
    sample->data->data = (double**)malloc(sizeof(double*) * 2);
    sample->data->data[0] = (double*)malloc(sizeof(double) * size);
    sample->data->data[1] = (double*)malloc(sizeof(double) * size);
    for (int i = 0; i < size; i++) {
        sample->data->data[0][i] = sin(2 * M_PI * 440 * (i / 44100.0)) * (1 - i / (double)size);
        sample->data->data[1][i] = sample->data->data[0][i];
    }
    return sample;
}*/

vector<ProjectInfo> Device::getProjects() {
    return {};
}

Project* Device::loadProject(string name) {
    Project* project = new Project();
    Track* track1 = new Track();
    Track* track2 = new Track();
    Track* track3 = new Track();
    Track* track4 = new Track();

    track1->volume = 0.8;
    track1->msDelay = 0.0;
    track1->sample = this->loadSampleFromPath("samples/kick.wav");

    track2->volume = 0.5;
    track2->msDelay = 0.5;
    track2->sample = this->loadSampleFromPath("samples/ride.wav");
    
    Effect* fx1 = new BitBoxEffect::TestEffect();
    track2->effects.push_back(fx1);
    Effect* fx2 = new BitBoxEffect::Test2Effect();
    track2->effects.push_back(fx2);

    project->tracks.push_back(track1);
    project->tracks.push_back(track2);
    project->tracks.push_back(track3);
    project->tracks.push_back(track4);

    return project;
}

void Device::saveSample(Sample* sample) {
    AudioFile<double> file;
    file.samples.clear();

    for (int c = 0; c < 2; c++) {
        vector<double> channelSamples;
        for (int s = 0; s < sample->data->length; s++) {
            channelSamples.push_back(sample->data->data[c][s]);
        }
        file.samples.push_back(channelSamples);
    }

    file.save(this->audioDir + sample->path);

    printf("[DEVICEAPI] Saved sample to %s\n", (this->audioDir + sample->path).c_str());
}

Sample* Device::loadSampleFromPath(string path) {
    AudioFile<double>* file = new AudioFile<double>(this->audioDir + path);

    // TODO: Check if file is 44100Hz, 2-Channel, etc.
    // TODO: samples that are too long would break this

    Sample* sample = new Sample(file->getNumSamplesPerChannel());
    sample->path = path;
    for (int c = 0; c < 2; c++) {
        for (int s = 0; s < file->getNumSamplesPerChannel(); s++) {
            sample->data->data[c][s] = file->samples[c][s];
        }
    }

    printf("[DEVICEAPI] Loaded sample from %s\n", (this->audioDir + path).c_str());

    return sample;
}
