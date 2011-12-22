#ifndef __I_MONO_ARRAY_H__
#define __I_MONO_ARRAY_H__

struct IMonoObject;

struct IMonoArray
{
public:
	virtual void Clear() = 0;
	virtual int GetSize() const = 0;

	virtual IMonoObject *GetItem(int index) = 0;
	template <class T>
	T GetItemUnboxed(int index) { return GetItem(index)->Unbox<T>(); }
	virtual const char *GetItemString(int index) = 0;

	virtual void InsertObject(IMonoObject *pObject) = 0;
	virtual void InsertString(const char *string) = 0;
	virtual void InsertArray(IMonoArray *pArray) = 0;
};

#endif //__I_MONO_ARRAY_H__