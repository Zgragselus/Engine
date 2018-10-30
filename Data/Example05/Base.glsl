[vertex shader]
#version 420 core

layout(location=0) in vec3 Sc_Vertex;
layout(location=1) in vec3 Sc_Normal;
layout(location=2) in vec3 Sc_Tangent;
layout(location=3) in vec3 Sc_Bitangent;
layout(location=4) in vec2 Sc_TexCoord0;
layout(location=5) in vec2 Sc_TexCoord1;

uniform mat4 Sc_World;
uniform mat4 Sc_View;
uniform mat4 Sc_Projection;

out vec3 Normal;

void main()
{
	Normal = vec3(Sc_Normal.xyz);

	gl_Position = Sc_Projection * (Sc_View * (Sc_World * vec4(Sc_Vertex.xyz, 1.0)));
}

[fragment shader]
#version 420 core

in vec3 Normal;

void main()
{
	vec3 n = normalize(Normal);

	gl_FragColor = vec4(n, 1.0);
}
