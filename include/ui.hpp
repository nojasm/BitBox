#pragma once

#include "bitbox.hpp"
#include <SDL_image.h>

#include <vector>
#include <string>
#include <functional>
#include <chrono>

using std::vector;
using std::string;

enum class UIView {
    PROJECT,
    TRACK_SAMP,
    TRACK_CONF,

    TRACK_FX, TRACK_FX_EFFECT, TRACK_FX_SETTINGS
};

struct CharsetCharacter {
    bool* pixels;
};

class Charset {
public:
    int charWidth = 5;
    int charHeight = 9;
    vector<CharsetCharacter> chars;

    Charset() {
        #include "charset.hpp"  // Loads in [vector<vector<bool>> chars]
        for (int i = 0; i < chars.size(); i++) {
            CharsetCharacter cc;
            cc.pixels = (bool*)malloc(sizeof(bool) * charWidth * charHeight);
            for (int j = 0; j < chars[i].size(); j++) {
                cc.pixels[j] = chars[i][j];
            }
            this->chars.push_back(cc);
        }
    }
};

struct DropdownOverlay {
    bool isOpened = false;
    bool justClosed = false;
    
    string title;
    int lastIndex = 1;
    vector<string> options;
    int row = 0;
};

class BitBox;

enum class RowEntryType {
    VALUE, DROPDOWN, BUTTON, TOGGLE
};

struct RowEntry {
    string key;
    RowEntryType type;

    // Function that should return the current value to the row entry
    std::function<double(void)> getValueFunc;

    // Argument: Current new value. Return that value to confirm it
    // is valid or return a different one if eg. the value is out of range
    std::function<double(double)> handleChangeFunc;
};

enum class InputEventValue;

class Rows {
public:
    vector<RowEntry> options;
    int current = 0;
    int scroll = 0;

    bool rowIsSelected = false;

    Rows();
};


// Use this to handle scrolling list functionalities
class ScrollingList {
public:
    bool editeableValues = true;
    int currentRow = 0;
    int currentScroll = 0;
    int nElements = 4;

    int relativeRow = 0;

    bool editing = false;
    bool exit = false;

    double addValue = 0;  // +1 for increase, -1 for decrease, 0 if not changing

    int getAbsoluteIndex(int rel) {
        return rel + currentScroll;
    }

    int getRelativeIndex(int abs) {
        return abs - currentScroll;
    }

    bool shouldDraw(int index, int* yValue) {
        if (index >= currentScroll && index < (currentScroll + 4)) {
            *yValue = 11 + (index - currentScroll) * 11;
            return true;
        }
        return false;
    }

    // ROTARY left (scroll / decrease value)
    void rotaryLeft() {
        if (editing) {
            addValue = -1;
        } else {
            currentRow = SDL_clamp(currentRow - 1, 0, nElements - 1);
            if (currentRow < currentScroll)
                currentScroll--;
        }
    }

    // ROTARY right (scroll / increase value)  
    void rotaryRight() {
        if (editing) {
            addValue = 1;
        } else {
            currentRow = SDL_clamp(currentRow + 1, 0, nElements - 1);
            if ((currentRow - currentScroll) > 3) {
                currentScroll++;
            }
        }
    }

    // SET button
    void set() {
        if (editeableValues) {
            editing = !editing;
        }
    }


    void back() {
        if (editing)
            editing = false;
        else
            exit = true;
    }
};

class UIManager {
public:
    BitBox* bb;
    Charset* charset;

    string title = "";

    UIView view = UIView::PROJECT;
    DropdownOverlay dropdownOverlay;

    // If empty, show normal UI. If not, show those one by one
    vector<Rows*> rowsStack;

    ScrollingList* trackConfList;

    int testValue = 1;

    // To show button that lights up, set this to true and reset
    // buttonLightUpStart.
    bool buttonLightUp = false;
    std::chrono::high_resolution_clock::time_point buttonLightUpStart;
    int buttonLightUpMs = 100;

    int projectTrackRow = 0;
    int projectTrackCol = 0;
    bool projectTrackRowIsSelected = false;

    int trackConfRow = 0;

    int trackOpenedTrackIndex = -1;

    int trackEffectsEffectIndex = 0;
    int trackEffectSettingsRow = 0;
    
    int trackEffectsParameterRow = 0;           // Currently selected parameter
    int trackEffectsParameterScroll = 0;        // Current parameter scroll value
    bool trackEffectsParameterEdit = false;     // Currently editing parameter?

    UIManager();

    void fillBox(int, int, int, int, bool = true);
    void drawBox(int, int, int, int, bool = true);
    void drawText(int, int, string, bool = false);
    void drawPixel(int, int, bool);
    void drawHorizontal(int);
    void drawRows(Rows*);

    void openDropdown(string, vector<string>, int);

    int calculateTextWidth(string, int, int, int);
    int getCharIndexFromChar(char);
};