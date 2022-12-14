#version 140

uniform float time; // used for simulation of moving lights (such as sun) and to select proper animation frame
uniform mat4 Vmatrix; // view (camera) transform --> world to eye coordinates
uniform sampler2D texSampler; // sampler for texture access
smooth in vec3 position_v; // camera space fragment position
smooth in vec2 texCoord_v; // fragment texture coordinates
out vec4 color_f; // outgoing fragment color

// there are 4 frames in the row, two rows total
uniform ivec2 pattern = ivec2(8, 2);

uniform float frameDuration;

vec4 sampleTexture(int frame){
    vec2 offset = vec2(1.0f) / vec2(pattern);
    vec2 texCoordBase = texCoord_v / vec2(pattern);
    vec2 texCoord = texCoordBase + vec2(frame % pattern.x, (frame / pattern.x)) * offset;
    return texture(texSampler, texCoord);
}

void main(){
    // frame of the texture to be used for fire drawing
    int frame = int(time / frameDuration);
    // sample proper frame of the texture to get a fragment color
    color_f = sampleTexture(frame);
}