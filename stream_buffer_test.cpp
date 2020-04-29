#include <iostream>
#include <thread>

#include "stream_buffer.h"

void input_task(chiptune::StreamBuffer<int>& buf);
void output_task(chiptune::StreamBuffer<int>& buf, int thread_num);

int main(int argc, char** argv) {
    chiptune::StreamBuffer<int> buf;

    auto input = std::bind(input_task, std::ref(buf));
    auto output1 = std::bind(output_task, std::ref(buf), 1);
    auto output2 = std::bind(output_task, std::ref(buf), 2);
    
    std::thread thread(input);

    std::thread thread1(output1);
    std::thread thread2(output2);

    thread2.join();
    thread1.join();
    thread.join();

    return 0;
}


void input_task(chiptune::StreamBuffer<int>& buf) {
    int n;
    while(true) {
        std::cin >> n;
        buf << n;
    }
}

void output_task(chiptune::StreamBuffer<int>& buf, int thread_num) {
    int n;
    while (true) {
        buf >> n;
        std::cout << "Thread " << thread_num << " get n=" << n << std::endl;
    }
}
