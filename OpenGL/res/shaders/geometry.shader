#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

out VS_OUT{
    vec3 color;
} vs_out;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.color = aColor;

}

#shader geometry
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
void build_house(vec4 position);
in VS_OUT{
    vec3 color;
} gs_in[];
out vec3 fColor;
void main() 
{
    fColor = gs_in[0].color;
    build_house(gl_in[0].gl_Position);
}
void build_house(vec4 position)
{
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();
    gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);    // 5:顶部
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}
#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 fColor;
void main()
{
    FragColor = vec4(fColor, 1.0);
}


