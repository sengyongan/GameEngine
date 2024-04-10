#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TextureFactor;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TextureFactor;

void main(){
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_TextureFactor = a_TextureFactor;
    gl_Position = u_ViewProjection * vec4(a_Position,1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;
in float v_TextureFactor;

uniform sampler2D u_Texture[32];
uniform float u_TextureFactor;
uniform vec4 u_Color;


void main(){
    color = texture(u_Texture[int (v_TexIndex)], v_TexCoord * v_TextureFactor) * v_Color;

}
        
