#pragma once

#include <vector>
#include <string>
#include <chrono>

#include <SDL.h>
#include <SDL_image.h>

#include "ui.hpp"
#include "device.hpp"

using std::vector;
using std::string;

class AudioData {
public:
    int length;
    double** data;
    ~AudioData();
    AudioData();
    AudioData(int);
};

// A sample is a file on the sd-card with AudioData
class Sample {
public:
    AudioData* data;
    string path;
    ~Sample();
    Sample();
    Sample(int);
};

struct AutomizationPoint {
    double x = 0;   // X-Sample (0 to 1)
    double y = 0;   // Y-Value (-1 to +1)
};

struct Automization {
    vector<AutomizationPoint> points = {{0, 0}, {1, 0}};
    double get(double x) {
        for (int i = 0; i < points.size(); i++) {
            if (x == 0.0) {
                return points[i].y;
            } else if (x == 1.0) {
                return points[points.size() - 1].y;
            } else if (points[i].x > x) {
                // X is too far, interpolate between last X and this one
                double x1 = points[i - 1].x;
                double x2 = points[i].x;
                double y1 = points[i - 1].y;
                double y2 = points[i].y;
                
                // 0 = x1, 1 = x2
                double rel = (x - x1) / (double)(x2 - x1);
                double y = y1 + rel * (double)(y2 - y1);
                return y;
            }
        }

        return 0.0;
    }
};

enum class ParameterType {
    PROCENT, MS, SAMPLES
};

struct Parameter {
    string name;
    string id;
    ParameterType type = ParameterType::PROCENT;

    double min = 0.0;
    double max = 1.0;
    double value;
    double defaultValue = 0.0;

    Automization automization;
    bool isAutomized();
};

class Effect {
public:
    string name;
    vector<Parameter> parameters;
    int sampleRate;
    
    Effect(string);
    virtual void process(double**, double**, int) = 0;

    // Get calculated parameter value based of index and relative sample position
    double getParameterValue(int, double);
};

enum class TrackMode {
    NONE = 0, SOLO, MUTE
};

class BitBox;

class Track {
public:
    Sample* sample = nullptr;
    vector<Effect*> effects;
    BitBox* bitBox;

    int trackIndex = -1;

    double volume;
    TrackMode trackMode = TrackMode::NONE;
    int msDelay = 0.0;

    Track();
    void freeze();
    void duplicate(Track*);
    AudioData* render();
    void clear();
};

class Project {
public:
    int length = 44100 * 1;
    int sampleRate = 44100;
    
    vector<Track*> tracks;

    Project();
    AudioData* render();
    Sample* exportToSample();
};

struct ProjectInfo {
    string name;
};

class Device;
class UIManager;

enum class InputEventType {
    BACK = 0, SET, PLAY, ROTARY,
    ROTARY_LEFT, ROTARY_RIGHT,
    N_INPUTS
};

enum class InputEventValue {
    NONE, JUST_PRESSED, PRESSED, JUST_RELEASED
};

struct InputEvent {
    InputEventType type;
    InputEventValue value;
};

class BitBox {
public:
    UIManager* ui;
    Device* device;

    InputEventValue* inputs;

    Project* project;

    std::chrono::high_resolution_clock::time_point lastRenderTime;

    Project* currentProject;
    bool isRunning = true;

    BitBox();
    void loop();
    void events();

    bool inputIsPressed(InputEventType);
    bool inputIsJustPressed(InputEventType);

    void clearScreen();
    void setPixel(int, int, bool);

    void openProject(string);

    vector<string> getEffectNames();
    Effect* newEffect(string);
};
