#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;
uniform vec3 cameraPos;
uniform float waterHeight;

in vec2 texCoord[];
out vec2 oTexCoord;
out float diffTerm;
out float specTerm;
out float height;

void main()
{
	vec3 vect1 = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec3 vect2 = gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec4 normal = vec4(normalize(cross(vect1, vect2)), 0);

	for(int i = 0; i < 3; i++)
	{	
		vec4 p = gl_in[i].gl_Position;

		vec4 camPos    = mvMatrix * p;
		vec4 lgtvec    = normalize(lightPos - camPos);
		vec4 viewVec   = normalize(vec4(-cameraPos.xyz, 0));
		vec4 halfVec   = normalize(lgtvec + viewVec); 
		vec4 normalCam = norMatrix * normal;

		diffTerm = max(dot(lgtvec, normalCam), 0);
		specTerm = max(dot(halfVec, normalCam), 0);

		oTexCoord = texCoord[i];
		height = p.y;
		gl_Position = mvpMatrix * p;
		EmitVertex();
	}

	EndPrimitive();
}