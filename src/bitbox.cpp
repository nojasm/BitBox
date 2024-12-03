#include "bitbox.hpp"
#include "ui.hpp"
#include "device.hpp"

// Effects
#include "test.hpp"
#include "test2.hpp"

Track::Track() {
    Effect* fx1 = new BitBoxEffect::TestEffect();
    this->effects.push_back(fx1);
}

Project::Project() {
    this->length = 0;
}

AudioData::AudioData() {

}

Sample::Sample() {
    this->data = new AudioData();
}

Effect::Effect(string name) : name(name) {
    
}

BitBox::BitBox() {
    this->inputs = (InputEventValue*) malloc(sizeof(InputEventValue) * (int)InputEventType::N_INPUTS);
    for (int i = 0; i < (int)InputEventType::N_INPUTS; i++) {
        this->inputs[i] = InputEventValue::NONE;
    }

    ui = new UIManager();

    this->project = device->loadProject("/");
}

vector<string> BitBox::getEffectNames() {
    return {"test0", "test0v2"};
}

Effect* BitBox::newEffect(string name) {
    if (name == "test0") {
        return new BitBoxEffect::TestEffect();
    } else if (name == "test0v2") {
        return new BitBoxEffect::Test2Effect();
    } else {
        return nullptr;
    }
}

bool BitBox::inputIsPressed(InputEventType type) {
    return (
        this->inputs[(int)type] == InputEventValue::PRESSED ||
        this->inputs[(int)type] == InputEventValue::JUST_PRESSED
    );
}

bool BitBox::inputIsJustPressed(InputEventType type) {
    return (
        this->inputs[(int)type] == InputEventValue::JUST_PRESSED
    );
}

void BitBox::clearScreen() {
    for (int i = 0; i < device->screenWidth * device->screenHeight; i++)
        this->device->pixels[i] = false;
}

void BitBox::setPixel(int x, int y, bool on) {
    this->device->pixels[y * device->screenWidth + x] = on;
}

void BitBox::events() {
    // Set all "just_pressed" events to "pressed" events
    for (int i = 0; i < (int)InputEventType::N_INPUTS; i++) {
        if (this->inputs[i] == InputEventValue::JUST_PRESSED) {
            this->inputs[i] = InputEventValue::PRESSED;
        } else if (this->inputs[i] == InputEventValue::JUST_RELEASED) {
            this->inputs[i] = InputEventValue::NONE;
        }
    }

    // SDL Emulator events to InputEvent converter
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            this->isRunning = false;
        } else if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
            InputEventValue value = event.type == SDL_KEYDOWN ? InputEventValue::JUST_PRESSED : InputEventValue::JUST_RELEASED;
            if (event.key.keysym.sym == SDL_KeyCode::SDLK_RETURN) {
                this->inputs[(int)InputEventType::SET] = value;
            } else if (event.key.keysym.sym == SDL_KeyCode::SDLK_BACKSPACE) {
                this->inputs[(int)InputEventType::BACK] = value;
            } else if (event.key.keysym.sym == SDL_KeyCode::SDLK_SPACE) {
                this->inputs[(int)InputEventType::PLAY] = value;
            } else if (event.key.keysym.sym == SDL_KeyCode::SDLK_PAGEDOWN) {
                this->inputs[(int)InputEventType::ROTARY_RIGHT] = value;
            } else if (event.key.keysym.sym == SDL_KeyCode::SDLK_PAGEUP) {
                this->inputs[(int)InputEventType::ROTARY_LEFT] = value;
            } else if (event.key.keysym.sym == SDL_KeyCode::SDLK_r) {
                this->inputs[(int)InputEventType::ROTARY] = value;
            }
        }
    }
}

