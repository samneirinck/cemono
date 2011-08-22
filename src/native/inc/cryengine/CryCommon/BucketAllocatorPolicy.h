#ifndef BUCKETALLOCATORPOLICY_H
#define BUCKETALLOCATORPOLICY_H

namespace BucketAllocatorDetail
{

	struct AllocHeader
	{
		AllocHeader* volatile next;

#ifdef BUCKET_ALLOCATOR_TRAP_DOUBLE_DELETES
		uint32 magic;
#endif
	};

	struct SyncPolicyLocked
	{




#if defined(_WIN32)
		struct FreeListHeader
		{
			volatile int lock;
			AllocHeader* header;
		};
#endif

#if defined(XENON) || defined(_WIN32)

		union Lock
		{
			volatile int lock;
			uint8 padding[128];
		} BUCKET_ALIGN(128);

		Lock m_cleanupLock;
		Lock m_refillLock;

		struct CleanupReadLock
		{
			ILINE CleanupReadLock(SyncPolicyLocked& policy)
			{
				int iter=0;
				while (policy.m_cleanupLock.lock)
					//Sleep 0 is yield and a potential deadlock if the thread holding the lock is preempted
					Sleep((++iter>100)?1:0);
			}
			~CleanupReadLock() {}

		private:
			CleanupReadLock(const CleanupReadLock&);
			CleanupReadLock& operator = (const CleanupReadLock&);
		};

		struct CleanupWriteLock
		{
			CleanupWriteLock(SyncPolicyLocked& policy) : m_policy(policy) { CrySpinLock(&policy.m_cleanupLock.lock, 0, 1); }
			~CleanupWriteLock() { CryReleaseSpinLock(&m_policy.m_cleanupLock.lock, 0); }

		private:
			SyncPolicyLocked& m_policy;
		};

		struct RefillLock
		{
			RefillLock(SyncPolicyLocked& policy) : m_policy(policy) { CrySpinLock(&m_policy.m_refillLock.lock, 0, 1); }
			~RefillLock() { CryReleaseSpinLock(&m_policy.m_refillLock.lock, 0); }

		private:
			SyncPolicyLocked& m_policy;
		};
		




























































#endif


































































































#if defined(_WIN32)

		static void PushOnto(FreeListHeader& list, AllocHeader* ptr)
		{
			CrySpinLock(&list.lock, 0, 1);

			ptr->next = list.header;
			list.header = ptr;

			CryReleaseSpinLock(&list.lock, 0);
		}

		static void PushListOnto(FreeListHeader& list, AllocHeader* head, AllocHeader* tail)
		{
			CrySpinLock(&list.lock, 0, 1);

			tail->next = list.header;
			list.header = head;

			CryReleaseSpinLock(&list.lock, 0);
		}
		
		static AllocHeader* PopOff(FreeListHeader& list)
		{
			CrySpinLock(&list.lock, 0, 1);

			AllocHeader* ret = list.header;
			if (ret)
			{
				list.header = ret->next;
			}

			CryReleaseSpinLock(&list.lock, 0);

			return ret;
		}

		ILINE static AllocHeader*& GetFreeListHead(FreeListHeader& list)
		{
			return list.header;
		}

#endif

	};

	struct SyncPolicyUnlocked
	{
		typedef AllocHeader* FreeListHeader;

		struct CleanupReadLock { CleanupReadLock(SyncPolicyUnlocked&) {} };
		struct CleanupWriteLock { CleanupWriteLock(SyncPolicyUnlocked&) {} };
		struct RefillLock { RefillLock(SyncPolicyUnlocked&) {} };

		ILINE static void PushOnto(FreeListHeader& list, AllocHeader* ptr)
		{
			ptr->next = list;
			list = ptr;
		}

		ILINE static void PushListOnto(FreeListHeader& list, AllocHeader* head, AllocHeader* tail)
		{
			tail->next = GetFreeListHead(list);
			GetFreeListHead(list) = head;
		}

		ILINE static AllocHeader* PopOff(FreeListHeader& list)
		{
			AllocHeader* top = list;
			if (top)
				list = *(AllocHeader**)(&top->next); // cast away the volatile
			return top;
		}

		ILINE static AllocHeader*& GetFreeListHead(FreeListHeader& list)
		{
			return list;
		}
	};

	template <size_t Size, typename SyncingPolicy, bool FallbackOnCRT = true>
	struct DefaultTraits
	{
		enum
		{
			MaxSize = 512,
			NumBuckets = 32 / 4 + (512 - 32) / 8,

			PageLength = 64 * 1024,
			SmallBlockLength = 1024,
			SmallBlocksPerPage = 64,

			NumGenerations = 4,

			NumPages = Size / PageLength,

			FallbackOnCRTAllowed = FallbackOnCRT,
		};

		typedef SyncingPolicy SyncPolicy;

		static uint8 GetBucketForSize(size_t sz)
		{
			if (sz <= 32)
			{
				const int alignment = 4;
				size_t alignedSize = (sz + (alignment - 1)) & ~(alignment - 1);
				return alignedSize / alignment - 1;
			}
			else
			{
				const int alignment = 8;
				size_t alignedSize = (sz + (alignment - 1)) & ~(alignment - 1);
				alignedSize -= 32;
				return alignedSize / alignment + 7;
			}
		}

		static size_t GetSizeForBucket(uint8 bucket)
		{
			size_t sz;

			if (bucket <= 7)
				sz = (bucket + 1) * 4;
			else
				sz = (bucket - 7) * 8 + 32;

#ifdef BUCKET_ALLOCATOR_TRAP_DOUBLE_DELETES
			return sz < sizeof(AllocHeader) ? sizeof(AllocHeader) : sz;
#else
			return sz;
#endif
		}

		static size_t GetGenerationForStability(uint8 stability)
		{
			return 3 - stability / 64;
		}
	};
}

#endif
