#pragma once
#include"hzpch.h"
#include"Hazel/Utils/PlatformUtils.h"
#include<commdlg.h>
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include"Hazel/Core/Application.h"

namespace Hazel {
    //文件选择对话框&&文件过滤器
    std::string FileDialogs::OpenFile(const char* filter) {
        OPENFILENAMEA ofn; // 定义一个OPENFILENAME结构体变量ofn(文件选择对话框）//打开文件名
        CHAR szFile[260] = { 0 }; // 定义一个字符数组szFile，用于存储用户选择的文件路径
        ZeroMemory(&ofn, sizeof(OPENFILENAME)); // 将ofn变量的内存清零
        ofn.lStructSize = sizeof(OPENFILENAME); // 设置ofn结构体的尺寸
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()); // 设置文件选择对话框的父窗口句柄
        ofn.lpstrFile = szFile; // 设置文件选择对话框中文件路径的缓冲区(指针）
        ofn.nMaxFile = sizeof(szFile); // 设置文件路径缓冲区的最大长度
        ofn.lpstrFilter = filter; // 设置文件选择对话框中的文件过滤器字符串（传入filter == hazel，过滤hazel扩展名，）
        ofn.nFilterIndex = 1; // 设置默认选中的文件过滤器索引
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // 设置文件选择对话框的标志位
        if (GetOpenFileNameA(&ofn) == TRUE) // 显示文件选择对话框，并判断用户是否选择了文件
        {
            return ofn.lpstrFile; // 如果用户选择了文件，则返回文件路径
        }
        return std::string(); // 如果用户未选择文件，则返回空字符串
    }
    ////文件选择对话框&&文件过滤器
    std::string FileDialogs::SaveFile(const char* filter) {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetSaveFileNameA(&ofn) == TRUE)//唯一区别save
        {
            return ofn.lpstrFile;
        }
        return std::string();
    }

}