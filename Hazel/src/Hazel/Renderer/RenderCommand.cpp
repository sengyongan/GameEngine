#include"hzpch.h"
#include"RenderCommand.h"
#include"Hazel/Platfrom/Opengl/OpenGLRendererAPI.h"
namespace Hazel {
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}