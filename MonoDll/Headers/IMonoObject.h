#ifndef __I_MONO_OBJECT_H__
#define __I_MONO_OBJECT_H__

struct IMonoObject
{
public:
	template <class T>
	T Unbox() { return *(T *)UnboxObject(); }

private:
	virtual void *UnboxObject() = 0;
};

#endif //__I_MONO_OBJECT_H__