void BitBox::loop() {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    double s = std::chrono::duration_cast<std::chrono::microseconds>(now - lastRenderTime).count() / 1000000.0;
    double fps = 1.0 / s;

    this->clearScreen();

    Track* track;
    if (this->project->tracks.size() == 0) track = nullptr;
    else track = this->project->tracks[this->ui->trackOpenedTrackIndex];

    if (this->ui->rowsStack.size() > 0) {
        // Always display last Rows in stack
        Rows* rows = this->ui->rowsStack[this->ui->rowsStack.size() - 1];
        for (int i = 0; i < rows->options.size(); i++) {
            int rel = i - rows->scroll;
            if (rel >= 0 && rel <= 4) {
                this->ui->drawText(1, 11 + rel * 11, rows->options[i].key, rows->current == i);
            }
        }

        if (this->inputIsJustPressed(InputEventType::SET)) {
            rows->rowIsSelected = !rows->rowIsSelected;
        } else if (this->inputIsJustPressed(InputEventType::ROTARY_LEFT)) {
            // TODO: ADD Value change if row is selected
            if ((rows->current - 1) < rows->scroll && rows->scroll > 0)
                rows->scroll--;
            
            rows->current = SDL_clamp(rows->current - 1, 0, rows->options.size() - 1);
        } else if (this->inputIsJustPressed(InputEventType::ROTARY_RIGHT)) {
            if (((rows->current + 1) - rows->scroll) >= 4)
                rows->scroll++;
            
            rows->current = SDL_clamp(rows->current + 1, 0, rows->options.size() - 1);
        }
        // TODO: Add BACK here and pop from stack
        // TODO: Add LINK element to row entry types
    } else if (this->ui->dropdownOverlay.isOpened) {
        this->ui->drawText(1, 1, this->ui->dropdownOverlay.title);
        for (int i = 0; i < this->ui->dropdownOverlay.options.size(); i++) {
            string text = this->ui->dropdownOverlay.options[i];
            this->ui->drawText(25, 11 + i * 11, text, i == this->ui->dropdownOverlay.row);
        }

        if (this->inputIsJustPressed(InputEventType::ROTARY_LEFT))
            this->ui->dropdownOverlay.row = SDL_clamp(this->ui->dropdownOverlay.row - 1, 0, this->ui->dropdownOverlay.options.size() - 1);
        else if (this->inputIsJustPressed(InputEventType::ROTARY_RIGHT))
            this->ui->dropdownOverlay.row = SDL_clamp(this->ui->dropdownOverlay.row + 1, 0, this->ui->dropdownOverlay.options.size() - 1);
        else if (this->inputIsJustPressed(InputEventType::SET)) {
            this->ui->dropdownOverlay.isOpened = false;
            this->ui->dropdownOverlay.justClosed = true;
        } else if (this->inputIsJustPressed(InputEventType::BACK)) {
            this->ui->dropdownOverlay.isOpened = false;
            this->ui->dropdownOverlay.row = this->ui->dropdownOverlay.lastIndex;
        }
            
    } else if (this->ui->view == UIView::PROJECT) {
        this->ui->title = "PROJECT";
        this->ui->drawHorizontal(9);
        this->ui->drawHorizontal(54);
        for (int i = 0; i < 4; i++) {
            bool highlightSamp = this->ui->projectTrackRowIsSelected && this->ui->projectTrackRow == i && this->ui->projectTrackCol == 0;
            bool highlightConf = this->ui->projectTrackRowIsSelected && this->ui->projectTrackRow == i && this->ui->projectTrackCol == 1;
            bool highlightFx = this->ui->projectTrackRowIsSelected && this->ui->projectTrackRow == i && this->ui->projectTrackCol == 2;

            this->ui->drawText(1, 11 + 11 * i, "TRACK " + std::to_string(i + 1), this->ui->projectTrackRow == i);
            this->ui->drawText(55, 11 + 11 * i, "SAMP", highlightSamp);
            this->ui->drawText(86, 11 + 11 * i, "CONF", highlightConf);
            this->ui->drawText(116, 11 + 11 * i, "FX", highlightFx);
        }

        if (this->ui->projectTrackRowIsSelected) {
            // Scroll horizontally
            if (this->inputIsJustPressed(InputEventType::ROTARY_LEFT))
                this->ui->projectTrackCol = SDL_clamp(this->ui->projectTrackCol - 1, 0, 2);
            if (this->inputIsJustPressed(InputEventType::ROTARY_RIGHT))
                this->ui->projectTrackCol = SDL_clamp(this->ui->projectTrackCol + 1, 0, 2);
            if (this->inputIsJustPressed(InputEventType::BACK))
                this->ui->projectTrackRowIsSelected = false;

            if (this->inputIsJustPressed(InputEventType::SET)) {
                this->ui->trackOpenedTrackIndex = this->ui->projectTrackRow;
                if (this->ui->projectTrackCol == 0)
                    this->ui->view = UIView::TRACK_SAMP;
                else if (this->ui->projectTrackCol == 1)
                    this->ui->view = UIView::TRACK_CONF;
                else if (this->ui->projectTrackCol == 2)
                    this->ui->view = UIView::TRACK_FX;
            }
        } else {
            // Scroll vertically
            if (this->inputIsJustPressed(InputEventType::ROTARY_LEFT))
                this->ui->projectTrackRow = SDL_clamp(this->ui->projectTrackRow - 1, 0, 3);
            if (this->inputIsJustPressed(InputEventType::ROTARY_RIGHT))
                this->ui->projectTrackRow = SDL_clamp(this->ui->projectTrackRow + 1, 0, 3);
            if (this->inputIsPressed(InputEventType::SET))
                this->ui->projectTrackRowIsSelected = true;
        }
    } else if (this->ui->view == UIView::TRACK_SAMP) {
        this->ui->title = "SAMP";
        if (this->inputIsJustPressed(InputEventType::BACK)) this->ui->view = UIView::PROJECT;
    
    } else if (this->ui->view == UIView::TRACK_CONF) {
        this->ui->title = "CONF";
        this->ui->drawHorizontal(9);
        if (this->inputIsJustPressed(InputEventType::BACK)) this->ui->view = UIView::PROJECT;

        this->ui->drawText(1, 30, "MODE", false);
        vector<string> modeOptions = {"NONE", "SOLO", "MUTE"};
        this->ui->drawText(60, 30, modeOptions[(int)track->trackMode], false);

        if (this->inputIsJustPressed(InputEventType::SET)) {
            if (this->ui->trackConfRow == 0) {
                this->ui->openDropdown("MODE", modeOptions, (int)track->trackMode);
            }
        }

        if (this->ui->trackConfRow && this->ui->dropdownOverlay.justClosed) {
            track->trackMode = (TrackMode)this->ui->dropdownOverlay.row;
        }



        /*
        for (int i = 0; i < 6; i++) {
            if (i == 0) {
                if (i == this->ui->trackConfRow)
                    this->ui->drawBox(0, 10, 128, 11, true);

                this->ui->drawText(1, 11, "MODE");
            }
        }
        */
    
    } else if (this->ui->view == UIView::TRACK_FX) {
        this->ui->title = "FX";
        if (this->inputIsJustPressed(InputEventType::BACK)) this->ui->view = UIView::PROJECT;
        
        // Info text if no effects are there yet
        if (track->effects.size() == 0) {
            this->ui->drawText(25, 25, "PRESS IN");
            this->ui->drawText(25, 36, "TO ADD FX");
        } else {
            Effect* currentEffect = track->effects[this->ui->trackEffectsEffectIndex];
            
            // x / y in top right corner
            string indexText = std::to_string(this->ui->trackEffectsEffectIndex + 1) + "/" + std::to_string(track->effects.size());
            this->ui->drawText(100, 1, indexText);

            // Effect name in center of screen
            int textWidth = this->ui->calculateTextWidth(currentEffect->name, 5, 1, 3);
            this->ui->drawText(128 / 2 - textWidth / 2, 30, currentEffect->name);
        }

        if (this->inputIsJustPressed(InputEventType::ROTARY)) {
            Rows* fxSettings = new Rows();

            RowEntry re1;
            re1.key = "INSERT FX";
            re1.type = RowEntryType::BUTTON;

            RowEntry re2;
            re2.key = "CLEAR FX";
            re2.type = RowEntryType::BUTTON;

            RowEntry test;
            test.key = "TEST";
            test.type = RowEntryType::VALUE;
            test.getValueFunc = [&](){
                return this->ui->testValue;
            };
            test.handleChangeFunc = [&](double val){
                val = SDL_clamp(val, 0, 1.0);
                this->ui->testValue = val;
                return val;
            };

            fxSettings->options.push_back(re1);
            fxSettings->options.push_back(re2);
            this->ui->rowsStack.push_back(fxSettings);

        } else if (this->inputIsJustPressed(InputEventType::SET)) {
            this->ui->view = UIView::TRACK_FX_EFFECT;
        } else if (this->inputIsJustPressed(InputEventType::ROTARY_LEFT)) {
            int nEffects = this->project->tracks[this->ui->trackOpenedTrackIndex]->effects.size();
            this->ui->trackEffectsEffectIndex = SDL_clamp(this->ui->trackEffectsEffectIndex - 1, 0, nEffects);
        } else if (this->inputIsJustPressed(InputEventType::ROTARY_RIGHT)) {
            int nEffects = this->project->tracks[this->ui->trackOpenedTrackIndex]->effects.size();
            this->ui->trackEffectsEffectIndex = SDL_clamp(this->ui->trackEffectsEffectIndex + 1, 0, nEffects);
        }
    
    } else if (this->ui->view == UIView::TRACK_FX_SETTINGS) {
        printf("wtf");
    
    } else if (this->ui->view == UIView::TRACK_FX_EFFECT) {
        Effect* effect = track->effects[this->ui->trackEffectsEffectIndex];
        this->ui->title = "FX " + effect->name;
    }

    if (this->ui->title != "" && !this->ui->dropdownOverlay.isOpened) {
        this->ui->drawText(1, 0, this->ui->title);
    }

    this->device->updateScreen();
    
    //printf("FPS: %d\n", (int)fps);

    lastRenderTime = now;
}



#undef main
int main() {
    BitBox* bb = new BitBox();
    bb->device = new Device();
    bb->ui->bb = bb;

    while (bb->isRunning) {
        bb->events();
        bb->loop();
    }

    return 0;
}