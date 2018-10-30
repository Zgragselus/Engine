[vertex shader]
#version 420 core

layout(location=0) in vec4 Sc_Vertex;

void main()
{
	gl_Position = vec4(Sc_Vertex.xy, 0.0, 1.0);
}

[fragment shader]
#version 420 core

uniform vec4 color;

void main()
{
	gl_FragColor = color;
}
