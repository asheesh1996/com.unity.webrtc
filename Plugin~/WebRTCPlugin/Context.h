#pragma once
#include "DummyAudioDevice.h"
#include "PeerConnectionObject.h"
#include "NvVideoCapturer.h"
#include "Codec/IEncoder.h"

namespace WebRTC
{
    class Context;
    class PeerSDPObserver;
    class IGraphicsDevice;
    class ContextManager
    {
    public:
        static ContextManager* GetInstance() { return &s_instance; }
     
        Context* GetContext(int uid) const;
        Context* CreateContext(int uid, UnityEncoderType encoderType);
        void DestroyContext(int uid);
        void SetCurContext(Context*);

    public:
        using ContextPtr = std::unique_ptr<Context>;
        Context* curContext = nullptr;
    private:
        ~ContextManager();
        std::map<int, ContextPtr> m_contexts;
        static ContextManager s_instance;
    };

    class Context
    {
    public:
        explicit Context(int uid = -1, UnityEncoderType encoderType = UnityEncoderType::UnityEncoderHardware);
        ~Context();

        CodecInitializationResult GetCodecInitializationResult();
        webrtc::MediaStreamInterface* CreateVideoStream(void* frameBuffer, int width, int height);
        void DeleteVideoStream(webrtc::MediaStreamInterface* stream);
        webrtc::MediaStreamInterface* CreateAudioStream();
        void DeleteAudioStream(webrtc::MediaStreamInterface* stream);
        PeerConnectionObject* CreatePeerConnection();
        PeerConnectionObject* CreatePeerConnection(const std::string& conf);
        void DeletePeerConnection(PeerConnectionObject* obj) { clients.erase(obj); }
        UnityEncoderType GetEncoderType() const;

        // You must call these methods on Rendering thread.
        bool InitializeEncoder(IGraphicsDevice* device);
        void EncodeFrame();
        void FinalizeEncoder();
        //

        void StopCapturer() { nvVideoCapturer->Stop(); }
        void ProcessAudioData(const float* data, int32 size);

        DataChannelObject* CreateDataChannel(PeerConnectionObject* obj, const char* label, const RTCDataChannelInit& options);
        void DeleteDataChannel(DataChannelObject* obj);

        std::map<DataChannelObject*, std::unique_ptr<DataChannelObject>> dataChannels;

    private:
        int m_uid;
        UnityEncoderType m_encoderType;
        std::unique_ptr<rtc::Thread> workerThread;
        std::unique_ptr<rtc::Thread> signalingThread;
        std::map<PeerConnectionObject*, rtc::scoped_refptr<PeerConnectionObject>> clients;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
        NvVideoCapturer* nvVideoCapturer;
        std::unique_ptr<NvVideoCapturer> nvVideoCapturerUnique;
        rtc::scoped_refptr<DummyAudioDevice> audioDevice;
        rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack;
        rtc::scoped_refptr<webrtc::MediaStreamInterface> audioStream;
        //TODO: move videoTrack to NvVideoCapturer and maintain multiple NvVideoCapturer here
        std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>> videoStreams;
        std::map<const void*, rtc::scoped_refptr<webrtc::VideoTrackInterface>> videoTracks;
    };

    class PeerSDPObserver : public webrtc::SetSessionDescriptionObserver
    {
    public:
        static PeerSDPObserver* Create(PeerConnectionObject* obj);
        virtual void OnSuccess();
        virtual void OnFailure(const std::string& error);
    protected:
        PeerSDPObserver() {}
        ~PeerSDPObserver() {}
    private:
        PeerConnectionObject* m_obj;
    };  // class PeerSDPObserver

    extern void Convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config);
    extern webrtc::SdpType ConvertSdpType(RTCSdpType type);
    extern RTCSdpType ConvertSdpType(webrtc::SdpType type);
}
