#version 140

in vec3 position;
in vec3 normal;
in vec2 texCoord;
uniform mat4 normalMatrix; // inverse transposed VMmatrix
uniform mat4 PVMmatrix;
uniform mat4 Vmatrix; // View                       --> world to eye coordinates
uniform mat4 Mmatrix; // Model                      --> model to world coordinates
smooth out vec2 texCoord_v;
smooth out vec3 normal_v;
smooth out vec3 position_v;


void main(){
    gl_Position = PVMmatrix * vec4(position, 1); // out:v vertex in clip coordinates

    normal_v = normalize((normalMatrix * vec4(normal, 0.0)).xyz); // normal in eye coordinates by NormalMatrix
    texCoord_v = texCoord;
    position_v = (Vmatrix * Mmatrix * vec4(position, 1)).xyz;

}
