#include "segBuffer.h"
#include "bufferQue.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>

const size_t CHUNK_SIZE = 128 * 1024;  // 128KB

bool read_file_to_buffer_queue(const std::string& file_path, BufferQueue<SegBuffer>& buffer_queue) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "cannot open file: " << file_path << std::endl;
        return false;
    }

    std::vector<char> temp_buffer(CHUNK_SIZE);
    size_t total_read = 0;
    size_t chunks_read = 0;

    while (true) {

        file.read(temp_buffer.data(), CHUNK_SIZE);
        std::streamsize bytes_read = file.gcount();
        
        if (bytes_read <= 0) {
            break;
        }

        SegBuffer buffer(static_cast<size_t>(bytes_read));
        buffer.write(temp_buffer.data(), static_cast<size_t>(bytes_read));
        buffer_queue.enqueue(std::move(buffer));
        
        total_read += static_cast<size_t>(bytes_read);
        chunks_read++;
        
        if (chunks_read % 10 == 0) {
            std::cout << "read: " << chunks_read << " chunks, total is " << total_read / 1024 << " KB" << std::endl;
        }
    }

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

    try {
        if (!read_file_to_buffer_queue(file_path, buffer_queue)) {
            return 1;
        }

        display_queue_info(buffer_queue);
    }
    catch (const std::exception& e) {
        std::cerr << "error happened: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}