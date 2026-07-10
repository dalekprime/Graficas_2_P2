#version 460 core

in vec2 fragUV;
uniform sampler3D uNoiseText;
uniform vec3 uViewPos;
uniform vec3 uViewDir;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform int uSteps;
uniform float uStepSize;
out vec4 FragColor;

const vec3 boxMin = vec3(-0.5, -0.5, -0.5);
const vec3 boxMax = vec3( 0.5,  0.5,  0.5);

vec2 intersectAABB(vec3 ro, vec3 rd) {
		vec3 t0 = (boxMin - ro) / rd;
		vec3 t1 = (boxMax - ro) / rd;
		vec3 tmin = min(t0, t1);
		vec3 tmax = max(t0, t1);
		float dstA = max(max(tmin.x, tmin.y), tmin.z);
		float dstB = min(min(tmax.x, tmax.y), tmax.z);
		return vec2(dstA, dstB);
}

void main() {
		// Ajuste de pantalla
		vec2 uv = fragUV * 2.0 - 1.0;
		uv.x *= 800.0 / 600.0;

		vec3 worldUp = vec3(0.0, 1.0, 0.0);
		vec3 right = normalize(cross(uViewDir, worldUp));
		vec3 up = normalize(cross(right, uViewDir));

		// Disparo del rayo usando los vectores pre-armados
		vec3 ro = uViewPos;
		vec3 rd = normalize(uViewDir + uv.x * right + uv.y * up);

		// Verificamos si tocamos el AABB
		vec2 hit = intersectAABB(ro, rd);
		float tIn = hit.x;
		float tOut = hit.y;

		if (tOut < 0.0 || tIn > tOut) {
			discard; 
		}
		tIn = max(tIn, 0.0); 

		// Ray Marching Integrador
		vec4 accu = vec4(0.0);
		float t = tIn; 

		for(int i = 0; i < uSteps; i++) {
			if (t > tOut) break; 
			
			vec3 pos = ro + rd * t;
			vec3 texPos = pos + vec3(0.5); // Convertimos espacio de mundo a espacio UV
			
			float n = texture(uNoiseText, texPos).r * 255.0;
			
			if (n >= 20.0) {
				vec3 stepColor = vec3(0.0);
				float tau = 0.0; 
				
				// Tu Función de Transferencia
				if (n < 75.0) {
					stepColor = vec3(0.75, 1.0, 1.0); 
					tau = 2.0; 
				}
				else if (n < 150.0) {
					stepColor = vec3(1.0, 0.25, 0.3); 
					tau = 5.0; 
				}
				else { 
					stepColor = vec3(0.5); 
					tau = 20.0; 
				}
				
				// Discretización exponencial física
				float stepAlpha = 1.0 - exp(-uStepSize * tau);
				
				accu.rgb += (1.0 - accu.a) * stepColor * stepAlpha;
				accu.a += (1.0 - accu.a) * stepAlpha;
				
				if(accu.a >= 0.95) break; 
			}
			
			t += uStepSize; 
		}
		
		if (accu.a < 0.01) discard;
		FragColor = accu;
	}