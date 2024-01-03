#include "RingBuffer.h"
#include <memory.h>

RingBuffer::RingBuffer()
{
    _buffer = new char[BUFFER_SIZE];
}

RingBuffer::~RingBuffer()
{
    if (nullptr != _buffer)
    {
        delete[] _buffer;
    }
}

const bool RingBuffer::Push(const char* data, const int size)
{
    if (false == CanPush(size))
    {
        return false;
    }

    for (int i = 0; i < size; ++i)
    {
        _buffer[_write_index++] = data[i];
        _write_index %= BUFFER_SIZE;
    }

    return true;
}

const bool RingBuffer::Pop(OUT char* result_buffer, const int size)
{
    if (false == CanPop(size))
    {
        return false;
    }

    for (int i = 0; i < size; ++i)
    {
        result_buffer[i] = _buffer[_read_index++];
        _read_index %= BUFFER_SIZE;
    }

    return true;
}

void RingBuffer::Clear()
{
    memset(_buffer, 0, BUFFER_SIZE);
    _read_index = 0;
    _write_index = 0;
}

const int RingBuffer::PopAll(OUT char* result_buffer)
{
    int pop_count = 0;

    while (false == Empty())
    {
        Pop(result_buffer, 1);
        ++pop_count;
    }

    return pop_count;
}

const bool RingBuffer::SetWriteIndex(const int index)
{
    if (index >= BUFFER_SIZE)
    {
        return false;
    }

    _write_index += index;
    _write_index %= BUFFER_SIZE;

    return true;
}

void RingBuffer::Copy(RingBuffer& buffer)
{
    memcpy_s(_buffer, BUFFER_SIZE, buffer.Data(), BUFFER_SIZE);
    _read_index = buffer.GetReadIndex();
    _write_index = buffer.GetWriteIndex();
}

const int RingBuffer::AvailableSize()
{
    if (_read_index > _write_index)
    {
        return ((BUFFER_SIZE - 1) - ((_write_index + BUFFER_SIZE) - _read_index));
    }
    else
    {
        return ((BUFFER_SIZE - 1) - (_write_index - _read_index));
    }
}

const bool RingBuffer::CanPush(const int size)
{
    if (size <= 0 || size >= BUFFER_SIZE)
    {
        return false;
    }

    if (_read_index > _write_index)
    {
        return (size <= (BUFFER_SIZE - 1) - ((_write_index + BUFFER_SIZE) - _read_index));
    }
    else
    {
        return (size <= (BUFFER_SIZE - 1) - (_write_index - _read_index));
    }
}

const bool RingBuffer::CanPop(const int size)
{
    if (size <= 0 || size >= BUFFER_SIZE)
    {
        return false;
    }

    if (_read_index > _write_index)
    {
        return (size <= (_write_index + BUFFER_SIZE) - _read_index);
    }
    else
    {
        return (size <= _write_index - _read_index);
    }
}
