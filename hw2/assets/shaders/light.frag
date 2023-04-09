#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight {
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

void main() {
    vec4 dAmbient = vec4(0.0);
    vec4 dDiffuse = vec4(0.0);
    vec4 dSpecular = vec4(0.0);
    vec4 pAmbient = vec4(0.0);
    vec4 pDiffuse = vec4(0.0);
    vec4 pSpecular = vec4(0.0);
    vec4 sAmbient = vec4(0.0);
    vec4 sDiffuse = vec4(0.0);
    vec4 sSpecular = vec4(0.0);
    vec4 cAmbient = vec4(0.0);
    vec4 cDiffuse = vec4(0.0);
    vec4 cSpecular = vec4(0.0);
    
    if (dl.enable == 1) { // directional light
        // ambient
        dAmbient = vec4(dl.lightColor * material.ambient, 1.0);

        //diffuse
        float ddCoef = dot(normalize((-1) * dl.direction), normalize(Normal));
        dDiffuse = vec4(dl.lightColor * material.diffuse, 1.0) * max(ddCoef, 0.0);
        
        // specular
        vec3 reflect_dir = reflect(dl.direction, normalize(Normal));
        vec3 view_dir = viewPos - FragPos;
        float dsCoef = dot(normalize(reflect_dir), normalize(view_dir));
        dSpecular = vec4(dl.lightColor * material.specular, 1.0) * pow(max(dsCoef, 0.0), material.shininess);
    }

    if (pl.enable == 1) { // point light
        // attenuation
        float dist = length(FragPos - pl.position);
        float attenuation = 1.0 / (pl.constant + pl.linear * dist + pl.quadratic * dist * dist);

        // ambient
        pAmbient = vec4(pl.lightColor * material.ambient, 1.0) * attenuation;

        // diffuse
        vec3 light_dir = FragPos - pl.position;
        float pdCoef = dot(normalize((-1) * light_dir), normalize(Normal));
        pDiffuse = vec4(pl.lightColor * material.diffuse, 1.0) * max(pdCoef, 0.0) * attenuation;

        // specular
        vec3 reflect_dir = reflect(light_dir, normalize(Normal));
        vec3 view_dir = viewPos - FragPos;
        float psCoef = dot(normalize(reflect_dir), normalize(view_dir));
        pSpecular = vec4(pl.lightColor * material.specular, 1.0) * pow(max(psCoef, 0.0), material.shininess) * attenuation;
    }

    if (sl.enable == 1) {
        // attenuation
        float dist = length(FragPos - sl.position);
        float attenuation = 1.0 / (sl.constant + sl.linear * dist + sl.quadratic * dist * dist);

        // ambient
        sAmbient = vec4(sl.lightColor * material.ambient, 1.0) * attenuation;

        // check if position is in the spotlight
        vec3 light_dir = FragPos - sl.position;
        if (dot(normalize(light_dir), normalize(sl.direction)) > sl.cutOff) {
            // diffuse
            float sdCoef = dot(normalize((-1) * light_dir), normalize(Normal));
            sDiffuse = vec4(sl.lightColor * material.diffuse, 1.0) * max(sdCoef, 0.0) * attenuation;

            // specular
            vec3 reflect_dir = reflect(light_dir, normalize(Normal));
            vec3 view_dir = viewPos - FragPos;
            float ssCoef = dot(normalize(reflect_dir), normalize(view_dir));
            sSpecular = vec4(sl.lightColor * material.specular, 1.0) * pow(max(ssCoef, 0.0), material.shininess) * attenuation;
        }
    }

    cAmbient = dAmbient + pAmbient + sAmbient;
    cDiffuse = dDiffuse + pDiffuse + sDiffuse;
    cSpecular = dSpecular + pSpecular + sSpecular;
    color = (cAmbient + cDiffuse + cSpecular) * texture(ourTexture, TexCoord);
}
