#include "segBuffer.h"
#include "bufferQue.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include "FixedCDC.h"
#include <atomic>
#include <thread>
std::atomic<bool> finished=false;

const size_t BUF_SIZE = 2 * 1024 * 1024;  // 2M
struct readBuf{
    std::array<char, BUF_SIZE> data;
    size_t offset;
    size_t size;
    readBuf(){
        reset();
    }
    reset(){
        offset = 0;
        size = 0;
    }
    char* data(){
        return data.data()+offset;
    }
};
bool read_file_to_buffer_queue(const std::string& file_path, BufferQueue<SegBuffer>& buffer_queue) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "cannot open file: " << file_path << std::endl;
        return false;
    }
    //TODO:ring buffer
    readBuf read_buffer;
    
    size_t total_read = 0;
    size_t chunks_read = 0;

    while (true) {

        file.read(read_buffer.data(), BUF_SIZE);
        std::streamsize bytes_read = file.gcount();
        read_buffer.size = bytes_read;
        if (bytes_read <= 0) {
            break;
        }

        std::unique_ptr<CDC> cdc = CDC::create("fixed", 17);
        while(true){
            uint64_t chunk_offset = 0;
            if(cdc->calc_chunks(read_buffer.data(), static_cast<size_t>(bytes_read), chunk_offset) == -1){
                std::cout<< "hit boundary"<<std::endl;
                break;
            }
            
            SegBuffer buffer(static_cast<size_t>(chunk_offset-read_buffer.offset));
            buffer.write(read_buffer.data(), chunk_offset-read_buffer.offset);
            buffer_queue.enqueue(std::move(buffer));

            read_buffer.offset=chunk_offset;
            chunks_read++;
        }
        
        total_read += static_cast<size_t>(bytes_read);
        
        if (chunks_read % 10 == 0) {
            std::cout << "read: " << chunks_read << " chunks, total is " << total_read / 1024 << " KB" << std::endl;
        }
    }
    finished.store(true);
    //finish
    buffer_queue.enqueue(SegBuffer(0));

    std::cout << "read finished. Total " << chunks_read << " chunks, " << total_read / 1024 << " KB" << std::endl;
    std::cout << "buffer queue currently contains " << buffer_queue.size() << " chunks" << std::endl;
    
    return true;
}

void display_queue_info(const BufferQueue<SegBuffer>& buffer_queue) {
    std::cout << "queue info:" << std::endl;
    std::cout << "chunks: " << buffer_queue.size() << std::endl;
    std::cout << "total size: " << buffer_queue.total_size() / 1024 << " KB" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    BufferQueue<SegBuffer> buffer_queue;

    //TODO: create another thread to send the segment to server
    std::thread sender_thread([&buffer_queue]() {
        while (!finished.load() || buffer_queue.size() > 0) {
            SegBuffer buffer;
            if (buffer_queue.dequeue(buffer)) {
                // Send buffer to server
            }
        }
    });

    try {
        if (!read_file_to_buffer_queue(file_path, buffer_queue)) {
            return 1;
        }

        display_queue_info(buffer_queue);

        sender_thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "error happened: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}