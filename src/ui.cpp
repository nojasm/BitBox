#include "ui.hpp"

UIManager::UIManager() {
    this->charset = new Charset();
}

void UIManager::drawPixel(int x, int y, bool on) {
    this->bb->setPixel(x, y, on);
}

int UIManager::getCharIndexFromChar(char c) {
    // Make character lowercase first
    if (c >= 'A' && c <= 'Z') {
        c -= 'A';
        c += 'a';
    }

    if (c >= 'a' && c <= 'z') {
        return c - 'a';
    } else if (c >= '0' && c <= '9') {
        return (c - '0') + 26;
    } else {
        return 23;
    }
}

void UIManager::drawBox(int x, int y, int w, int h, bool on) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            this->drawPixel(px, py, on);
        }
    }
}

void UIManager::drawHorizontal(int y) {
    for (int x = 0; x < this->bb->device->screenWidth; x++) {
        drawPixel(x, y, true);
    }
}

int UIManager::calculateTextWidth(string text, int charWidth, int sep, int space) {
    int textWidth = 0;
    for (int i = 0; i < text.size(); i++) {
        if (text[i] == ' ') {
            textWidth += space;
        } else if (i < (text.size() - 1)) {
            textWidth += charWidth + sep;
        } else {
            textWidth += charWidth;
        }
    }
    return textWidth;
}

void UIManager::drawText(int x, int y, string text, bool inverted) {
    int charWidth = charset->charWidth;
    int charHeight = charset->charHeight;
    int sep = 1;
    int space = 3;
    
    if (inverted) {
        this->drawBox(x - 1, y - 1, calculateTextWidth(text, charWidth, sep, space) + 2, charHeight + 2);
    }

    int xPos = x;
    for (int i = 0; i < text.size(); i++) {
        if (text[i] == ' ') {
            xPos += space;
            continue;
        }
        
        int ci = getCharIndexFromChar(text[i]);
        for (int pi = 0; pi < charWidth * charHeight; pi++) {
            int px = xPos + (pi % charWidth);
            int py = y + (pi / charWidth);
            bool on = charset->chars[ci].pixels[pi];
            if (inverted) on = !on;
            drawPixel(px, py, on);
        }

        xPos += charWidth + sep;
    }
}