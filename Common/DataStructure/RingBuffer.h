#pragma once

#define OUT

constexpr int BUFFER_SIZE = 1024;

class RingBuffer
{
public:
    RingBuffer();
    ~RingBuffer();

    const bool Push(const char* data, const int size);
    const bool Pop(OUT char* result_buffer, const int size);
    void Clear();
    const int PopAll(OUT char* result_buffer);
    const bool SetWriteIndex(const int index);
    void Copy(RingBuffer& buffer);

    const int GetReadIndex() { return _read_index; }
    const int GetWriteIndex() { return _write_index; }

    inline const char* Data() { return _buffer; }
    inline const bool Empty() { return (_read_index == _write_index); }

private:
    const bool CanPush(const int size);
    const bool CanPop(const int size);

private:
    char* _buffer = nullptr;
    int _read_index = 0;
    int _write_index = 0;
};
