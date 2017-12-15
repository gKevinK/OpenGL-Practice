#version 430 core
layout(location = 0) in vec2 aPos;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 viewPos;

void main()
{
    vec3 pos = vec3(aPos.x, 0.0, -aPos.y);
    if (length(aPos) > 8000.0) {
        gl_Position = (proj * mat4(mat3(view)) * vec4(pos, 1.0)).xyww;
    } else {
        pos.xz = pos.xz + viewPos.xz;
        gl_Position = proj * view * vec4(pos, 1.0);
    }

    FragPos = vec3(pos.x, -1.0, pos.z);
    Normal = vec3(0.0, 1.0, 0.0);
}
