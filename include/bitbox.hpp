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
    AudioData();
};

class Sample {
public:
    AudioData* data;
    Sample();
};

struct AutomizationPoint {
    int x = 0;      // X-Sample (0 to project.length)
    double y = 0;   // Y-Value (-1 to +1)
};

struct Automization {
    vector<AutomizationPoint> points = {{0, 0}, {1, 0}};
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
    Effect(string);
    virtual void process(double**, double**, int) = 0;
};

enum class TrackMode {
    NONE = 0, SOLO, MUTE
};

class Track {
public:
    Sample* sample;
    vector<Effect*> effects;

    double volume;
    TrackMode trackMode = TrackMode::NONE;
    double msDelay = 0.0;

    Track();
};

class Project {
public:
    int length = 44100 * 10;
    
    vector<Track*> tracks;

    Project();
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

    vector<string> getEffectNames();
    Effect* newEffect(string);
};
