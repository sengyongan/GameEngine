#pragma once
#include"hzpch.h"
#include"Hazel/Utils/PlatformUtils.h"
#include<commdlg.h>
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include"Hazel/Core/Application.h"

namespace Hazel {
    //�ļ�ѡ��Ի���&&�ļ�������
    std::string FileDialogs::OpenFile(const char* filter) {
        OPENFILENAMEA ofn; // ����һ��OPENFILENAME�ṹ�����ofn(�ļ�ѡ��Ի���//���ļ���
        CHAR szFile[260] = { 0 }; // ����һ���ַ�����szFile�����ڴ洢�û�ѡ����ļ�·��
        ZeroMemory(&ofn, sizeof(OPENFILENAME)); // ��ofn�������ڴ�����
        ofn.lStructSize = sizeof(OPENFILENAME); // ����ofn�ṹ��ĳߴ�
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()); // �����ļ�ѡ��Ի���ĸ����ھ��
        ofn.lpstrFile = szFile; // �����ļ�ѡ��Ի������ļ�·���Ļ�����(ָ�룩
        ofn.nMaxFile = sizeof(szFile); // �����ļ�·������������󳤶�
        ofn.lpstrFilter = filter; // �����ļ�ѡ��Ի����е��ļ��������ַ���������filter == hazel������hazel��չ������
        ofn.nFilterIndex = 1; // ����Ĭ��ѡ�е��ļ�����������
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // �����ļ�ѡ��Ի���ı�־λ
        if (GetOpenFileNameA(&ofn) == TRUE) // ��ʾ�ļ�ѡ��Ի��򣬲��ж��û��Ƿ�ѡ�����ļ�
        {
            return ofn.lpstrFile; // ����û�ѡ�����ļ����򷵻��ļ�·��
        }
        return std::string(); // ����û�δѡ���ļ����򷵻ؿ��ַ���
    }
    ////�ļ�ѡ��Ի���&&�ļ�������
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
        if (GetSaveFileNameA(&ofn) == TRUE)//Ψһ����save
        {
            return ofn.lpstrFile;
        }
        return std::string();
    }

}