#pragma once

namespace DawnStar
{
	struct Buffer
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;
		explicit Buffer(uint64_t size) { Allocate(size); }

		static Buffer Copy(Buffer other)
		{
			const auto result = Buffer(other.Size);
			std::memcpy(result.Data, other.Data, result.Size);
			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = new uint8_t[size];
			Size = size;
		}

		void Release()
		{
			delete[] Data;
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As() { return reinterpret_cast<T*>(Data); }

		operator bool() const {	return Data; }
	};

	struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: _buffer(buffer)
		{
		}

		explicit ScopedBuffer(uint64_t size)
			: _buffer(size)
		{
		}

		~ScopedBuffer() { _buffer.Release(); }

		uint8_t* Data() const { return _buffer.Data; }
		uint64_t Size() const { return _buffer.Size; }

		template<typename T>
		T* As() { return _buffer.As<T>(); }

		operator bool() const { return _buffer; }

	private:
		Buffer _buffer;
	};
}