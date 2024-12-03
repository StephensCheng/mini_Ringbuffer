#include "ringbuffer.hpp"

Ringbuffer::Ringbuffer(size_t size)
    : m_length(0)
    , m_capacity(size + 1)
    , m_head(0)
    , m_tail(0)
    , m_buffer(new uint8_t[m_capacity])
{
}

Ringbuffer::~Ringbuffer()
{
}

size_t Ringbuffer::length()
{
    return m_length;
}

size_t Ringbuffer::capacity()
{
    return m_capacity - 1;
}

void Ringbuffer::clear()
{
    m_head = 0;
    m_tail = 0;
    m_length = 0;
    m_buffer.reset(new uint8_t[m_capacity]);
}

size_t Ringbuffer::write(const uint8_t *data, size_t len)
{
    if (len > m_capacity - m_length || len == 0)
    {
        return 0;
    }
    
    size_t currhead = m_head.load(std::memory_order_relaxed);
    size_t write_len_a = 0;
    size_t write_len_b = 0;
    
    size_t tail = m_tail.load(std::memory_order_acquire);
    if(m_capacity - tail >= len)
    {
        write_len_a = len;
        write_len_b = 0;
        std::memcpy(m_buffer.get() + tail, data, write_len_a);
        m_length += len;
        m_tail.store((tail + len) % m_capacity, std::memory_order_release);
    }
    else
    {
        write_len_a = m_capacity - tail;
        write_len_b = len - write_len_a;
        std::memcpy(m_buffer.get() + tail, data, write_len_a);
        std::memcpy(m_buffer.get(), data + write_len_a, write_len_b);
        m_length += len;
        m_tail.store(write_len_b, std::memory_order_release);
    }
    printf("write_len_a: %d, write_len_b: %d, m_head: %d, m_tail: %d\n", write_len_a, write_len_b, m_head.load(), m_tail.load());
    return len;
}

size_t Ringbuffer::read(uint8_t *data, size_t len)
{
    if (len > m_length || len == 0)
    {
        return 0;
    }
    
    size_t curr_tail = m_tail.load(std::memory_order_relaxed);
    size_t read_len_a = 0;
    size_t read_len_b = 0;
    
    size_t head = m_head.load(std::memory_order_acquire);
    if(m_capacity - head >= len)
    {
        read_len_a = len;
        read_len_b = 0;
        std::memcpy(data, m_buffer.get() + head, read_len_a);
        m_length -= len;
        m_head.store((head + len) % m_capacity, std::memory_order_release);
    }
    else
    {
        read_len_a = m_capacity - head;
        read_len_b = len - read_len_a;
        std::memcpy(data, m_buffer.get() + head, read_len_a);
        std::memcpy(data + read_len_a, m_buffer.get(), read_len_b);
        m_length -= len;
        m_head.store(read_len_b, std::memory_order_release);
    }
    printf("read_len_a: %d, read_len_b: %d, m_head: %d, m_tail: %d\n", read_len_a, read_len_b, m_head.load(), m_tail.load());
    return len;
}