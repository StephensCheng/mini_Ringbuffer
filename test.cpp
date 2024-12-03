#include <iostream>
#include <thread>
#include <condition_variable>
#include <atomic>
#include "ringbuffer.hpp"

int test()  // The deepth ringbuffer is 100, and the data is 5 bytes
{
    Ringbuffer rb(100);
    std::condition_variable cv;
    std::mutex mtx; // Mutex for synchronizing access to the buffer
    std::atomic<bool> done(false); // To signal the consumer to stop

    std::thread t1([&rb, &cv, &mtx, &done](){
        uint8_t data[5] = {0};
        for(int i = 0; i < 10; i++)
        {
            std::fill(data, data + sizeof(data), i);
            {
                // while (rb.write(data, sizeof(data)) != sizeof(data))  // Write to the buffer loop
                // {
                //     std::cout << "Write failed, retrying..." << std::endl;
                // }
                rb.write(data, sizeof(data));
                std::cout << "Writing: " << i << std::endl;
            }
            cv.notify_one(); // Notify the consumer
        }
        done = true; // Signal completion
        cv.notify_one(); // Notify in case consumer is waiting
    });

    std::thread t2([&rb, &cv, &mtx, &done](){
        uint8_t data[5] = {0};
        while(true)
        {
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&rb, &done] { return rb.length() > 0 || done; });

                if (done && rb.length() == 0) 
                    break; // Exit if done and buffer is empty

                rb.read(data, sizeof(data)); // Read from the buffer
                printf("Reading: %d\n", data[0]);
            }

            for (int i = 0; i < sizeof(data); i++)
            {
                std::cout << (int)data[i] << " ";
            }
            std::cout << std::endl;
        }
    });

    t1.join();
    t2.join();
    return 0;
}

int main()
{    
    Ringbuffer rb(100);

    std::thread t1([&rb](){
        uint8_t data[5] = {0};
        for(int i = 0; i < 10; i++)
        {
            std::fill(data, data + sizeof(data), i);
            if(!rb.write(data, sizeof(data)))
            {
                std::cout << "Writing: " << i << std::endl;
            }

        }
    });

    std::thread t2([&rb](){
        uint8_t data[5] = {0};
        for(int i = 0; i < 100; i++)
        {
            if(!rb.read(data, sizeof(data)))
            {
                std::cout << "Reading: " << i << std::endl;
            }
            else{
                for (int i = 0; i < sizeof(data); i++)
                {
                    std::cout << (int)data[i] << " ";
                }
                std::cout << std::endl;
            }
        }
    });

    t1.join();
    t2.join();
    return 0;
}