#pragma once

#include "bitbox.hpp"
#include <SDL_image.h>

#include <vector>
#include <string>
#include <functional>

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

class UIManager {
public:
    BitBox* bb;
    Charset* charset;

    string title = "";

    UIView view = UIView::PROJECT;
    DropdownOverlay dropdownOverlay;

    // If empty, show normal UI. If not, show those one by one
    vector<Rows*> rowsStack;

    int testValue = 1;

    int projectTrackRow = 0;
    int projectTrackCol = 0;
    bool projectTrackRowIsSelected = false;

    int trackConfRow = 0;

    int trackOpenedTrackIndex = -1;

    int trackEffectsEffectIndex = 0;
    int trackEffectSettingsRow = 0;

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