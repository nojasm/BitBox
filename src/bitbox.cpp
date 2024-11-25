#include "bitbox.hpp"
#include "ui.hpp"

BitBox::BitBox() {
    ui = new UIManager();
}

void BitBox::events() {}

void BitBox::loop() {}

int main() {
    BitBox* bb = new BitBox();
}