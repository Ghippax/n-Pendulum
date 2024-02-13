#version 400

uniform float fracHeight;
uniform float fracWidth;
uniform float dt;
uniform vec3 param1;
uniform vec2 param2;
uniform sampler2D tex;
uniform bool starting;
uniform float wBound;

// Maps a value in the interval [a,b] to the interval [c,d]
float linearMap(float var, float bound1, float bound2, float outBound1, float outBound2){
    return outBound1 + (var-bound1)/(bound2-bound1)*(outBound2-outBound1);
}

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
    // Parameters of the system
	float param[5];
    float pi = 3.141592653589793238462643;

	param[0] = param1.x;
	param[1] = param1.y;
	param[2] = param1.z;
	param[3] = param2.x;
	param[4] = param2.y;

    vec4 outRG;
    vec4 inRG;

    // Bounds on texture data compression
    float pBound = 1;

    // Set up input of Runge-Kutta
    if(starting){
        // Initial conditions of the system
        inRG = vec4((gl_FragCoord.x-0.5)/fracWidth * 2.0*pi - pi, (fracHeight-gl_FragCoord.y-0.5)/fracHeight * 2.0*pi - pi, 0, 0);
    }else{
        // Retrieve previous state of the system from texture information
        vec4 texelValue = texelFetch(tex, ivec2(gl_FragCoord.x-0.5,fracHeight-gl_FragCoord.y-0.5), 0);
        inRG = vec4(linearMap(texelValue.x,0,pBound,-pi,pi),linearMap(texelValue.y,0,pBound,-pi,pi),linearMap(texelValue.z,0,pBound,-wBound,wBound),linearMap(texelValue.w,0,pBound,-wBound,wBound));
    }

    // Do 1 iteration of Runge-Kutta
    outRG = rungeKutta(inRG, param, dt);

    // Wrap angles to [-pi, pi]
    outRG.x = outRG.x-2*floor( (outRG.x+pi)/(2*pi) )*pi;
    outRG.y = outRG.y-2*floor( (outRG.y+pi)/(2*pi) )*pi;

    // Apply sigmoid to soften the bounds on omega
    outRG.z = (2*wBound)/(1+exp(-2/wBound*outRG.z)) - wBound;
    outRG.w = (2*wBound)/(1+exp(-2/wBound*outRG.w)) - wBound;
  
    // Transform calculated state to pixel values
    gl_FragColor = vec4(linearMap(outRG.x,-pi,pi,0,pBound),linearMap(outRG.y,-pi,pi,0,pBound),linearMap(outRG.z,-wBound,wBound,0,pBound),linearMap(outRG.w,-wBound,wBound,0,pBound));

    // Use the initial state as initial texture
    if(starting){
        gl_FragColor = vec4(linearMap(inRG.x,-pi,pi,0,pBound),linearMap(inRG.y,-pi,pi,0,pBound),linearMap(inRG.z,-wBound,wBound,0,pBound),linearMap(inRG.w,-wBound,wBound,0,pBound));
    }
}