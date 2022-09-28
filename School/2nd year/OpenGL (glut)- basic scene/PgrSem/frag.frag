#version 140

struct Material { // structure that describes currently used material
    vec3 ambient; // ambient component
    vec3 diffuse; // diffuse component
    vec3 specular; // specular component
    float shininess; // sharpness of specular reflection
    bool useTexture; // defines whether the texture is used or not
};

struct Light { // structure describing light parameters
    vec3 ambient; // intensity & color of the ambient component
    vec3 diffuse; // intensity & color of the diffuse component
    vec3 specular; // intensity & color of the specular component
    vec3 position; // light position
    vec3 spotDirection; // spotlight direction
    float spotCosCutOff; // cosine of the spotlight's half angle
    float spotExponent; // distribution of the light energy within the reflector's cone (center->cone's edge)
};

uniform sampler2D texSampler; // sampler for the texture access
uniform Material material; // current material
uniform float time; // time used for simulation of moving lights (such as sun)
uniform bool fogOn;

uniform mat4 PVMmatrix; // Projection * View * Model  (model to clip coordinates)
uniform mat4 Vmatrix; // View                       (world to eye coordinates)
uniform mat4 Mmatrix; // Model                      (model to world coordinates)
uniform mat4 normalMatrix; // inverse transposed VMmatrix

smooth in vec2 texCoord_v; // fragment texture coordinates
smooth in vec3 normal_v; //camera space normal
smooth in vec3 position_v; // camera space position
out vec4 color_f; // outgoing fragment color


Light sun;
float sunSpeed = 0.25f;
uniform bool sunOn;

Light pointLighter;
uniform vec4 positionPointLight;
uniform bool pointLightOn;
uniform bool flashOn;

Light reflector;
uniform vec3 reflectorPosition;
uniform vec3 reflectorDirection;
float fogFactor;

void setupLights(){
	float fogDist = abs(position_v.z);
	fogFactor = clamp(
		(3.0f - fogDist) / (3.0f - 0.0), 0.0, 1.0
	);
    // set up sun parameters
    sun.ambient = vec3(0.5f);
    sun.diffuse = vec3(0.5f, 0.5f, 0.5f);
    sun.specular = vec3(0.2f);

    sun.position = (Vmatrix * vec4(cos(time * sunSpeed), 0.0, sin(time * sunSpeed), 0.0)).xyz;

    if (pointLightOn == true) {
        pointLighter.ambient = vec3(0.1f);
        pointLighter.diffuse = vec3(0.2f);
        pointLighter.specular = vec3(0.1f);
        pointLighter.position = (Vmatrix * positionPointLight).xyz;
    }

    // set up reflector parameters
    reflector.ambient = vec3(0.2f);
    reflector.diffuse = vec3(1.0f);
    reflector.specular = vec3(1.0f);
    reflector.spotCosCutOff = 0.95f;
    reflector.spotExponent = 5.0;
    reflector.position = (Vmatrix * vec4(reflectorPosition, 1.0)).xyz;
    reflector.spotDirection = normalize((Vmatrix * vec4(reflectorDirection, 0.0f))).xyz;
}

vec4 directionalLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal){
    vec3 ret = vec3(0.0f);

    vec3 L = normalize(light.position.xyz);
    vec3 R = reflect(-L, vertexNormal);
    vec3 V = normalize(-vertexPosition);

    ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * max(0.0f, dot(vertexNormal, L));
    ret += material.specular * light.specular * pow(max(0.0f, dot(R, V)), material.shininess);
    return vec4(ret, 1.0f);
}

vec4 pointLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal){
    vec3 ret = vec3(0.0f);

	float distance = length(light.position - vertexPosition.xyz);
    
	vec3 L = normalize(light.position.xyz - vertexPosition);
    vec3 R = reflect(-L, vertexNormal);
    vec3 V = normalize(-vertexPosition);

    ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * max(0.0f, dot(vertexNormal, L));
    ret += material.specular * light.specular * pow(max(0.0f, dot(R, V)), material.shininess);
	ret /= distance*distance*20;
    return vec4(ret, 1.0f);
}

vec4 spotLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal){

    vec3 ret = vec3(0.0);

    vec3 L = normalize(light.position.xyz - vertexPosition);
    vec3 R = reflect(-L, vertexNormal);
    vec3 V = normalize(-vertexPosition);
    float NdotL = max(0.0, dot(vertexNormal, L));
    float RdotV = max(0.0, dot(R, V));
    float spotCoef = max(0.0, dot(-L, light.spotDirection));

    ret += material.ambient * light.ambient;
    ret += material.diffuse * light.diffuse * NdotL;
    ret += material.specular * light.specular * pow(RdotV, material.shininess);

    if (spotCoef < light.spotCosCutOff)
        ret *= 0.0;
    else
        ret *= pow(spotCoef, light.spotExponent);

    return vec4(ret, 1.0);
}

vec3 fogColor = vec3(0.65f, 0.65f, 0.65f);

void main(){

    setupLights();

    vec3 normal = normalize(normal_v);
    //basic ambient light
    vec3 globalAmbientLight = vec3(0.2f);
    vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0f);
    //sun
    if (sunOn)
        outputColor += directionalLight(sun, material, position_v, normal);

    color_f = outputColor;

    // texture
    if (material.useTexture)
        color_f = outputColor * texture(texSampler, texCoord_v);

    if (fogOn) {
	  color_f = vec4(mix(fogColor, color_f.rgb, fogFactor), color_f.w);
    }
        //reflector
    if (flashOn)
        color_f += spotLight(reflector, material, position_v, normal);
    //fire
    if (pointLightOn)
        color_f += pointLight(pointLighter, material, position_v, normal);
}
