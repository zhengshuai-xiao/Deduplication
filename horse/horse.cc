#include "common/segBuffer.h"
#include "common/bufferQue.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include "common/fixedCDC.h"
#include <atomic>
#include <thread>
#include "include/rpc_client.h"
#include "include/common.h"

struct horse_config {
    std::string server_address;
    int timeout;
    std::string log_level;
    std::string log_path;
};

horse_config horse_cfg;

void config_init(const std::string cfg_path) {
    ConfigManager::Instance().LoadFromJson(cfg_path);
    horse_cfg.server_address = ConfigManager::Instance().Get<std::string>("server.address", "0.0.0.0:50051");
    horse_cfg.timeout = ConfigManager::Instance().Get<int>("server.timeout", 30);
    horse_cfg.log_level = ConfigManager::Instance().Get<std::string>("log.level", "info");
    horse_cfg.log_path = ConfigManager::Instance().Get<std::string>("log.path", "/var/log/horse.log");
}

std::atomic<bool> finished=false;

const size_t BUF_SIZE = 2 * 1024 * 1024;  // 2M
struct readBuf{
    std::array<char, BUF_SIZE> buf;
    size_t offset;
    size_t size;
    readBuf(){
        reset();
    }
    void reset(){
        offset = 0;
        size = 0;
    }
    char* data(){
        return &buf[offset];
    }
    size_t remaining() const {
        return size - offset;
    }
};
size_t read_file_to_buffer_queue(const std::string& file_path, BufferQueue<SegBuffer>& buffer_queue) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR() << "cannot open file: " << file_path ;
        return 0;
    }
    //TODO:ring buffer
    readBuf read_buffer;
    
    size_t total_read = 0;
    size_t chunks_read = 0;

    while (true) {

        read_buffer.reset();
        
        file.read(read_buffer.data(), BUF_SIZE);
        std::streamsize bytes_read = file.gcount();
        read_buffer.size = bytes_read;
        if (bytes_read <= 0) {
            break;
        }

        std::unique_ptr<CDC> cdc = CDC::create("fixed", 17);
        bool finish_cut = false;
        while(!finish_cut){
            uint64_t chunk_size = 0;
            if(cdc->calc_chunks(read_buffer.data(), read_buffer.remaining(), chunk_size) == -1){
                LOG_TRACE() << "hit boundary";
                if(chunk_size == 0){
                    break;
                }
                finish_cut = true;
            }

            std::unique_ptr<SegBuffer> pbuffer = std::make_unique<SegBuffer>(static_cast<size_t>(chunk_size));
            pbuffer->write(read_buffer.data(), chunk_size);
            buffer_queue.enqueue(std::move(pbuffer));

            read_buffer.offset += chunk_size;
            chunks_read++;
        }
        
        total_read += static_cast<size_t>(bytes_read);
        
        if (chunks_read % 10 == 0) {
            LOG_TRACE() << "read: " << chunks_read << " chunks, total is " << total_read / 1024.0 << " KB" ;
        }
    }
    finished.store(true);
    //finish
    buffer_queue.enqueue(std::make_unique<SegBuffer>(0));

    LOG_INFO() << "read finished. Total " << chunks_read << " chunks, " << total_read / 1024.0 << " KB";
    LOG_INFO() << "buffer queue currently contains " << buffer_queue.size() << " chunks" ;
    
    return total_read;
}

void display_queue_info(const BufferQueue<SegBuffer>& buffer_queue) {
    LOG_INFO() << "queue info:" ;
    LOG_INFO() << "chunks: " << buffer_queue.size() ;
    LOG_INFO() << "total size: " << buffer_queue.total_size() / 1024.0 << " KB" ;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        LOG_ERROR() << "usage: " << argv[0] << " <file_path>" ;
        return 1;
    }

    //TODO: get the server address from config file
    config_init("/workspace/Deduplication/config/horse.cfg.json");
    const std::string target_str = horse_cfg.server_address;
    //init logger
    common::Logger::Instance().setLogFile(horse_cfg.log_path, false);
    common::Logger::Instance().setLevel(horse_cfg.log_level);

    auto start_time = std::chrono::high_resolution_clock::now();

    std::string file_path = argv[1];
    BufferQueue<SegBuffer> buffer_queue(100);
    size_t total_read=0; 

    //TODO: create another thread to send the segment to server
    std::thread sender_thread([&buffer_queue, target_str]() {
        const std::string file_path = "/xzs.data";
        StorageClient client(
            grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
        client.FileOpen(file_path,0);
        while (!finished.load() || buffer_queue.size() > 0) {
            std::unique_ptr<SegBuffer> bufferPtr = buffer_queue.dequeue();
            if (bufferPtr->size() > 0) {
                // TODO:Send buffer to server
                if(client.FileWrite(file_path,bufferPtr->data(),bufferPtr->size())==0){
                    LOG_ERROR() << "failed to write buffer to server, exit";
                    return;
                }
            }
        }
        client.FileClose(file_path);
    });

    try {
        if ((total_read=read_file_to_buffer_queue(file_path, buffer_queue)) == 0) {
            LOG_ERROR() << "failed to read file to buffer queue";
            return 1;
        }

        display_queue_info(buffer_queue);

        sender_thread.join();
    }
    catch (const std::exception& e) {
        LOG_ERROR() << "error happened: " << e.what() ;
        return 1;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    double test_duration_sec = std::chrono::duration<double>(
        end_time - start_time
    ).count();

    double bandwidth_mb=(total_read/(1024.0 * 1024.0))/test_duration_sec;
    LOG_INFO() << "test duration: " << test_duration_sec << " seconds";
    LOG_INFO() << "bandwidth: " << bandwidth_mb << " MB/s";

    return 0;
}