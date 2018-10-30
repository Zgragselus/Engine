[vertex shader]
#version 420 core

layout(location=0) in vec4 Sc_Vertex;

out vec2 coord;

void main()
{
	coord.xy = Sc_Vertex.xy;
	gl_Position = vec4(Sc_Vertex.xy, 0.0, 1.0);
}

[fragment shader]
#version 420 core

in vec2 coord;

void main()
{
	gl_FragColor = vec4(coord.x * 0.5 + 0.5, 
                        coord.y * 0.5 + 0.5, 
                        1.0 - (coord.x * 0.5 + 0.5 + coord.y * 0.5 + 0.5), 
                        1.0);
}
