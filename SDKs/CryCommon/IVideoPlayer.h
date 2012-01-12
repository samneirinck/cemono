#include DEVIRTUALIZE_HEADER_FIX(IVideoPlayer.h)

#ifndef _I_VIDEO_PLAYER_H_
#define _I_VIDEO_PLAYER_H_

#pragma once
// Summary:
//	 VideoPlayer interface.
struct IVideoPlayer
{
	// Summary:
	//	 Flags for video playback option.
	enum EOptions
	{
		LOOP_PLAYBACK = 0x01, // Infinitely loop playback of video.
		DELAY_START   = 0x02  // Delay start of playback until first Render() call.
	};

	// Summary:
	//	 Playback status flags.
	enum EPlaybackStatus
	{
		PBS_ERROR,
		PBS_PREPARING,
		PBS_PLAYING,
		PBS_FINISHED,
		PBS_PAUSED,
		PBS_PAUSED_NOFOCUS,
		PBS_STOPPED
	};

	// Lifetime.
	//##@{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	//##@}

	// Initialization
	virtual bool Load(const char* pFilePath, unsigned int options, int audioCh = 0, int voiceCh = -1, bool useSubtitles = false) = 0;

	// Rendering

	// Summary:
	//	 Gets the playback status.
	virtual IVideoPlayer::EPlaybackStatus GetStatus() const = 0;
	// Summary:
	//	 Starts the playback.
	virtual bool Start() = 0;
	// Summary:
	//	 Stops the playback.
	virtual bool Stop() = 0;
	// Summary:
	//	 Pauses the playback.
	virtual bool Pause(bool pause) = 0;	
	// Summary:
	//	 Sets the viewport of the playback.
	virtual bool SetViewport(int x0, int y0, int width, int height) = 0;
	virtual void Render(bool stereo = false, bool displayWhileLoading = false) = 0;
	// Summary:
	//	 Gets the subtitle for the video playback.
	virtual void GetSubtitle(int subtitleCh, char* pStBuf, size_t pStBufLen) = 0;

	// Summary:
	//	 Enables/disables the updates frame-by-frame.
	virtual void EnablePerFrameUpdate(bool enable) = 0;
	// Summary:
	//	 Checks if the update frame-by-frame is enabled.
	virtual bool IsPerFrameUpdateEnabled() const = 0;


	// General property queries

	// Summary:
	//	 Gets the width.
	virtual int GetWidth() const = 0;
	// Summary:
	//	 Gets the height.
	virtual int GetHeight() const = 0;

	// more to come...

protected:
	IVideoPlayer() {}
	virtual ~IVideoPlayer() {}
};


struct IVideoPlayer_RenderProxy
{
	enum EFrameType
	{
		EFT_Mono,
		EFT_StereoLeft,
		EFT_StereoRight
	};

	virtual void RenderCallback(EFrameType ft, bool displayWhileLoading = false, bool releaseOnExit = true) = 0;
	virtual void DestroyTexturesCallback() = 0;

protected:
	IVideoPlayer_RenderProxy() {}
	virtual ~IVideoPlayer_RenderProxy() {}
};


#endif // #ifndef _I_VIDEO_PLAYER_H_
