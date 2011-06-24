using System;

namespace Cemono
{
    [Flags]
    public enum EntityFlags
    {
        CastShadow = (1 << 1),
        /// <summary>
        /// This entity cannot be removed using IEntitySystem::RemoveEntity until this flag is cleared.
        /// </summary>
        Unremovable = (1 << 2),
        GoodOccluder = (1 << 3),

        //////////////////////////////////////////////////////////////////////////
        WriteOnly = (1 << 5),
        NotRegisterInSectors = (1 << 6),
        CalcPhysics = (1 << 7),
        ClientOnly = (1 << 8),
        ServerOnly = (1 << 9),
        /// <summary>
        /// This entity have special custom view distance ratio (AI/Vehicles must have it).
        /// </summary>
        CustomViewdistRatio = (1 << 10),
        /// <summary>
        /// Use character and objects in BBOx calculations
        /// </summary>
        CalcBBoxUseAll = (1 << 11),
        /// <summary>
        /// Entity is a volume sound (will get moved around by the sound proxy)
        /// </summary>
        VolumeSound = (1 << 12),
        /// <summary>
        /// Entity has an AI object
        /// </summary>
        HasAI = (1 << 13),
        /// <summary>
        /// This entity will trigger areas when it enters them.
        /// </summary>
        TriggerAreas = (1 << 14),
        /// <summary>
        /// This entity will not be saved.
        /// </summary>
        NoSave = (1 << 15),
        /// <summary>
        /// This entity Must be present on network.
        /// </summary>
        NetPresent = (1 << 16),
        /// <summary>
        /// Prevents error when state changes on the client and does not sync state changes to the client.
        /// </summary>
        ClientsideState = (1 << 17),
        /// <summary>
        /// When set entity will send ENTITY_EVENT_RENDER every time its rendered.
        /// </summary>
        SendRenderEvent = (1 << 18),
        /// <summary>
        /// Entity will not be registered in the partition grid and can not be found by proximity queries.
        /// </summary>
        NoProximity = (1 << 19),
        /// <summary>
        /// Entity will be relevant for radar.
        /// </summary>
        OnRadar = (1 << 20),
        /// <summary>
        /// Entity will be update even when hidden.
        /// </summary>
        UpdateHidden = (1 << 21),
        /// <summary>
        /// Entity should never be considered a static entity by the network system
        /// </summary>
        NeverNetworkStatic = (1 << 22),
        /// <summary>
        /// Used by Editor only, (dont set)
        /// </summary>
        IgnorePhysicsUpdate = (1 << 23),
        /// <summary>
        /// Entity was spawned dynamically without a class
        /// </summary>
        Spawned = (1 << 24),
        /// <summary>
        /// Entity's slots were changed dynamically
        /// </summary>
        SlotsChanged = (1 << 25),
        /// <summary>
        /// Entity was procedurally modified by physics
        /// </summary>
        ModifiedByPhysics = (1 << 26),
        /// <summary>
        /// same as Brush->Outdoor only
        /// </summary>
        OutdoorOnly = (1 << 27),
        /// <summary>
        /// Entity will be sent ENTITY_EVENT_NOT_SEEN_TIMEOUT if it is not rendered for 30 seconds
        /// </summary>
        SendNotSeenTimeout = (1 << 28),
        /// <summary>
        /// Receives wind
        /// </summary>
        ReceiveWind = (1 << 29),
        LocalPlayer = (1 << 30),
        /// <summary>
        /// AI can use the object to calculate automatic hide points.
        /// </summary>
        AIHideable = (1 << 31),
    }
}
