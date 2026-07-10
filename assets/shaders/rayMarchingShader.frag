#version 460 core
uniform sampler3D uNoiseText; 
uniform vec2 uRotation;
uniform int uSteps;
uniform float uStepSize;
in vec2 fragUV;
out vec4 FragColor;

mat2 rot(float a) {
	float s = sin(a), c = cos(a);
	return mat2(c, -s, s, c);
}

float hash(vec2 p) {
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	vec2 uv = fragUV * 2.0 - 1.0;
	uv.x *= 800.0 / 600.0;

	//Camara Ortogonal
	//vec3 rayOrigin = vec3(uv.x, uv.y, -2.0);
	//vec3 rayDirection = vec3(0.0, 0.0, 1.0);

	//Camara Perspectiva
	vec3 rayOrigin = vec3(0.0, 0.0, -2.0);
	vec3 rayDirection = normalize(vec3(uv, 1.0)); 

	rayOrigin.yz *= rot(uRotation.y);
	rayDirection.yz *= rot(uRotation.y);
	rayOrigin.xz *= rot(uRotation.x);
	rayDirection.xz *= rot(uRotation.x);
	rayOrigin += vec3(0.5);
	//Ray Marching
	vec4 accu = vec4(0.0);

	float jitter = hash(fragUV) * uStepSize;
	//vec3 pos = rayOrigin + (rayDirection * jitter);

	vec3 pos = rayOrigin;
	for(int i = 0; i < uSteps; i++) {
		pos += rayDirection * uStepSize;  
		if(pos.x < 0.0 || pos.x > 1.0 || pos.y < 0.0 || pos.y > 1.0 || pos.z < 0.0 || pos.z > 1.0) {
			continue;
		}
		float n = texture(uNoiseText, pos).r * 255.0;
		vec3 stepColor;
		float stepAlpha = 0.05;
		if (n < 20.0) {
			continue;
		}
		else if (n < 75.0) {
			stepColor = vec3(0.75, 1.0, 1.0); 
			stepAlpha = 0.02;
		}
		else if (n < 150.0) {
			stepColor = vec3(1.0, 0.25, 0.3); 
			stepAlpha = 0.05;
		}
		else { 
			stepColor = vec3(0.5); 
			stepAlpha = 1.0;
		}
		accu.rgb += (1.0 - accu.a) * stepColor * stepAlpha;
		accu.a += (1.0 - accu.a) * stepAlpha;
		//Early Termination
		if(accu.a >= 0.95) break;
	}
	if (accu.a < 0.01) discard;
	FragColor = accu;
}