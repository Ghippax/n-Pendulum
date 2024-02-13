#version 400

uniform float fracHeight;
uniform float fracWidth;
uniform float dt2;
uniform float prec;
uniform vec3 param1;
uniform vec2 param2;
uniform float base;
uniform vec3 a;
uniform vec3 b;
uniform vec3 c;
uniform vec3 d;

vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d){
    return a + b*cos(6.28318*(c*t+d));
}

float logc(float x, float basec){
	return log(x)/log(basec);
}

vec4 edoSys(vec4 inpt, float param[5]){
	vec4 outpt;
    outpt.z =  (-sin(inpt.x-inpt.y)*(param[4]*param[1]*pow(inpt.z,2.0)*cos(inpt.x-inpt.y)+param[4]*param[2]*pow(inpt.w,2.0)) -param[0]*((param[3]+param[4])*sin(inpt.x)-param[4]*sin(inpt.y)*cos(inpt.x-inpt.y)) )/(param[1]*(param[3] + param[4]*pow(sin(inpt.x-inpt.y),2.0)));
    outpt.w =  (sin(inpt.x-inpt.y)*(param[4]*param[1]*pow(inpt.z,2.0)+param[4]*param[2]*pow(inpt.w,2.0)*cos(inpt.x-inpt.y)) +param[0]*((param[3]+param[4])*sin(inpt.x)*cos(inpt.x-inpt.y)-(param[3]+param[4])*sin(inpt.y)) )/(param[2]*(param[3] + param[4]*pow(sin(inpt.x-inpt.y),2.0)));
    outpt.x = inpt.z;
    outpt.y = inpt.w;

    return outpt;
}

vec4 rungeKutta(vec4 inpt, float param[5], float dt){
    vec4 k1;
    vec4 k2;
    vec4 k3;
    vec4 k4;

    k1 = edoSys(inpt, param);
    k2 = edoSys(inpt + k1*dt/2.0, param);
    k3 = edoSys(inpt + k2*dt/2.0, param);
    k4 = edoSys(inpt + k3*dt, param);

    vec4 sol;

    sol = vec4(inpt + (k1+2*k2+2*k3+k4)*dt/6.0);

    return sol;
}

void main(){
	float pi = 3.141592;
	float param[5];

	param[0] = param1.x;
	param[1] = param1.y;
	param[2] = param1.z;
	param[3] = param2.x;
	param[4] = param2.y;

	vec4 inRG;
	vec4 outRG;

    float stepsTillFlip = 0.0;

   	inRG = vec4((gl_FragCoord.x-0.5)/fracWidth * 2.0*pi - pi, (fracHeight-gl_FragCoord.y-0.5)/fracHeight * 2.0*pi - pi, 0, 0);

   	while(abs(outRG.x) < pi && abs(outRG.y) < pi && stepsTillFlip < prec){
   		stepsTillFlip = stepsTillFlip + 1.0;
   		outRG = rungeKutta(inRG, param, dt2);
   		inRG = outRG;
   	}

   	for(float k = 0.0; k < floor(logc(prec,base)); k++){
   		if(floor(logc(stepsTillFlip,base)) >= k){
   			vec3 col = palette(k/floor(logc(prec,base)),a,b,c,d);
   			gl_FragColor = vec4(col.xyz, 1.0);
   		}
   	}
}