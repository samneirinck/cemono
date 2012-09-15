using System;

namespace CryEngine.Native
{
    internal interface INativeActorMethods
    {
        float GetPlayerHealth(IntPtr actorPtr);
        void SetPlayerHealth(IntPtr actorPtr, float newHealth);
        float GetPlayerMaxHealth(IntPtr actorPtr);
        void SetPlayerMaxHealth(IntPtr actorPtr, float newMaxHealth);

		bool IsPlayer(IntPtr actorPtr);
		bool IsClient(IntPtr actorPtr);

        ActorInfo GetActorInfoByChannelId(ushort channelId);
        ActorInfo GetActorInfoById(uint entId);
        ActorInfo CreateActor(int channelId, string name, string className, Vec3 pos, Vec3 angles, Vec3 scale);
        void RemoveActor(uint id);
        uint GetClientActorId();
    }
}