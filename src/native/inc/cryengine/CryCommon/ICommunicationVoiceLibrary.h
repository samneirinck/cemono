#pragma once

#ifndef ICommunicationVoiceLibrary_h
#define ICommunicationVoiceLibrary_h

struct ICommunicationVoiceLibrary
{
	virtual ~ICommunicationVoiceLibrary(){}
	virtual uint32 GetGroupNames(const char* libraryName, uint32 maxCount, const char** names) const = 0;
};

#endif // ICommunicationVoiceLibrary_h
