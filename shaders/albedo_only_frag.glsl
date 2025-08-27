#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Texture sampler
uniform sampler2D uAlbedo;

// Directional light (in world space)
uniform vec3 lightDir = normalize(vec3(0, 0, 5));  // set this in C++
uniform vec3 viewPos;   // camera position in world space

void main()
{
    // Base color
    vec3 albedo = texture(uAlbedo, TexCoord).rgb;

    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDirN = normalize(-lightDir); // make sure light points toward the surface
    vec3 viewDir = normalize(viewPos - FragPos);

    // Diffuse shading (Lambert)
    float diff = max(dot(norm, lightDirN), 0.0);

    // Specular shading (Phong)
    vec3 reflectDir = reflect(-lightDirN, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // Combine (simple coefficients)
    vec3 diffuse  = diff * albedo;
    vec3 specular = spec * vec3(0.5); // constant white specular highlight

    vec3 result = diffuse + specular;
    FragColor = vec4(result, 1.0);
}
