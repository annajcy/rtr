#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPosition;

struct Material {
    vec3 ka;  
    vec3 kd; 
    vec3 ks; 
    float shininess;
}; 

uniform Material material;

#define MAX_LIGHTS 16
uniform int activeLights;
uniform int mainLightIndex;

struct Light {
    int type;        // 0=ambient, 1=directional, 2=point, 3=spot
    vec3 color;
    float intensity;
    vec3 position;   
    vec3 direction;  
    vec3 attenuation; // x=kc, y=k1, z=k2
    float innerAngle;
    float outerAngle;
};

uniform Light lights[MAX_LIGHTS];

uniform sampler2D mainTexture;

vec3 diffuseComponent(vec3 lightDir, vec3 normal, vec3 lightColor, vec3 objectColor, float intensity) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightColor * objectColor * intensity;
}

vec3 specularComponent(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 lightColor, vec3 objectColor, float shininess, float intensity) {
	vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0.0), shininess);
    return spec * lightColor * objectColor * intensity;
}

vec3 ambientComponent(vec3 lightColor, vec3 objectColor, float intensity) {
    return lightColor * objectColor * intensity;
}

vec3 calculateAmbientLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    return mat.ka * ambientComponent(light.color, objectColor, light.intensity);
}

vec3 calculateDirectionalLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 diffuse = mat.kd * diffuseComponent(lightDir, normal, light.color, objectColor, light.intensity);
    vec3 specular = mat.ks * specularComponent(lightDir, normal, viewDir, light.color, objectColor, mat.shininess, light.intensity);
    return diffuse + specular;
}

vec3 calculatePointLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(cameraPos - fragPos);
    float kc = light.attenuation.x; 
    float k1 = light.attenuation.y;
    float k2 = light.attenuation.z;
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (kc + k1 * dist + k2 * dist * dist);
    vec3 diffuse = mat.kd * diffuseComponent(lightDir, normal, light.color, objectColor, light.intensity);
    vec3 specular = mat.ks * specularComponent(lightDir, normal, viewDir, light.color, objectColor, mat.shininess, light.intensity);
    return (diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 targetDir = normalize(-light.direction);
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 diffuse = mat.kd * diffuseComponent(lightDir, normal, light.color, objectColor, light.intensity);
    vec3 specular = mat.ks * specularComponent(lightDir, normal, viewDir, light.color, objectColor, mat.shininess, light.intensity);
    float theta = dot(targetDir, lightDir);
    float attenuation = clamp((theta - light.outerAngle) / (light.innerAngle - light.outerAngle), 0.0, 1.0);
    return (diffuse + specular) * attenuation;
}
 
void main()
{
    vec4 texColor = texture(mainTexture, TexCoord);
    vec3 objectColor = texColor.rgb;
    vec3 N = normalize(Normal);

    vec3 result = vec3(0.0);

    for (int i = 0; i < activeLights; i++) {
        Light light = lights[i];

        if (light.type == 0) {
            result += calculateAmbientLight(light, material, N, cameraPosition, FragPos, objectColor);
        } else if (light.type == 1) {
            result += calculateDirectionalLight(light, material, N, cameraPosition, FragPos, objectColor);
        } else if (light.type == 2) {
            result += calculatePointLight(light, material, N, cameraPosition, FragPos, objectColor);
        } else if (light.type == 3) {
            result += calculateSpotLight(light, material, N, cameraPosition, FragPos, objectColor);
        }
    }

    FragColor = vec4(result, 1.0);
}