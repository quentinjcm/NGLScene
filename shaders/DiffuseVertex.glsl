#version 410 core

layout(location = 0) in vec3 inVert;
layout(location = 2) in vec3 inNormal;

uniform mat4 MVP;
out vec3 fragmentNormal;

void main()
{
  fragmentNormal = inNormal;
  gl_Position = MVP*vec4(inVert, 1.0);
}
