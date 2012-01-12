#ifndef __LINESTREAMBUFFER_H__
#define __LINESTREAMBUFFER_H__

class LineStreamBuffer
{
public:
	template <typename T> LineStreamBuffer(T* object, void (T::*method)(const char* line))
	{
		m_target = new Target<T>(object, method);
	}

	~LineStreamBuffer()
	{
		Flush();
		delete m_target;
	}

	void HandleText(const char* text, int length)
	{
		const char* pos = text;
		while(pos - text < length)
		{
			const char* start;
			start = pos;

			while(pos - text < length && *pos != '\n' && *pos != '\r')
			{
				 ++pos;
			}
			m_buffer.append(start, pos);
			if (*pos == '\n' || *pos == '\r')
			{
				m_target->Call(m_buffer.c_str());
				m_buffer.resize(0);
				while (*pos == '\n' || *pos == '\r')
					++pos;
			}
		}
	}

	void Flush()
	{
		if (!m_buffer.empty())
		{
			m_target->Call(m_buffer.c_str());
			m_buffer.resize(0);
		}
	}

private:
	struct ITarget
	{
		virtual ~ITarget() {}
		virtual void Call(const char* line) = 0;
	};
	template <typename T> struct Target : public ITarget
	{
	public:
		Target(T* object, void (T::*method)(const char* line)): object(object), method(method) {}
		virtual void Call(const char* line)
		{
			(object->*method)(line);
		}
	private:
		T* object;
		void (T::*method)(const char* line);
	};

	ITarget* m_target;
	string m_buffer;
};

#endif //__LINESTREAMBUFFER_H__
