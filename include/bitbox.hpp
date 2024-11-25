#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

class AudioData {
public:
    int length;
    double** data;
};

class Sample {
public:
    AudioData* data;
};

struct TrackConfig {
    double volume;
    int samplesDelay;
};

struct AutomizationPoint {
    int x = 0;      // X-Sample (0 to project.length)
    double y = 0;   // Y-Value (-1 to +1)
};

struct Automization {
    vector<AutomizationPoint> points;
};

struct Parameter {
    string name;
    string id;
    double value;

    Automization automization;
    bool isAutomized();
};

class Effect {
public:
    string name;
    vector<Parameter> parameters;
};

class Track {
public:
    Sample* sample;
    TrackConfig config;
    vector<Effect> effects;
};

class Project {
public:
    int length = 44100 * 10;
    
    vector<Track*> tracks;
};

struct ProjectInfo {
    string name;
};

class BitBox {
public:
    UIManager* ui;

    Project* currentProject;
    bool isRunning = true;

    BitBox();
    void loop();
    void events();
}

class Device {
public:

    vector<ProjectInfo> getProjects();
    Project* loadProject(string);
    void saveProject(Project);
};