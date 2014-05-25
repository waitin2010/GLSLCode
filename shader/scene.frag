#version 400

uniform sampler2DShadow depth_texture;
uniform vec3 light_position;
uniform sampler3D offsetTex;
uniform vec3 offsetTexSize;
uniform float radius;

float shiness = 5.0;

out vec4 color;

in vs_fs_interface
{
	vec4 shadow_coord;
	vec3 world_coord;
	vec3 eye_coord;
	vec3 normal;
}vertex;

void main(void)
{

	vec3 n = vertex.normal;
	vec3 l = normalize(light_position - vertex.world_coord);
	vec3 r = reflect(-l, n);
	vec3 e = normalize(vertex.eye_coord);
	float ndotl = dot(n, l);
	float edotr = dot(e, r);
	float diffuse = 0.5;
	float specular = 0.01;
	
	diffuse = max(ndotl,0.0);


	specular = max(pow(edotr,shiness),0.0);

	
	vec3 material_ambient = vec3(0.2,0.2,0.2);
	vec3 material_diffuse = vec3(1.0,1.0,1.0);
	vec3 material_specular = vec3(1.0,1.0,1.0);
	//f = 1.0;

	ivec3 offsetCoord;
	offsetCoord.xy = ivec2(mod(gl_FragCoord.xy, offsetTexSize.xy));

	float sum = 0.0;
	int samplesDiv2 = int(offsetTexSize.z);
	vec4 sc = vertex.shadow_coord;

	for(int i=0; i < 4; i++)
	{
		offsetCoord.z = i;
		vec4 offset = texelFetch(offsetTex,offsetCoord,0) * radius * vertex.shadow_coord.w;

		sc.xy = vertex.shadow_coord.xy + offset.xy;
		sum += textureProj(depth_texture, sc);
		sc.xy = vertex.shadow_coord.xy + offset.zw;
		sum += textureProj(depth_texture,sc);
	}
	float shadow = sum / 8.0;
	#if 1
	if(shadow != 1.0 && shadow != 0.0)
	{
		for(int i=4; i < samplesDiv2; i++) 
		{
			offsetCoord.z = i;
			vec4 offset = texelFetch(offsetTex,offsetCoord,0) * radius * vertex.shadow_coord.w;

			sc.xy = vertex.shadow_coord.xy + offset.xy;
			sum += textureProj(depth_texture, sc);
			sc.xy = vertex.shadow_coord.xy + offset.zw;
			sum += textureProj(depth_texture,sc);
		}
		shadow = sum /(float (samplesDiv2 * 2.0));
	}

	#endif
	float f = shadow;
	vec4 tempcolor = vec4(material_ambient + f * (material_diffuse * diffuse + material_specular * specular), 1.0);
	color = tempcolor;
}