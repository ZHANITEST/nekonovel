// ===============================================================
//  nekoByteBuffer - 2009.7.7
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoByteBuffer_h__
#define __nekoByteBuffer_h__

#include "nekoDataTypes.h"

#define MAX_BYTEBUFFER_SIZE 100 * 1024 * 1024

namespace neko
{
	class nekoByteBuffer
	{
	public:

		/// Constructor
		nekoByteBuffer(int32 size = 0)
		{
			isLimited = (size > 0);
			writePos = readPos = 0;
			buf = 0;
			length = size;

			if(isLimited)
				buf = nekoNew char8[length];
			else
				Resize(size);
		}

		/// Copy Constructor
		nekoByteBuffer(const nekoByteBuffer &from)
		{
			buf = 0;
			length = 0;
			isLimited = from.isLimited;
			writePos = from.writePos;
			readPos = from.readPos;

			if(isLimited)
				buf = nekoNew char8[(length = from.length)];
			else
				Resize(from.length);

			// Copy data.
			if(from.length)
				memcpy(buf, from.buf, from.length);
		}

		/// Destructor
		virtual ~nekoByteBuffer()
		{
			SafeDeleteArray(buf);
		}

		/// Resize
		bool Resize(int32 size)
		{
			// Buffer's length is limited.
			if(isLimited)
				return false;

			if(size > MAX_BYTEBUFFER_SIZE)
			{
				nekoAssert(false, "최대 ByteBuffer 용량을 초과하였습니다.");
				return false;
			}

			char8 *tmp = 0;

			if(size)
			{
				/// Re-allocate buffer.
				tmp = nekoNew char8[size];

				/// Older's is bigger..
				if(length >= size)
					memcpy(tmp, buf, size);
				else
					memcpy(tmp, buf, length);
			}

			this->length = size;
			SafeDeleteArray(buf);
			this->buf = tmp;

			return true;
		}

		/// Append 
		template <class T>
		nekoByteBuffer &Append(const T &data)
		{
			if(writePos + (int32)sizeof(T) > length)
			{
				if(!Resize(length + sizeof(T)))
					assert("Buffer(is limited) over-flow.");
			}

			memcpy(&buf[writePos], &data, sizeof(T));
			writePos += sizeof(T);
			return *this;
		}

		/// Append with size.
		nekoByteBuffer &Append(const void *c, int32 size)
		{
			if(writePos + size > length)
			{
				if(!Resize(length + size))
					assert("Buffer(is limited) over-flow.");
			}

			memcpy(&buf[writePos], c, size);
			writePos += size;
			return *this;
		}

		/// Read
		template <class T>
		T Read()
		{
			assert(readPos < length);
			T r = *(T *)&buf[readPos];
			readPos += sizeof(T);
			return r;
		}

		/// Read with size.
		void Read(void *c, int32 size)
		{
			assert(readPos + size <= length);
			if(c)
				memcpy(c, &buf[readPos], size);
			readPos += size;
		}

		inline int32 GetReadPosition() { return readPos; }

		inline void SetReadPosition(int32 oReadPos) { readPos = oReadPos; }

		////////////
		// Sets...

		/// Operator for STL string.
		nekoByteBuffer &operator <<(const string &str)
		{
			Append((uchar8 *)str.c_str(), str.length());	
			return Append((uchar8)0);
		}

		/// Operator for character-array string
		nekoByteBuffer &operator <<(const char8 *str)
		{
			Append((uchar8 *)str, strlen(str));
			return Append((uchar8)0);
		}

		/// Operator for 8bit bool.
		nekoByteBuffer &operator <<(bool b)
		{
			return Append<bool>(b);
		}

		/// Operator for 8bit char.
		nekoByteBuffer &operator <<(char8 c)
		{
			return Append<char8>(c);
		}

		/// Operator for 8bit uchar.
		nekoByteBuffer &operator <<(uchar8 c)
		{
			return Append<uchar8>(c);
		}

