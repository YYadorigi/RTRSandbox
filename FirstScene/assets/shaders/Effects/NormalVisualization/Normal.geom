#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT 
{
	vec3 normal;
} gs_in[];

uniform float normalLength = 0.2;
layout (std140) uniform VPMatrices
{
	mat4 view;
	mat4 projection;
};

void main()
{
	for (int i = 0; i < 3; i++)
	{
		vec4 position = gl_in[i].gl_Position;
		vec4 normal = normalize(projection * view * vec4(gs_in[i].normal, 0.0));	// Convert normal to clip space
		
		gl_Position = position;
		EmitVertex();

		gl_Position = position + normalLength * normal;
		EmitVertex();

		EndPrimitive();
	}
}