﻿#pragma once
#include "WebRTCPlugin.h"
#include "DataChannelObject.h"


namespace WebRTC
{
    using DelegateCreateSDSuccess = void(*)(PeerConnectionObject*, RTCSdpType, const char*);
    using DelegateCreateSDFailure = void(*)(PeerConnectionObject*);
    using DelegateSetSDSuccess = void(*)(PeerConnectionObject*);
    using DelegateSetSDFailure = void(*)(PeerConnectionObject*);
    using DelegateLocalSdpReady = void(*)(PeerConnectionObject*, const char*, const char*);
    using DelegateIceCandidate = void(*)(PeerConnectionObject*, const char*, const char*, const int);
    using DelegateOnIceConnectionChange = void(*)(PeerConnectionObject*, webrtc::PeerConnectionInterface::IceConnectionState);
    using DelegateOnDataChannel = void(*)(PeerConnectionObject*, DataChannelObject*);
    using DelegateOnRenegotiationNeeded = void(*)(PeerConnectionObject*);
    using DelegateOnTrack = void(*)(PeerConnectionObject*, const webrtc::RtpTransceiverInterface*);
    using DelegateOnGetStats = void(*)(PeerConnectionObject*, const webrtc::RTCStatsReport*);

    class PeerConnectionObject
        : public webrtc::CreateSessionDescriptionObserver
        , public webrtc::PeerConnectionObserver
        , public webrtc::RTCStatsCollectorCallback
    {
    public:
        PeerConnectionObject(Context& context);
        ~PeerConnectionObject();

        void Close();
        void SetLocalDescription(const RTCSessionDescription& desc);
        void GetLocalDescription(RTCSessionDescription& desc) const;
        void SetRemoteDescription(const RTCSessionDescription& desc);
        webrtc::RTCErrorType SetConfiguration(const std::string& config);
        void GetConfiguration(std::string& config) const;
        void CreateOffer(const RTCOfferOptions& options);
        void CreateAnswer(const RTCAnswerOptions& options);
        void AddIceCandidate(const RTCIceCandidate& candidate);
        void GetStats();
        void GetStats(const webrtc::MediaStreamTrackInterface& selector);
        void RegisterCallbackSetSD(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure)
        {
            onSetSDSuccess = onSuccess;
            onSetSDFailure = onFailure;
        }
        void RegisterCallbackCreateSD(DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure)
        {
            onCreateSDSuccess = onSuccess;
            onCreateSDFailure = onFailure;
        }
        void RegisterLocalSdpReady(DelegateLocalSdpReady callback) { onLocalSdpReady = callback; }
        void RegisterIceCandidate(DelegateIceCandidate callback) { onIceCandidate = callback; }
        void RegisterIceConnectionChange(DelegateOnIceConnectionChange callback) { onIceConnectionChange = callback; };
        void RegisterOnDataChannel(DelegateOnDataChannel callback) { onDataChannel = callback; }
        void RegisterOnRenegotiationNeeded(DelegateOnRenegotiationNeeded callback) { onRenegotiationNeeded = callback; }
        void RegisterOnTrack(DelegateOnTrack callback) { onTrack = callback; }
        void RegisterOnGetStats(DelegateOnGetStats callback) { onGetStats = callback; }

        RTCPeerConnectionState GetConnectionState();
        RTCIceConnectionState GetIceCandidateState();

        //webrtc::CreateSessionDescriptionObserver
        // This callback transfers the ownership of the |desc|.
        void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
        // The OnFailure callback takes an RTCError, which consists of an
        // error code and a string.
        void OnFailure(webrtc::RTCError error) override;
        // webrtc::PeerConnectionObserver
        // Triggered when the SignalingState changed.
        void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
        // Triggered when media is received on a new stream from remote peer.
        void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
        // Triggered when a remote peer closes a stream.
        void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
        // Triggered when a remote peer opens a data channel.
        void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
        // Triggered when renegotiation is needed. For example, an ICE restart
        // has begun.
        void OnRenegotiationNeeded() override;
        // Called any time the IceConnectionState changes.
        void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
        // Called any time the IceGatheringState changes.
        void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
        // A new ICE candidate has been gathered.
        void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
        // Ice candidates have been removed.
        void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) override {}
        // Called when the ICE connection receiving status changes.
        void OnIceConnectionReceivingChange(bool Receiving) override {}
        // This is called when signaling indicates a transceiver will be receiving
        // media from the remote endpoint. This is fired during a call to
        // SetRemoteDescription. The receiving track can be accessed by:
        // |transceiver->receiver()->track()| and its associated streams by
        // |transceiver->receiver()->streams()|.
        // Note: This will only be called if Unified Plan semantics are specified.
        // This behavior is specified in section 2.2.8.2.5 of the "Set the
        // RTCSessionDescription" algorithm:
        // https://w3c.github.io/webrtc-pc/#set-description
        void OnTrack(
            rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver);
        // GetStats callback.
        void OnStatsDelivered(const rtc::scoped_refptr<const webrtc::RTCStatsReport>& report) override;

        friend class DataChannelObject;
    public:
        DelegateCreateSDSuccess onCreateSDSuccess;
        DelegateCreateSDFailure onCreateSDFailure;
        DelegateSetSDSuccess onSetSDSuccess;
        DelegateSetSDFailure onSetSDFailure;
        DelegateLocalSdpReady onLocalSdpReady;
        DelegateIceCandidate onIceCandidate;
        DelegateOnIceConnectionChange onIceConnectionChange;
        DelegateOnDataChannel onDataChannel;
        DelegateOnRenegotiationNeeded onRenegotiationNeeded;
        DelegateOnTrack onTrack;
        DelegateOnGetStats onGetStats;
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> connection;
    private:
        Context& context;
    };
}
