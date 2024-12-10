#pragma once

#include "bitbox.hpp"
#include "AudioFile.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

struct ProjectInfo;
class Project;
class Sample;

class Device {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Point* points;

    const int screenWidth = 128;
    const int screenHeight = 64;
    bool* pixels;
    
    Device();
    void updateScreen();
    
    vector<ProjectInfo> getProjects();
    Project* loadProject(string);
    void saveProject(Project*);

    Sample* loadSampleFromPath(string);
    void saveSample(Sample*);
};