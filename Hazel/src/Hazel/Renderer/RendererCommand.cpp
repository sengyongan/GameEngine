#include"hzpch.h"
#include"RendererCommand.h"
#include"Hazel/Platfrom/Opengl/OpenGLRendererAPI.h"
namespace Hazel {
    RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;
}