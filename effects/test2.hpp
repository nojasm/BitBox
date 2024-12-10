#include "bitbox.hpp"

namespace BitBoxEffect {
    class Test2Effect : public Effect {
    public:
        Test2Effect() : Effect("Test0v2") {
            Parameter vol;
            vol.id = "volx";
            vol.name = "VOLX";
            vol.type = ParameterType::PROCENT;
            vol.value = 1.0;
            vol.defaultValue = 1.0;
            this->parameters.push_back(vol);

            Parameter spread;
            spread.type = ParameterType::MS;
            spread.id = "spread";
            spread.name = "SPREAD";
            spread.value = 500;
            spread.defaultValue = 500;
            spread.min = 0;
            spread.max = 1000;
            this->parameters.push_back(spread);
        }

        void process(double** in, double** out, int size) {
            for (int i = 0; i < size; i++) {
                out[0][i] = in[0][i];
                out[1][i] = in[1][i];
            }
        }
    };
}
