#version 460 core

in vec2 fragPos;
out vec4 FragColor;

uniform sampler3D uVolumenText;
uniform sampler1D uTransferFuncText;
uniform vec3 uViewPos;
uniform vec3 uViewDir;
uniform vec3 uViewUp;
uniform float uStepSize;
uniform vec3 uAABBMin;
uniform vec3 uAABBMax;
uniform int uShadowOn;
uniform float uAspectRatio;
uniform float uStartingPoint;
uniform float uFov;

bool intersectAABB(vec3 rayOrigin, vec3 rayDirection, vec3 AABBMin, vec3 AABBMax, out float tEnter, out float tExit) {
		vec3 t0 = (AABBMin - rayOrigin) / rayDirection;
		vec3 t1 = (AABBMax - rayOrigin) / rayDirection;
		vec3 tmin = min(t0, t1);
		vec3 tmax = max(t0, t1);
		tEnter = max(max(tmin.x, tmin.y), tmin.z);
		tExit = min(min(tmax.x, tmax.y), tmax.z);
		return tExit >= tEnter && tExit > 0.0;
}

void main() {
		//Camara Perspectiva
		vec2 uv = fragPos;
		uv.x *= uAspectRatio;
		vec3 rayOrigin = uViewPos;
		vec3 forward = normalize(uViewDir);
		vec3 right = normalize(cross(forward, normalize(uViewUp)));
		vec3 up = cross(right, forward);
		float fovScale = tan(radians(uFov) / 2.0);
		vec3 rayDirection = normalize(forward + (uv.x * fovScale * right) + (uv.y * fovScale * up));

		//Intersectar con contenedor
		float tEnter, tExit;
		if(!intersectAABB(rayOrigin, rayDirection, uAABBMin, uAABBMax, tEnter, tExit)) {
			discard;
		}
		tEnter = max(tEnter, 0.0); 

		//Ray Marching
		vec4 accu = vec4(0.0);
		for(float t = tEnter + uStartingPoint; t < tExit; t += uStepSize) {
			vec3 pos = rayOrigin + rayDirection * t;
			vec3 texPos = (pos - uAABBMin) / (uAABBMax - uAABBMin);
			float n = texture(uVolumenText, texPos).r;
			vec4 tfSample = texture(uTransferFuncText, n);
			vec3 stepColor = tfSample.rgb;
			float stepAlpha = tfSample.a;
			if (stepAlpha > 0.0) {
				float refStepSize = 0.039;
				stepAlpha = 1.0 - pow(1.0 - stepAlpha, uStepSize / refStepSize);
				if(uShadowOn == 1) {
					float depthShadow = clamp(1.0 - (t - tEnter) / (tExit - tEnter), 0.0, 1.0);
					stepColor *= depthShadow;
				}
				accu.rgb += (1.0 - accu.a) * stepColor * stepAlpha;
				accu.a += (1.0 - accu.a) * stepAlpha;
				if(accu.a >= 0.95) break;
			}
		}
		if (accu.a < 0.01) discard;
		FragColor = clamp(accu, 0.0, 1.0);
}