#pragma once

#include <cstdint>
#include "AudioBufferProvider.h"
#include <cassert>
#include "audio.h"

namespace cocos2d { namespace experimental {

class AudioResampler {
public:
    // Determines quality of SRC.
    //  LOW_QUALITY: linear interpolator (1st order)
    //  MED_QUALITY: cubic interpolator (3rd order)
    //  HIGH_QUALITY: fixed multi-tap FIR (e.g. 48KHz->44.1KHz)
    // NOTE: high quality SRC will only be supported for
    // certain fixed rate conversions. Sample rate cannot be
    // changed dynamically.
    enum src_quality { // NOLINT(readability-identifier-naming)
        DEFAULT_QUALITY = 0,
        LOW_QUALITY = 1,
        MED_QUALITY = 2,
        HIGH_QUALITY = 3,
        VERY_HIGH_QUALITY = 4,
    };

    static const CONSTEXPR float UNITY_GAIN_FLOAT = 1.0F;

    static AudioResampler *create(audio_format_t format, int inChannelCount,
                                  int32_t sampleRate, src_quality quality = DEFAULT_QUALITY);

    virtual ~AudioResampler();

    virtual void init() = 0;
    virtual void setSampleRate(int32_t inSampleRate);
    virtual void setVolume(float left, float right);
    virtual void setLocalTimeFreq(uint64_t freq);

    // set the PTS of the next buffer output by the resampler
    virtual void setPTS(int64_t pts);

    // Resample int16_t samples from provider and accumulate into 'out'.
    // A mono provider delivers a sequence of samples.
    // A stereo provider delivers a sequence of interleaved pairs of samples.
    //
    // In either case, 'out' holds interleaved pairs of fixed-point Q4.27.
    // That is, for a mono provider, there is an implicit up-channeling.
    // Since this method accumulates, the caller is responsible for clearing 'out' initially.
    //
    // For a float resampler, 'out' holds interleaved pairs of float samples.
    //
    // Multichannel interleaved frames for n > 2 is supported for quality DYN_LOW_QUALITY,
    // DYN_MED_QUALITY, and DYN_HIGH_QUALITY.
    //
    // Returns the number of frames resampled into the out buffer.
    virtual size_t resample(int32_t *out, size_t outFrameCount,
                            AudioBufferProvider *provider) = 0;

    virtual void reset();
    virtual size_t getUnreleasedFrames() const { return mInputIndex; }

    // called from destructor, so must not be virtual
    src_quality getQuality() const { return mQuality; }

protected:
    // number of bits for phase fraction - 30 bits allows nearly 2x downsampling
    static const int kNumPhaseBits = 30; // NOLINT(readability-identifier-naming)

    // phase mask for fraction
    static const uint32_t kPhaseMask = (1LU << kNumPhaseBits) - 1; // NOLINT(readability-identifier-naming)

    // multiplier to calculate fixed point phase increment
    static const double kPhaseMultiplier; // NOLINT(readability-identifier-naming)

    AudioResampler(int inChannelCount, int32_t sampleRate, src_quality quality);

    // prevent copying
    AudioResampler(const AudioResampler &);
    AudioResampler &operator=(const AudioResampler &);

    int64_t calculateOutputPTS(int outputFrameIndex);


    const int32_t mChannelCount;// NOLINT(readability-identifier-naming)
    const int32_t mSampleRate;// NOLINT(readability-identifier-naming)
    int32_t mInSampleRate;// NOLINT(readability-identifier-naming)
    AudioBufferProvider::Buffer mBuffer;// NOLINT(readability-identifier-naming)
    union {
        int16_t mVolume[2];// NOLINT(readability-identifier-naming)
        uint32_t mVolumeRL;// NOLINT(readability-identifier-naming)
    };
    int16_t mTargetVolume[2];// NOLINT(readability-identifier-naming)
    size_t mInputIndex;// NOLINT(readability-identifier-naming)
    int32_t mPhaseIncrement;// NOLINT(readability-identifier-naming)
    uint32_t mPhaseFraction;// NOLINT(readability-identifier-naming)
    uint64_t mLocalTimeFreq;// NOLINT(readability-identifier-naming)
    int64_t mPTS;// NOLINT(readability-identifier-naming)

    // returns the inFrameCount required to generate outFrameCount frames.
    //
    // Placed here to be a consistent for all resamplers.
    //
    // Right now, we use the upper bound without regards to the current state of the
    // input buffer using integer arithmetic, as follows:
    //
    // (static_cast<uint64_t>(outFrameCount)*mInSampleRate + (mSampleRate - 1))/mSampleRate;
    //
    // The double precision equivalent (float may not be precise enough):
    // ceil(static_cast<double>(outFrameCount) * mInSampleRate / mSampleRate);
    //
    // this relies on the fact that the mPhaseIncrement is rounded down from
    // #phases * mInSampleRate/mSampleRate and the fact that Sum(Floor(x)) <= Floor(Sum(x)).
    // http://www.proofwiki.org/wiki/Sum_of_Floors_Not_Greater_Than_Floor_of_Sums
    //
    // (so long as double precision is computed accurately enough to be considered
    // greater than or equal to the Floor(x) value in int32_t arithmetic; thus this
    // will not necessarily hold for floats).
    //
    // REFINE:
    // Greater accuracy and a tight bound is obtained by:
    // 1) subtract and adjust for the current state of the AudioBufferProvider buffer.
    // 2) using the exact integer formula where (ignoring 64b casting)
    //  inFrameCount = (mPhaseIncrement * (outFrameCount - 1) + mPhaseFraction) / phaseWrapLimit;
    //  phaseWrapLimit is the wraparound (1 << kNumPhaseBits), if not specified explicitly.
    //
    inline size_t getInFrameCountRequired(size_t outFrameCount) const {
        return (static_cast<size_t>(outFrameCount) * mInSampleRate + (mSampleRate - 1)) / mSampleRate;
    }

    inline float clampFloatVol(float volume) {//NOLINT(readability-identifier-naming, readability-convert-member-functions-to-static)
        float ret = 0.0F;
        if (volume > UNITY_GAIN_FLOAT) {
            ret = UNITY_GAIN_FLOAT;
        } else if (volume >= 0.) {
            ret = volume;
        }
        return ret; // NaN or negative volume maps to 0.
    }

private:
    const src_quality mQuality;// NOLINT(readability-identifier-naming)

    // Return 'true' if the quality level is supported without explicit request
    static bool qualityIsSupported(src_quality quality);

    // For pthread_once()
    static void init_routine(); // NOLINT(readability-identifier-naming)

    // Return the estimated CPU load for specific resampler in MHz.
    // The absolute number is irrelevant, it's the relative values that matter.
    static uint32_t qualityMHz(src_quality quality);
};
// ----------------------------------------------------------------------------
}} // namespace cocos2d { namespace experimental
