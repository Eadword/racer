#version 410

layout(location=1) in vec4 vPosition;
layout(location=2) in vec4 vNormal;
layout(location=4) in vec2 tex_coord;

uniform mat4 proj; // Projection
uniform mat4 view; // worldtoview
uniform mat4 obj;  // position * objtowld * transform

out vec2 itex_coord;

out vec3 normal;
out vec3 tangent;
out vec3 eyepos;

void main() {
    itex_coord = tex_coord;
    mat4 toeye = view * obj;
    eyepos = (toeye * vPosition).xyz;
    mat3 normal_matrix = mat3(toeye[0].xyz, toeye[1].xyz, toeye[2].xyz);
    normal = normal_matrix * vNormal.xyz;

    //this should be calculated and stored in VAO for objects other than the road
    //But since it is just the road, I have arbatrially lined it up with world coords
    tangent = normal_matrix * vec3(1, 0, 0);

    gl_Position = proj * view * obj * vPosition;
}
