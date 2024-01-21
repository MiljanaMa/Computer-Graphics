#version 330 core

struct PositionalLight {
	vec3 Position;
	vec3 Ka; //ambinet
	vec3 Kd; //difuse
	vec3 Ks; //specular
	//attenuation, za slabljenje svjetla u daljini
	float Kc; //constant
	float Kl; //linear
	float Kq; //quadratic
};
//reflektorsko svjetlo, spotlight
struct SpotLight {
	vec3 Position;
	vec3 Direction; //usmjereno
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float InnerCutOff;
	float OuterCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct DirectionLight {
	vec3 Direction; //usmjereno
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

struct Material {
	sampler2D Kd;  //difuse
	sampler2D Ks;  //specular
	float Shininess; //jacina
};

uniform DirectionLight uSunLight;
uniform DirectionLight uMoonLight;
uniform PositionalLight uFireLight;

uniform SpotLight uReflectorLight;

uniform Material uMaterial;
uniform vec3 uViewPos;

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

out vec4 FragColor;

void main() {
	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);

	// Sun Light
	vec3 SunLightVector = normalize(-uSunLight.Direction);
	vec3 halfwayDir = normalize(SunLightVector + ViewDirection);
	float SunDiffuse = max(dot(vWorldSpaceNormal, SunLightVector), 0.0f);
	vec3 SunReflectDirection = reflect(-SunLightVector, vWorldSpaceNormal);
	float SunSpecular = pow(max(dot(ViewDirection, SunReflectDirection), 0.0f), uMaterial.Shininess);
	float spec = pow(max(dot(vWorldSpaceNormal, halfwayDir), 0.0), uMaterial.Shininess);
	vec3 SunAmbientColor = uSunLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SunDiffuseColor = uSunLight.Kd * SunDiffuse * vec3(texture(uMaterial.Kd, UV));
	vec3 SunSpecularColor = uSunLight.Ks * spec * vec3(texture(uMaterial.Ks, UV));
	vec3 SunColor = SunAmbientColor + SunDiffuseColor + SunSpecularColor;

	//Moon Light
	vec3 MoonLightVector = normalize(-uMoonLight.Direction);
	halfwayDir = normalize(SunLightVector + ViewDirection);
	float MoonDiffuse = max(dot(vWorldSpaceNormal, MoonLightVector), 0.0f);
	vec3 MoonReflectDirection = reflect(-MoonLightVector, vWorldSpaceNormal);
	float MoonSpecular = pow(max(dot(ViewDirection, MoonReflectDirection), 0.0f), uMaterial.Shininess);
	spec = pow(max(dot(vWorldSpaceNormal, halfwayDir), 0.0), uMaterial.Shininess);
	vec3 MoonAmbientColor = uMoonLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 MoonDiffuseColor = uMoonLight.Kd * MoonDiffuse * vec3(texture(uMaterial.Kd, UV));
	vec3 MoonSpecularColor = uMoonLight.Ks * spec * vec3(texture(uMaterial.Ks, UV));
	vec3 MoonColor = MoonAmbientColor + MoonDiffuseColor + MoonSpecularColor;

	// fire
	vec3 PtLightVector1 = normalize(uFireLight.Position - vWorldSpaceFragment);
	halfwayDir = normalize(PtLightVector1 + ViewDirection);
	float PtDiffuse1 = max(dot(vWorldSpaceNormal, PtLightVector1), 0.0f);
	vec3 PtReflectDirection1 = reflect(-PtLightVector1, vWorldSpaceNormal);
	spec = pow(max(dot(vWorldSpaceNormal, halfwayDir), 0.0), uMaterial.Shininess);

	vec3 PtAmbientColor1 = uFireLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 PtDiffuseColor1 = PtDiffuse1 * uFireLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 PtSpecularColor1 = spec * uFireLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float PtLightDistance1 = length(uFireLight.Position - vWorldSpaceFragment);
	float PtAttenuation1 = 1.0f / (uFireLight.Kc + uFireLight.Kl * PtLightDistance1 + uFireLight.Kq * (PtLightDistance1 * PtLightDistance1));
	vec3 PtColorFire = PtAttenuation1 * (PtAmbientColor1 + PtDiffuseColor1 + PtSpecularColor1);

	//Reflector 
	vec3 SpotlightVector = normalize(uReflectorLight.Position - vWorldSpaceFragment);
	halfwayDir = normalize(SpotlightVector + ViewDirection);
	float SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
	vec3 SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
	spec = pow(max(dot(vWorldSpaceNormal, halfwayDir), 0.0), 16.0);//uMaterial.Shininess);
	vec3 SpotAmbientColor = uReflectorLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotDiffuseColor = SpotDiffuse * uReflectorLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotSpecularColor = spec * uReflectorLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float SpotlightDistance = length(uReflectorLight.Position - vWorldSpaceFragment);
	float SpotAttenuation = 1.0f / (uReflectorLight.Kc + uReflectorLight.Kl * SpotlightDistance + uReflectorLight.Kq * (SpotlightDistance * SpotlightDistance));

	float Theta = dot(SpotlightVector, normalize(-uReflectorLight.Direction));
	float Epsilon = uReflectorLight.InnerCutOff - uReflectorLight.OuterCutOff;
	float SpotIntensity = clamp((Theta - uReflectorLight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
	vec3 SpotColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);

	vec3 FinalColor = MoonColor + SunColor + SpotColor + PtColorFire;
	FragColor = vec4(FinalColor, 0.5f);
}