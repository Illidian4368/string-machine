#pragma once
#include "StringOsc.h"
#include "StringFilters.h"
#include "SolinaChorus.h"
#include "dsp/ADSREnvelope.h"
#include "dsp/TriangleLFO.h"
#include <boost/intrusive/list.hpp>
#include <memory>
#include <cstdint>

class StringSynth {
public:
    StringSynth();
    ~StringSynth();

    void init(double sampleRate);

    void handleMessage(const uint8_t *msg);
    void resetAllControllers();
    void generate(float *outputs[2], unsigned count);

    float getDetune() const { return fDetuneAmount; }
    void setDetune(float amount) { fDetuneAmount = amount; }

    const StringOsc::Settings &getOscSettings() const { return fOscSettings; }
    StringOsc::Settings &getOscSettings() { return fOscSettings; }

    const ADSREnvelope::Settings &getEnvSettings() const { return fEnvSettings; }
    ADSREnvelope::Settings &getEnvSettings() { return fEnvSettings; }

    const StringFilters::Settings &getFltSettings() const { return fFltSettings; }
    StringFilters::Settings &getFltSettings() { return fFltSettings; }

    float getMixGainUpper() const { return fMixGainUpper; }
    void setMixGainUpper(float value) { fMixGainUpper = value; }
    float getMixGainLower() const { return fMixGainLower; }
    void setMixGainLower(float value) { fMixGainLower = value; }

    const SolinaChorus &getChorus() const { return fChorus; }
    SolinaChorus &getChorus() { return fChorus; }

    float getMasterGain() const { return fMasterGain; }
    void setMasterGain(float value) { fMasterGain = value; }

    float getLastDetuneUpper() const { return fLastDetuneUpper; }
    float getLastDetuneLower() const { return fLastDetuneLower; }

private:
    struct Voice : boost::intrusive::list_base_hook<> {
        unsigned note;
        float pitch;
        float bend;
        bool active;
        ADSREnvelope env;
        StringOsc osc;
        StringFilters flt;
    };

private:
    std::unique_ptr<Voice[]> fVoices;
    boost::intrusive::list<Voice> fActiveVoices;

    float fDetuneAmount;
    TriangleLFO fDetuneLFO[2];

    float fLastDetuneUpper;
    float fLastDetuneLower;

    float fMixGainUpper;
    float fMixGainLower;

    StringOsc::Settings fOscSettings;
    ADSREnvelope::Settings fEnvSettings;
    StringFilters::Settings fFltSettings;

    SolinaChorus fChorus;

    float fMasterGain;

    float fCtlPitchBend;
    float fCtlPitchBendSensitivity;

    struct RpnIdentifier {
        unsigned registered : 1;
        unsigned msb : 7;
        unsigned lsb : 7;
    };
    RpnIdentifier fCtlRpnIdentifier;

private:
    void noteOn(unsigned note, unsigned vel);
    void noteOff(unsigned note, unsigned vel);
    void allNotesOff();
    void allSoundOff();
    bool generateVoiceAdding(Voice &voice, float *output, const float *const detune[2], float bend, unsigned count);
    static void clearFinishedVoice(Voice &voice);
    static bool voiceHasReleased(const Voice &voice);
    static bool voiceHasFinished(const Voice &voice);
};
