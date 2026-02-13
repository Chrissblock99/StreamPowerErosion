#version 450 core

#ifdef VERTEX_SHADER
void main(void)
{
	vec3 vertices[4] = vec3[4](
		vec3( 0.0, 0.0,  0.0),
		vec3(64.0, 0.0,  0.0),
		vec3( 0.0, 0.0, 64.0),
		vec3(64.0, 0.0, 64.0)
	);

	vec3 pos[16] = vec3[16](
		vec3(0.0, 0.0, 0.0),
		vec3(1.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(3.0, 0.0, 0.0),
		
		vec3(0.0, 0.0, 1.0),
		vec3(1.0, 0.0, 1.0),
		vec3(2.0, 0.0, 1.0),
		vec3(3.0, 0.0, 1.0),
		
		vec3(0.0, 0.0, 2.0),
		vec3(1.0, 0.0, 2.0),
		vec3(2.0, 0.0, 2.0),
		vec3(3.0, 0.0, 2.0),
		
		vec3(0.0, 0.0, 3.0),
		vec3(1.0, 0.0, 3.0),
		vec3(2.0, 0.0, 3.0),
		vec3(3.0, 0.0, 3.0)
	);

	vec3 offset = vertices[gl_VertexID % 4];
	vec3 src = pos[gl_VertexID / 4];

	vec3 position = src*64 - 128 + offset;

    gl_Position = vec4(position, 1.0);
} 
#endif

#ifdef TESSELATION_CONTROL
layout (vertices=4) out;

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = 64;
        gl_TessLevelOuter[1] = 64;
        gl_TessLevelOuter[2] = 64;
        gl_TessLevelOuter[3] = 64;

        gl_TessLevelInner[0] = 64;
        gl_TessLevelInner[1] = 64;
    }
}
#endif

#ifdef EVALUATION_CONTROL
layout (quads, equal_spacing, ccw) in;
layout(binding = 0, r32f) uniform image2D bedrockMap;

void main() {
    vec2 uv = gl_TessCoord.xy;

    vec2 p00 = gl_in[0].gl_Position.xz;
    vec2 p11 = gl_in[3].gl_Position.xz;

    vec2 position = (p11 - p00) * uv + p00;
    ivec2 texPosition = ivec2(position)+128;

    float height = imageLoad(bedrockMap, texPosition).x/1176.47;

    gl_Position =  vec4(position.x, height, position.y, 1);
}
#endif


#ifdef GEOMETRY_SHADER
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec3 cameraPos;

out vec3 normal;

const vec3 upDir = vec3(0, 1, 0);
const vec3 target = vec3(0);

void main() {
	vec3 camDir = normalize(cameraPos - target);
	vec3 camRight = normalize(cross(upDir, camDir));
	vec3 camUp = cross(camDir, camRight);

	mat4 transformMatrix = mat4(0);
	transformMatrix[3].w = 1;
	transformMatrix[0].xyz = camRight;
	transformMatrix[1].xyz = camUp;
	transformMatrix[2].xyz = camDir;

    vec3 constNormal = normalize(cross(gl_in[1].gl_Position.xyz-gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz));

    for (int i = 0; i < 3; i++) {
        vec3 pos = gl_in[i].gl_Position.xyz;
        gl_Position = transformMatrix * vec4(pos / length(cameraPos), 1);
        normal = constNormal;
        EmitVertex();
    }
    EndPrimitive();
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 normal;

out vec4 FragColor;

const vec3 lightDir = normalize(vec3(1, -1, 1));

void main(void) {
    float angle0to1 = acos(normal.y)/3.1415926535*.5;
    vec3 color;
	if (angle0to1 < 0.4)
        color = vec3(0.375);
    else
        color = vec3(.31, .47, .22);


    float diffuse = dot(normal, lightDir);
    diffuse = diffuse*.5+.5;


    float specular = 0;


    FragColor = vec4(color * diffuse, 1.0);
}
#endif
