using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// 
	/// </summary>
	public abstract class EntityBase : CryScriptInstance
	{
		#region Externals
		/// <summary>
		/// Spawns an entity with the specified parameters.
		/// </summary>
		/// <param name="spawnParams">The <see cref="EntitySpawnParams"/></param>
		/// <param name="autoInit">Should the entity automatically be initialised?</param>
		/// <returns></returns>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _SpawnEntity(EntitySpawnParams spawnParams, bool autoInit = true);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveEntity(uint entityId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _RegisterEntityClass(EntityRegisterParams registerParams, object[] properties);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Physicalize(uint entityId, PhysicalizationParams physicalizationParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Sleep(uint entityId, bool sleep);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _BreakIntoPieces(uint entityId, int slot, int piecesSlot, BreakageParameters breakageParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddImpulse(uint entityId, ActionImpulse actionImpulse);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetVelocity(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetVelocity(uint entityId, Vec3 velocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetMaterial(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetMaterial(uint entityId, string material);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetName(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetName(uint entityId, string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static EntityFlags _GetFlags(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetFlags(uint entityId, EntityFlags name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddMovement(uint entityId, ref EntityMovementRequest request);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetWorldTM(uint entityId, Matrix34 tm);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Matrix34 _GetWorldTM(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetLocalTM(uint entityId, Matrix34 tm);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Matrix34 _GetLocalTM(uint entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static BoundingBox _GetBoundingBox(uint entityId, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static EntitySlotFlags _GetSlotFlags(uint entityId, int slot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetSlotFlags(uint entityId, int slot, EntitySlotFlags slotFlags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetWorldPos(uint entityId, Vec3 newPos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetWorldPos(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetRotation(uint entityId, Quat newAngles);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Quat _GetRotation(uint entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetPropertyValue(uint entityId, string propertyName);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetPropertyValue(uint entityId, string property, string value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LoadObject(uint entityId, string fileName, int slot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetStaticObjectFilePath(uint entityId, int slot);

		/// <summary>
		/// Loads an non-static model on the object (.chr, .cdf, .cga)
		/// </summary>
		/// <param name="entityId"></param>
		/// <param name="fileName"></param>
		/// <param name="slot"></param>
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LoadCharacter(uint entityId, string fileName, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _CreateGameObjectForEntity(uint entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static uint _FindEntity(string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static object[] _GetEntitiesByClass(string className);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _EntityExists(uint entityId);
		#endregion

		internal void InitPhysics()
		{
			_physics = new PhysicsParams(Id);
			_physics.Slot = 0;
		}

		public EntitySlotFlags GetSlotFlags(int slot = 0)
		{
			return _GetSlotFlags(Id, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
			_SetSlotFlags(Id, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return _GetVelocity(Id); }

			set { _SetVelocity(Id, value); }
		}

		PhysicsParams _physics;
		public PhysicsParams Physics { get { return _physics; } set { _physics = value; _physics._entityId = Id; } }

		public Vec3 Position { get { return _GetWorldPos(Id); } set { _SetWorldPos(Id, value); } }
		public Quat Rotation { get { return _GetRotation(Id); } set { _SetRotation(Id, value); } }

		public Matrix34 WorldTM { get { return _GetWorldTM(Id); } set { _SetWorldTM(Id, value); } }
		public Matrix34 LocalTM { get { return _GetLocalTM(Id); } set { _SetLocalTM(Id, value); } }

		public BoundingBox BoundingBox { get { return _GetBoundingBox(Id, 0); } }

		public string Name { get { return _GetName(Id); } set { _SetName(Id, value); } }
		public EntityFlags Flags { get { return _GetFlags(Id); } set { _SetFlags(Id, value); } }

		public EntityId Id { get; set; }
	}
}
