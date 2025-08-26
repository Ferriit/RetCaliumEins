#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Texture samplers
uniform sampler2D uAlbedo;
uniform sampler2D uNormal;
uniform sampler2D uSpecular;
uniform sampler2D uMetallic;
uniform sampler2D uEmission;

// Simple directional light
uniform vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
uniform vec3 viewPos;

void main()
{
    vec3 albedo   = texture(uAlbedo, TexCoord).rgb;
    vec3 normal   = texture(uNormal, TexCoord).rgb * 2.0 - 1.0; // remap [0,1] -> [-1,1]
    vec3 specular = texture(uSpecular, TexCoord).rgb;
    vec3 emission = texture(uEmission, TexCoord).rgb;

    // Simple diffuse
    float diff = max(dot(normalize(normal), -lightDir), 0.0);

    // Simple specular (Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, normalize(normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 color = albedo * diff + specular * spec + emission;
    FragColor = vec4(color, 1.0);
}