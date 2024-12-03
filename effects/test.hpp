#include "bitbox.hpp"

namespace BitBoxEffect {
    class TestEffect : public Effect {
    public:
        TestEffect() : Effect("Test0") {
            Parameter vol;
            vol.id = "vol";
            vol.name = "VOL";
            vol.value = 1.0;
            this->parameters.push_back(vol);
        }

        void process(double** in, double** out, int size) {
            for (int i = 0; i < size; i++) {
                out[0][i] = in[0][i];
                out[1][i] = in[1][i];
            }
        }
    };
}
