//
// Created by mhmoud on 1/15/25.
//

#ifndef GPS_QUEUE_H
#define GPS_QUEUE_H

#ifdef MCU
using SIZETYPE = uint8_t ; // max size is 255
#else

#include <iostream>
#include <cstddef>
#include <mutex>
#include <logger.h>
#include <gpsTypes.h>

using SIZETYPE = size_t; // max size is 255
#endif

using gps::LogLevel::ERROR;
using gps::Logger;

template<typename T, SIZETYPE MaxSize>
class Queue {
private:
    T data[MaxSize];  // Array to store queue elements
    SIZETYPE front;     // Index of the front element
    SIZETYPE rear;      // Index of the rear element
    SIZETYPE currentSize;  // Current number of elements in the queue
    std::mutex queueMutex;
public:
    // Constructor to initialize the queue
    Queue() : front(0), rear(0), currentSize(0) {}

    // Push an element onto the queue
    u_int8_t push(const T &value) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (currentSize >= MaxSize) {
            return return_error_queue;
        }
        data[rear] = value;
        rear = (rear + 1) % MaxSize;
        currentSize++;
        return return_ok;
    }

    // Pop an element from the queue
    u_int8_t pop() {
        std::lock_guard<std::mutex> lock(queueMutex);  // Lock the mutex to ensure thread-safety
        if (currentSize == 0) {
#ifndef MCU
            Logger::log(ERROR, "GNSS", "Queue is empty!");
#endif
            return return_error_queue;  // Handle underflow scenario
        }
        front = (front + 1) % MaxSize;
        currentSize--;
        return return_ok;
    }

    // Get the size of the queue
    SIZETYPE size() {
        std::lock_guard<std::mutex> lock(queueMutex);  // Lock the mutex to ensure thread-safety
        return currentSize;
    }

    // Check if the queue is empty
    bool empty() {
        std::lock_guard<std::mutex> lock(queueMutex);  // Lock the mutex to ensure thread-safety
        return currentSize == 0;
    }

    // Peek at the front element (without removing it)
    T frontElement() {
        std::lock_guard<std::mutex> lock(queueMutex);  // Lock the mutex to ensure thread-safety
        if (currentSize == 0) {
#ifndef MCU
            Logger::log(ERROR, "GNSS", "Queue is empty!");
            throw std::out_of_range("Queue is empty");
#endif
            //TODO: create converting function that return null
        }
        return data[front];
    }
};

#endif //GPS_QUEUE_H
