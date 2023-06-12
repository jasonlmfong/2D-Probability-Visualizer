#version 460 core

layout(location = 0) out vec4 color;

in vec3 pos;
in vec4 view_pos;
in vec3 view_pos_normal;
in vec4 light_pos;

vec3 getRGB(float height)
{
	vec3 rgb;
    double hh, ff, p, q, t;
    
	float sat = 0.9;
	float val = 0.9;
    
    hh = 240 - height * 5 * 12;
    hh /= 60.0;
    int i = int(hh);
    ff = hh - i;
    p = val * (1.0 - sat);
    q = val * (1.0 - (sat * ff));
    t = val * (1.0 - (sat * (1.0 - ff)));

    switch(i) {
    case 0:
        rgb = vec3(val, t, p);
        break;
    case 1:
        rgb = vec3(q, val, p);
        break;
    case 2:
        rgb = vec3(p, val, t);
        break;

    case 3:
        rgb = vec3(p, q, val);
        break;
    case 4:
		rgb = vec3(t, p, val);
        break;
    case 5:
    default:
        rgb = vec3(val, p, q);
        break;
    }
    return rgb;
};


void main()
{
	vec3 ka; // ambient constant
	vec3 kd; // diffuse constant
	vec3 ks; // specular constnat
	float p = 50.0f; // phong exponent
	vec3 n = normalize(view_pos_normal);
	vec3 v = normalize(-view_pos.xyz);
	vec3 l = normalize(light_pos.xyz - view_pos.xyz);
	vec3 h = normalize(v + l);
	vec3 I = vec3(1, 1, 1); // light intensity of light

	vec3 rgb = getRGB(pos.y);
	
	ka = rgb * 0.5;
	kd = rgb;
	ks = rgb * 0.8;
	
	color = vec4(ka * 0.2 + kd * I * max(0.0, dot(n, l)) + ks * I * pow(max(0, dot(n, h)), p), 1.0);
};
