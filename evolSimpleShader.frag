#version 400

uniform float fracHeight;
uniform float fracWidth;
uniform float dt;
uniform vec3 param1;
uniform vec2 param2;
uniform vec3 a;
uniform vec3 b;
uniform vec3 c;
uniform vec3 d;
uniform float nSteps;

// EDO system for double pendulum
vec4 edoSys(vec4 inpt, float param[5]){
    vec4 outpt;
    outpt.z =  ((-sin(inpt.x-inpt.y)*(param[4]*param[1]*pow(inpt.z,2.0)*cos(inpt.x-inpt.y)+param[4]*param[2]*pow(inpt.w,2.0)) -param[0]*((param[3]+param[4])*sin(inpt.x)-param[4]*sin(inpt.y)*cos(inpt.x-inpt.y)) )/(param[1]*(param[3] + param[4]*pow(sin(inpt.x-inpt.y),2.0))));
    outpt.w =  ((sin(inpt.x-inpt.y)*(param[4]*param[1]*pow(inpt.z,2.0)+param[4]*param[2]*pow(inpt.w,2.0)*cos(inpt.x-inpt.y)) +param[0]*((param[3]+param[4])*sin(inpt.x)*cos(inpt.x-inpt.y)-(param[3]+param[4])*sin(inpt.y)) )/(param[2]*(param[3] + param[4]*pow(sin(inpt.x-inpt.y),2.0))));
    outpt.x = inpt.z;
    outpt.y = inpt.w;

    return outpt;
}

// Runge-Kutta 2nd order
vec4 rungeKutta(vec4 inpt, float param[5], float dt){
    return vec4(inpt + edoSys(inpt + edoSys(inpt, param)*dt/2.0, param)*dt);
}

void main(){
    // System parameters
	float param[5];
    float pi = 3.141592653589793238462643;

	param[0] = param1.x;
	param[1] = param1.y;
	param[2] = param1.z;
	param[3] = param2.x;
	param[4] = param2.y;

    // Initial conditions for the system
    vec4 inRG = vec4((gl_FragCoord.x-0.5)/fracWidth * 2.0*pi - pi, (fracHeight-gl_FragCoord.y-0.5)/fracHeight * 2.0*pi - pi, 0, 0);
    vec4 outRG;

    // Perform Runge-Kutta for n steps
    for(int i = 0; i < nSteps+1; i++){
        outRG = rungeKutta(inRG, param, dt);
        inRG = outRG;

        // Wrap angles to [-pi, pi]
        inRG.x = inRG.x-2*floor( (inRG.x+pi)/(2*pi) )*pi;
        inRG.y = inRG.y-2*floor( (inRG.y+pi)/(2*pi) )*pi;
    }

    // Maps the state radius (normalized to [0,1]) from the center to a pallete specified by [a,b,c,d]
    gl_FragColor = vec4(a + b*cos(6.28318*(c*(sqrt(pow(outRG.x,2.0)+pow(outRG.y,2.0))/4.442882938158366)+d)), 1.0);
}