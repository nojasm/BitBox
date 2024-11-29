#include "bitbox.hpp"
#include "ui.hpp"
#include "device.hpp"

Track::Track() {

}

Project::Project() {
    this->length = 0;
}

AudioData::AudioData() {

}

Sample::Sample() {
    this->data = new AudioData();
}

BitBox::BitBox() {
    this->inputs = (InputEventValue*) malloc(sizeof(InputEventValue) * (int)InputEventType::N_INPUTS);
    for (int i = 0; i < (int)InputEventType::N_INPUTS; i++) {
        this->inputs[i] = InputEventValue::NONE;
    }

    ui = new UIManager();

    this->project = device->loadProject("/");
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
            }
        }
    }
}

void BitBox::loop() {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    double s = std::chrono::duration_cast<std::chrono::microseconds>(now - lastRenderTime).count() / 1000000.0;
    double fps = 1.0 / s;

    this->clearScreen();

    printf("%d\n", this->inputs[(int)InputEventType::BACK]);

    if (this->ui->view == UIView::PROJECT) {
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
        if (this->inputIsJustPressed(InputEventType::BACK)) this->ui->view = UIView::PROJECT;
    
    } else if (this->ui->view == UIView::TRACK_FX) {
        this->ui->title = "FX";
        if (this->inputIsJustPressed(InputEventType::BACK)) this->ui->view = UIView::PROJECT;
    }

    if (this->ui->title != "") {
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