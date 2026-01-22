//******************************************************************************
//  Copyright (c) 2013, 2014 Tweak Software.
//  All rights reserved.
//
//  SPDX-License-Identifier: Apache-2.0
//
//******************************************************************************
#ifndef __audio__QTAudioRenderer__h__
#define __audio__QTAudioRenderer__h__

#include <IPCore/Session.h>
#include <IPCore/AudioRenderer.h>
#include <IPCore/Application.h>
#include <TwkMath/Time.h>
#include <TwkUtil/Clock.h>

#include <QtCore/QIODevice>

#ifdef QT_MULTIMEDIA_AVAILABLE
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDevice>
#include <QtMultimedia/QAudioSink>
#include <QtMultimedia/QMediaDevices>
#else
// Forward declarations when Multimedia is not available
class QAudioFormat;
class QAudioDevice;
class QAudioSink;
class QMediaDevices;
#endif

#include <QtCore/QThread>
#include <QtCore/QMutex>

#include <iostream>

namespace IPCore
{

#ifdef QT_MULTIMEDIA_AVAILABLE
    using SampleFormat = QAudioFormat::SampleFormat;
#else
    // Stub type when Multimedia is not available
    enum SampleFormat
    {
    };
#endif

    class QTAudioThread;
    class QTAudioRenderer;

    class QTAudioIODevice : public QIODevice
    {
        Q_OBJECT

    public:
        QTAudioIODevice(QTAudioThread& audioThread);
        virtual ~QTAudioIODevice();

        virtual qint64 readData(char* data, qint64 maxlen);
        virtual qint64 writeData(const char* data, qint64 len);
        qint64 bytesAvailable() const;

        void start();

        void startProducingSilence() { m_silence = true; }

        void stopProducingSilence() { m_silence = false; }

    public slots:
        void resetDevice();
        void stopDevice();

    private:
        QTAudioThread& m_thread;
        bool m_silence = false;
    };

#ifdef QT_MULTIMEDIA_AVAILABLE
    class QTAudioOutput : public QAudioSink
#else
    class QTAudioOutput : public QObject
#endif
    {
        Q_OBJECT

    public:
#ifdef QT_MULTIMEDIA_AVAILABLE
        QTAudioOutput(QAudioDevice& audioDevice, QAudioFormat& audioFormat, QTAudioIODevice& ioDevice, QTAudioThread& audioThread);
#else
        QTAudioOutput(void*, void*, QTAudioIODevice&, QTAudioThread&);
#endif
        ~QTAudioOutput();

        bool isFlushing() const { return m_isFlushing; }

        void startFlushing();

        void doneFlushing() { m_isFlushing = false; }

        void accumulateFlushedBytes(qint64 n) { m_flushedBytes += n; }

#ifdef QT_MULTIMEDIA_AVAILABLE
        bool flushedEnough() const { return m_flushedBytes >= bufferSize(); }
#else
        bool flushedEnough() const { return false; }
#endif

#ifdef QT_MULTIMEDIA_AVAILABLE
        // Methods inherited from QAudioSink
        using QAudioSink::bufferSize;
        using QAudioSink::processedUSecs;
        using QAudioSink::reset;
        using QAudioSink::resume;
        using QAudioSink::setBufferSize;
        using QAudioSink::start;
        using QAudioSink::state;
        using QAudioSink::stop;
        using QAudioSink::suspend;
#else
        // Stub methods when Multimedia is not available
        int state() const { return 0; }

        qint64 bufferSize() const { return 0; }

        void setBufferSize(qint64) {}

        void start(QIODevice*) {}

        void stop() {}

        void suspend() {}

        void reset() {}

        void resume() {}

        qint64 processedUSecs() const { return 0; }
#endif

    public slots:
        void startAudio();
        void resetAudio();
        void stopAudio();
        void suspendAudio();
        void suspendAndResetAudio();
        void setAudioOutputBufferSize();
        void play(IPCore::Session* s);

    private:
        int calcAudioBufferSize(const int channels, const int sampleRate, const int sampleSizeInBytes, const int defaultBufferSize) const;
#ifdef QT_MULTIMEDIA_AVAILABLE
        std::string toString(QAudio::State state);

    private:
        QAudioDevice& m_device;
        QAudioFormat& m_format;
#else
        std::string toString(int state);

    private:
        void* m_device;
        void* m_format;
#endif
        QTAudioIODevice& m_ioDevice;
        QTAudioThread& m_thread;
        bool m_isFlushing = false;
        qint64 m_flushedBytes;
    };

    class QTAudioThread : public QThread
    {
        Q_OBJECT

    public:
        QTAudioThread(QTAudioRenderer& audioRenderer, QObject* parent = 0);
        ~QTAudioThread();

        void startMe();

        size_t processedSamples() const;
        void setProcessedSamples(size_t n);

        size_t startSample() const;
        void setStartSample(size_t value);

        TwkAudio::Time actualPlayedTimeOffset() const;
        void setActualPlayedTimeOffset(TwkAudio::Time t);

