#pragma once

#include "bitbox.hpp"
#include <SDL_image.h>

#include <vector>
#include <string>

using std::vector;
using std::string;

enum class UIView {
    PROJECT,
    TRACK_SAMP, TRACK_CONF, TRACK_FX
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

class BitBox;

class UIManager {
public:
    BitBox* bb;
    Charset* charset;

    string title = "";

    UIView view = UIView::PROJECT;
    int projectTrackRow = 0;
    int projectTrackCol = 0;
    bool projectTrackRowIsSelected = false;

    int trackOpenedTrackIndex = -1;

    UIManager();

    void drawBox(int, int, int, int, bool = true);
    void drawText(int, int, string, bool = false);
    void drawPixel(int, int, bool);
    void drawHorizontal(int);

    int calculateTextWidth(string, int, int, int);
    int getCharIndexFromChar(char);
};