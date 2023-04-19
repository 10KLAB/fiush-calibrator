#ifndef SCALE_H_
#define SCALE_H_


namespace _10klab{
    namespace scale{
        void SetUpScale();
        void Tare();
        float GetRaw(int samples);
        float GetUnits(int samples);
        void UpdateCoefficients();
        float StableMeasure(bool enable_negatives);
        float StableMeasure2(int pulses, float input_threshold);

    }
}


#endif