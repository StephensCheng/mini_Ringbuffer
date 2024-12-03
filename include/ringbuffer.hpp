#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <memory>

class Ringbuffer
{
    public:
        Ringbuffer(size_t size);
        ~Ringbuffer();
        size_t write(const uint8_t *data, size_t len);
        size_t read(uint8_t *data, size_t len);
        size_t length();
        size_t capacity();
        void clear();
        
    private:
        size_t m_length;
        size_t m_capacity;
        std::atomic<size_t> m_head;  // head is the next position to read
        std::atomic<size_t> m_tail; // tail is the next position to write
        std::unique_ptr<uint8_t[]> m_buffer;
};