        void setDeviceLatency(double t);

        bool preRollDisable() const;
        void setPreRollDisable(bool disable);

        void setPreRollDelay(TwkAudio::Time t);

        int bytesPerSample() const;
        size_t framesPerBuffer() const;

        bool holdOpen() const;

        const AudioRenderer::DeviceState& deviceState() const;
        void setDeviceState(AudioRenderer::DeviceState& state);

        void emitStartAudio();
        void emitResetAudio();
        void emitStopAudio();
        void emitSuspendAudio();
        void emitSuspendAndResetAudio();

        void emitResetDevice();
        void emitStopDevice();

        void emitPlay(IPCore::Session* s);

        //
        //  callback for QTAudioIODevice's readData() to call
        //
        qint64 qIODeviceCallback(char* data, qint64 maxLenInBytes);

        void startOfInitialization();
        TwkUtil::SystemClock::Time endOfInitialization();
        bool isInInitialization() const;

        TwkUtil::SystemClock::Time getLastDeviceLatency() const { return m_lastDeviceLatency; }

        QTAudioOutput* audioOutput() const { return m_audioOutput; }

    protected:
        virtual void run();

    private:
        bool createAudioOutput();

        void detachAudioOutputDevice();

    private:
        QMutex m_mutex;

        QObject* m_parent;

        QTAudioIODevice* m_ioDevice;
        QTAudioOutput* m_audioOutput;
        TwkAudio::AudioBuffer m_abuffer;
        QTAudioRenderer& m_audioRenderer;

        int m_bytesPerSample;
        size_t m_preRollSamples;
        size_t m_processedSamples;
        size_t m_startSample;
        int m_preRollMode;
        bool m_preRollDisable;

        bool m_patch9355Enabled;
        TwkUtil::SystemClock m_systemClock;
        TwkUtil::SystemClock::Time m_startOfInitialization;
        TwkUtil::SystemClock::Time m_endOfInitialization;
        TwkUtil::SystemClock::Time m_lastDeviceLatency;
    };

    class QTAudioRenderer : public IPCore::AudioRenderer
    {
        friend class QTAudioThread; // Allow QTAudioThread to access private members
    public:
        typedef TwkAudio::AudioBuffer AudioBuffer;

        QTAudioRenderer(const RendererParameters&, QObject* parent);
        virtual ~QTAudioRenderer();

        //
        //  AudioRenderer API
        //

        virtual void availableLayouts(const Device&, LayoutsVector&);
        virtual void availableFormats(const Device&, FormatVector&);
        virtual void availableRates(const Device&, Format, RateVector&);

        //
        //  play() will return almost immediately -- a worker thread will
        //  be released and start playing.
        //

        virtual void play();
        virtual void play(IPCore::Session*);

        //
        //  stop() will cause the worker thread to wait until play.
        //

        virtual void stop();
        virtual void stop(IPCore::Session*);

        //
        //  shutdown() close all hardware devices
        //

        virtual void shutdown();

        //
        // T is the Application type that is adding this
        // renderer as an audio module e.g. 'RvApplication'
        // or 'NoodleApplication'.
        template <class T> static IPCore::AudioRenderer::Module addQTAudioModule();

#ifdef QT_MULTIMEDIA_AVAILABLE
        TwkAudio::Format getTwkAudioFormat() const;
        TwkAudio::Format convertToTwkAudioFormat(SampleFormat fmtType) const;

        QAudioFormat::SampleFormat convertToQtAudioFormat(TwkAudio::Format fmtType) const;

        void setSampleSizeAndType(Layout twkLayout, Format twkFormat, QAudioFormat& qformat) const;

        friend class QTAudioThread;

    private:
        bool supportsRequiredChannels(const QAudioDevice& info) const;

        void init();

        void initDeviceList();

    private:
        QObject* m_parent;
        QTAudioThread* m_thread;
        QAudioFormat m_format;
        QAudioDevice m_device;
        QList<QAudioDevice> m_deviceList;
#else
        // Stub implementations when Multimedia is not available
        TwkAudio::Format getTwkAudioFormat() const;
        TwkAudio::Format convertToTwkAudioFormat(SampleFormat) const;

        void setSampleSizeAndType(Layout, Format, void*) const {}

        bool supportsRequiredChannels(const void*) const { return false; }

        void init();
        void initDeviceList();

    private:
        QObject* m_parent;
        QTAudioThread* m_thread;
        void* m_format;
        void* m_device;
        QList<void*> m_deviceList;
#endif
        std::string m_codec;
    };

    template <class T> static IPCore::AudioRenderer* createQTAudio(const IPCore::AudioRenderer::RendererParameters& p)
    {
        return (new QTAudioRenderer(p, static_cast<T*>(IPCore::App())));
    }

    template <class T> IPCore::AudioRenderer::Module QTAudioRenderer::addQTAudioModule()
    {
        return IPCore::AudioRenderer::Module("Platform Audio", "", createQTAudio<T>);
    }

} // namespace IPCore

#endif // __audio__QTAudioRenderer__h__
