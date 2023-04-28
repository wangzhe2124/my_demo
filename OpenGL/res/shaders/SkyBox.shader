#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 view;

out vec3 fragTexcoord;
void main()
{
	gl_Position = (projection * view  * vec4(position, 1.0f)).xyww;
	fragTexcoord = position;

};
#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 fragTexcoord;
uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, fragTexcoord); 
}