		/// Operator for 16bit int.
		nekoByteBuffer &operator <<(int16 i)
		{
			return Append<int16>(i);
		}

		/// Operator for 32bit int.
		nekoByteBuffer &operator <<(int32 i)
		{
			return Append<int32>(i);
		}

		/// Operator for 32bit uint.
		nekoByteBuffer &operator <<(uint32 i)
		{
			return Append<uint32>(i);
		}

		/// Operator for 32bit float.
		nekoByteBuffer &operator <<(float32 f)
		{
			return Append<float32>(f);
		}

		/// Operator for 64bit float.
		nekoByteBuffer &operator <<(float64 f)
		{
			return Append<float64>(f);
		}

		/// Operator for custom data.
		template <class T>
		nekoByteBuffer &operator <<(const T &c)
		{
			return Append<T>(c);
		}

		////////////
		// Gets...

		/// Operator for STL string.
		nekoByteBuffer &operator >>(string &str)
		{
			str = "";
			while(readPos < length)
			{
				char8 c = Read<char8>();
				if(!c)
					break;
				str += c;
			}

			return *this;
		}

		/// Operator for character-array string
		nekoByteBuffer &operator >>(char8 *str)
		{
			while(readPos < length)
			{
				char8 c = Read<char8>();
				*str++ = c;
				if(!c)
					break;
			}
			*str = '\0';

			return *this;
		}

		/// Operator for 8bit bool.
		nekoByteBuffer &operator >>(bool &b)
		{
			b = Read<bool>();
			return *this;
		}

		/// Operator for 8bit char.
		nekoByteBuffer &operator >>(char8 &c)
		{
			c = Read<char8>();
			return *this;
		}

		/// Operator for 8bit uchar.
		nekoByteBuffer &operator >>(uchar8 &c)
		{
			c = Read<uchar8>();
			return *this;
		}

		/// Operator for 16bit int.
		nekoByteBuffer &operator >>(int16 &c)
		{
			c = Read<int16>();
			return *this;
		}

		/// Operator for 32bit int.
		nekoByteBuffer &operator >>(int32 &c)
		{
			c = Read<int32>();
			return *this;
		}

		/// Operator for 32bit uint.
		nekoByteBuffer &operator >>(uint32 &c)
		{
			c = Read<uint32>();
			return *this;
		}

		/// Operator for 32bit float.
		nekoByteBuffer &operator >>(float32 &c)
		{
			c = Read<float32>();
			return *this;
		}

		/// Operator for 64bit float.
		nekoByteBuffer &operator >>(float64 &c)
		{
			c = Read<float64>();
			return *this;
		}

		/// Operator for custom data.
		template <class T>
		nekoByteBuffer &operator >>(T &c)
		{
			c = Read<T>();
			return *this;
		}

		/// Get buffer
		inline char8 *GetBuffer()
		{
			return buf;
		}

		/// Get Length
		inline int32 GetLength()
		{
			return length;
		}

		/// Clear
		inline void Clear()
		{
			writePos = readPos = 0;
		}

		/// Is End position ?
		inline bool IsReadEndPosition()
		{
			return (readPos == length);
		}

		/// Compress data by Zlib
		bool Compress(int32 level);

		/// Uncompress data by Zlib
		bool Uncompress();

		void AdoptBuffer(char8 *nBuf, int32 nlength)
		{
			nekoAssert(nBuf, "");
			SafeDeleteArray(buf);

			buf = nBuf;
			length = nlength;
		}

		char8 *ReleaseBuffer()
		{
			char8 *obuf = buf;

			readPos = 0;
			writePos = 0;
			buf = 0;
			Resize(0);

			return obuf;
		}

		inline const char8 *GetReadPositionBuffer()
		{
			return buf + readPos;
		}

	protected:

		/// Is Limited ?
		bool isLimited;

		/// Buffer
		char8 *buf;

		/// Write Pos
		int32 writePos;

		/// Read Pos
		int32 readPos;

		/// Length
		int32 length;

	};
}


#endif // __nekoByteBuffer_h__