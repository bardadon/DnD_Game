#pragma once
#include <vector>
#include <windows.h>
#include <thread>

struct RenderEntity
{
    int x;
    int y;
    char symbol; // 'P', 'M', '.', '#', etc.
};

struct RenderFrame
{
    int width;
    int height;
    int entityCount;
    std::vector<RenderEntity> entities;
};

struct RenderFrameHeader
{
    int width;
    int height;
    int entityCount;
};


//class RenderPipeClient
//{
//public:
//    bool Connect()
//    {
//        while (true)
//        {
//            m_hPipe = CreateFileA(
//                "\\\\.\\pipe\\DnD_RenderPipe",
//                GENERIC_WRITE,
//                0,
//                nullptr,
//                OPEN_EXISTING,
//                0,
//                nullptr
//            );
//
//            if (m_hPipe != INVALID_HANDLE_VALUE)
//                return true;
//
//            std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        }
//    }
//
//    void SendFrame(const RenderFrame& frame, const std::vector<RenderEntity>& entities)
//    {
//        if (m_hPipe == INVALID_HANDLE_VALUE)
//            return;
//
//        DWORD written;
//
//        // Send header
//        WriteFile(m_hPipe, &frame, sizeof(RenderFrame), &written, nullptr);
//
//        // Send entity array
//        if (!entities.empty())
//        {
//            WriteFile(
//                m_hPipe,
//                entities.data(),
//                sizeof(RenderEntity) * entities.size(),
//                &written,
//                nullptr
//            );
//        }
//    }
//
//private:
//    HANDLE m_hPipe = INVALID_HANDLE_VALUE;
//};