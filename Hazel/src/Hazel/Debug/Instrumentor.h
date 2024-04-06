#pragma once
//���ڲ�������ε�ִ��ʱ��
#include <string>
#include <chrono>//���ڴ���ʱ��
#include <algorithm>
#include <fstream>

#include <thread>//���ڶ��̱߳��

namespace Hazel {
    struct ProfileResult//�洢���ܷ����Ľ��
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession//�洢һ�����ܷ����Ự����Ϣ
    {
        std::string Name;
    };

    class Instrumentor//�Ǳ��������ܷ����ĻỰ�ͽ��
    {
    private:
        InstrumentationSession* m_CurrentSession;//��ǰ�Ự
        std::ofstream m_OutputStream;//�����
        int m_ProfileCount;//��������
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")//��ʼ�Ự��
        {
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = new InstrumentationSession{ name };//�¼�ʱ��
        }

        void EndSession()//�����Ự
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)//д�������ļ�����ʱ��Stopʱ����
        {
            if (m_ProfileCount++ > 0)//�ָ�
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }

        void WriteHeader()//д����
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter()//д��ע
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }
    };

    class InstrumentationTimer//�Ǳ��ʱ���������������ʱ�Զ�ֹͣ��ʱ������¼���
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

            m_Stopped = true;
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}

#define HZ_PROFILE 1
#if HZ_PROFILE//���������ļ�
#define HZ_PROFILE_BEGIN_SESSION(name, filepath) ::Hazel::Instrumentor::Get().BeginSession(name, filepath)//�����ļ���ʼ�Ự
#define HZ_PROFILE_END_SESSION() ::Hazel::Instrumentor::Get().EndSession()//�����ļ������Ự
#define HZ_PROFILE_SCOPE(name) ::Hazel::InstrumentationTimer timer##__LINE__(name);//��������//__LINE__���ڻ�ȡ��ǰԴ�����к� __LINE__ �滻Ϊ��ǰԴ�����е��к�
#define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(__FUNCSIG__)//�����ļ�����//__FUNCSIG__���ڻ�ȡ��ǰ������ǩ��funcsig
#else
#define HZ_PROFILE_BEGIN_SESSION(name, filepath)
#define HZ_PROFILE_END_SESSION()
#define HZ_PROFILE_SCOPE(name)
#define HZ_PROFILE_FUNCTION()
#endif

