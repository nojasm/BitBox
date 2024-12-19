#include "bitbox.hpp"

namespace BitBoxEffect {
    class TestEffect : public Effect {
    public:
        int fc = 0;

        TestEffect() : Effect("LFO") {
            Parameter wet;
            wet.id = "wet";
            wet.name = "WET";
            wet.value = 1.0;
            wet.type = ParameterType::PROCENT;
            this->parameters.push_back(wet);

            Parameter speed;
            speed.id = "speed";
            speed.name = "SPEED";
            speed.value = 500.0;
            speed.min = 1.0;
            speed.max = 2000.0;
            speed.type = ParameterType::MS;
            this->parameters.push_back(speed);
        }

        void process(double** in, double** out, int size) {
            double sr = this->sampleRate;
            for (int i = 0; i < size; i++) {
                double relTrackPos = i / (double)size;
                double wet = getParameterValue(0, relTrackPos);
                double speed = getParameterValue(1, relTrackPos);


                double wL = in[0][i] * 0.5 * (sin((1000.0 / speed) * 2 * M_PI * (fc++) / sr) + 1);
                double wR = in[1][i] * 0.5 * (sin((1000.0 / speed) * 2 * M_PI * (fc++) / sr) + 1);

                out[0][i] = in[0][i] * (1 - wet) + wL * wet;
                out[1][i] = in[1][i] * (1 - wet) + wR * wet;
            }
        }
    };
}
