#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "core/core.hpp"
#include "videoio/videoio.hpp"
#include "imgproc/imgproc.hpp"

using namespace std;
using namespace sf;
using namespace cv;

double mouseX;
double mouseY;
bool mousePressed = false;
bool focus = true;

// Misc variables
double pi = 3.141592;

// Modified atan function
double modAtan(double x, double y){
    double sol = atan(y/x) - pi/2;
    if(x >= 0){
        sol += pi;
    }
    return sol;
}

// Checks the sign of a number
int sign(double num){
    if(num > 0){
        return 1;
    }
    return -1;
}

// Maps linearly a variable between two bounds to another variable between another two bounds
double linearMap(double var, double bound1, double bound2, double outBound1, double outBound2){
    return outBound1 + (var-bound1)/(bound2-bound1)*(outBound2-outBound1);
}

// Checks if point and rect intersect
bool rectIntersect(double px, double py, double x, double y, double width, double height){
    if(px >= x && px <= x + width && py >= y && py <= y + height){
        return true;
    }
    return false;
}


// Converts number into string with a given number of significant digits
string nToS(auto num, int sigDig){
    string aux = to_string(num);
    string res = "";
    string nums = "1234567890";
    for(int i = 0; i < sigDig; i++){
        int counter = 0;
        for(int j = 0; j < nums.length(); j++){
            if(aux[i] != nums[j]){
                counter += 1;
            }
        }
        if(counter == nums.length()){
            sigDig += 1;
        }
    }
    for(int i = 0; i < sigDig; i++){
        res += aux[i];
    }
    return res;
}

// Class for making numerical buttons that change variables dinamycally
class numButton{
    public:
        double x;
        double y;
        double width;
        double height;
        string preTex;
        double num;
        int sigDig;
        bool once = true;

        RectangleShape butt;
        RectangleShape shadow;
        Text textData;

        numButton(double posx, double posy, double bwidth, double bheight, Font *fonText, string preText, int sigDigs){
            x = posx;
            y = posy;
            preTex = preText;
            width = bwidth;
            height = bheight;
            sigDig = sigDigs;

            butt.setSize(Vector2f(width, height));
            butt.setPosition(posx, posy);
            butt.setOutlineThickness(2.f);
            butt.setOutlineColor(Color::White);
            butt.setFillColor(Color(100,100,100));

            shadow.setSize(Vector2f(width, height));
            shadow.setPosition(posx, posy);
            shadow.setOutlineThickness(2.f);
            shadow.setOutlineColor(Color::White);
            shadow.setFillColor(Color(20,20,20));

            textData.setFont(*fonText);
            textData.setCharacterSize(10 + (int)width/7);
            textData.setFillColor(sf::Color::White);
            textData.setPosition(posx+width/2,posy+height/2);
        }

        void update(auto *numInput, double displayNum, double bound1, double bound2){
            num = *numInput;

            shadow.setSize(Vector2f(linearMap(displayNum,bound1,bound2,0,width), height));

            if (focus && (Mouse::isButtonPressed(sf::Mouse::Left) || Mouse::isButtonPressed(sf::Mouse::Right)) && rectIntersect(mouseX,mouseY,x,y,width,height)){
                *numInput = linearMap(mouseX-x,0,width,bound1,bound2);
            }

            num = *numInput;

            textData.setString(preTex+nToS(displayNum,sigDig));
            textData.setPosition(x+width/2-textData.getGlobalBounds().width/2,y+height/2-textData.getGlobalBounds().height);
        }

        void render(RenderWindow* win){
            win->draw(butt);
            win->draw(shadow);
            win->draw(textData);
        }
};

// Class for making boolean buttons that change variables dinamycally
class boolButton{
    public:
        double x;
        double y;
        double width;
        double height;
        string tex;
        bool var;

        int col = 40;

        RectangleShape butt;
        Text textData;

        boolButton(double posx, double posy, double bwidth, double bheight, Font *fonText, string text){
            x = posx;
            y = posy;
            tex = text;
            width = bwidth;
            height = bheight;

            butt.setSize(Vector2f(width, height));
            butt.setPosition(posx, posy);
            butt.setOutlineThickness(2.f);
            butt.setOutlineColor(Color::White);
            butt.setFillColor(Color(col,col,col));

            textData.setFont(*fonText);
            textData.setCharacterSize(10 + (int)width/7);
            textData.setFillColor(sf::Color::White);
            textData.setPosition(posx+width/2,posy+height/2);
            textData.setString(text);
            textData.setPosition(x+width/2-textData.getGlobalBounds().width/2,y+height/2-textData.getGlobalBounds().height);
        }

        void update(bool *newVar){
            // Changes boolean var if button is pressed
            if (focus && mousePressed && rectIntersect(mouseX,mouseY,x,y,width,height)){
                mousePressed = false;
                *newVar = !(*newVar);
            }

            // Changes color of button
            if(*newVar){
                col = 20;
            }else{
                col = 100;
            }
            butt.setFillColor(Color(col,col,col));

            var = *newVar;
        }

        void render(RenderWindow* win){
            win->draw(butt);
            win->draw(textData);
        }
};

void rungeKutta(void(*edo)(double[], double*, double[]), int n, double in[], double *out, double param[], double* t, double dt){
    double *k1 = new double[2*n];
    double *k2 = new double[2*n];
    double *k3 = new double[2*n];
    double *k4 = new double[2*n];

    double *aux1 = new double[2*n];
    double *aux2 = new double[2*n];
    double *aux3 = new double[2*n];

    edo(in, k1, param);

    for(int i = 0; i < 2*n; i++){
        aux1[i] = in[i] + k1[i]*dt/2.0;
    }
    edo(aux1, k2, param);

    for(int i = 0; i < 2*n; i++){
        aux2[i] = in[i] + k2[i]*dt/2.0;
    }
    edo(aux2, k3, param);

    for(int i = 0; i < 2*n; i++){
        aux3[i] = in[i] + k3[i]*dt;
    }
    edo(aux3, k4, param);

    free(k1);
    free(k2);
    free(k3);
    free(k4);
    free(aux1);
    free(aux2);
    free(aux3);
    for(int i = 0; i < 2*n; i++){
        out[i] = in[i] + (k1[i]+2*k2[i]+2*k3[i]+k4[i])*dt/6;
    }
    *t += dt;
}

// Calculates any explicit rk method given its Butcher tableau
void rk(void(*edo)(double[], double*, double[]), int n, double in[], double *out, double param[], double* t, double dt, int s, vector<vector<double> > a, vector<double> b, vector<double> c){
    double ks[s][2*n];
    // Calculate each k
    for(int i = 0; i < s; i++){
        // Input for stage s of rk
        double aux1[2*n];
        // Calculate input s
        for(int j = 0; j < 2*n; j++){
            aux1[j] = in[j];
            for(int k = 0; k < i; k++){
                aux1[j] += dt*a[i][k]*ks[k][j];
            }
        }
        // Output (ks)
        double aux2[2*n];
        edo(aux1, aux2, param);

        for(int j = 0; j < 2*n; j++){
            ks[i][j] = aux2[j];
        }
    }

    // Calculate next point using all ks
    for(int i = 0; i < 2*n; i++){
        out[i] = in[i];
        for(int j = 0; j < s; j++){
            out[i] += dt*b[j]*ks[j][i];
        }
    }
    *t += dt;
}

// ----------------
// Butcher Tableaus
// ----------------

// Forward Euler 1st
vector<vector<double> > rk_a1 = {
    {0}
};
vector<double> rk_b1 = {1};
vector<double> rk_c1 = {0};

// Midpoint 2nd
vector<vector<double> > rk_a2 = {
    {0,0},
    {1/2.0,0}
};
vector<double> rk_b2 = {0,1};
vector<double> rk_c2 = {0,1/2.0};

// Heun's 2nd
vector<vector<double> > rk_a3 = {
    {0,0},
    {1,0}
};
vector<double> rk_b3 = {1/2.0,1/2.0};
vector<double> rk_c3 = {0,1};

// Ralston's 2nd
vector<vector<double> > rk_a4 = {
    {0,0},
    {2/3.0,0}
};
vector<double> rk_b4 = {1/4.0,3/4.0};
vector<double> rk_c4 = {0,2/3.0};

// Kutta's 3rd
vector<vector<double> > rk_a5 = {
    {0,0,0},
    {1/2.0,0,0},
    {-1,2,0}
};
vector<double> rk_b5 = {1/6.0,2/3.0,1/6.0};
vector<double> rk_c5 = {0,1/2.0,1};

// Heun's 3rd
vector<vector<double> > rk_a6 = {
    {0,0,0},
    {1/3.0,0,0},
    {0,2/3.0,0}
};
vector<double> rk_b6 = {1/4.0,0,3/4.0};
vector<double> rk_c6 = {0,1/3.0,2/3.0};

// Wray's 3rd
vector<vector<double> > rk_a7 = {
    {0,0,0},
    {8/15.0,0,0},
    {1/4.0,5/12.0,0}
};
vector<double> rk_b7 = {1/4.0,0,3/4.0};
vector<double> rk_c7 = {0,8/15.0,2/3.0};

// Ralston's 3rd
vector<vector<double> > rk_a8 = {
    {0,0,0},
    {1/2.0,0,0},
    {0,3/4.0,0}
};
vector<double> rk_b8 = {2/9.0,1/3.0,4/9.0};
vector<double> rk_c8 = {0,1/2.0,3/4.0};

// SSPRK3 3rd
vector<vector<double> > rk_a9 = {
    {0,0,0},
    {1,0,0},
    {1/4.0,1/4.0,0}
};
vector<double> rk_b9 = {1/6.0,1/6.0,2/3.0};
vector<double> rk_c9 = {0,1,1/2.0};

// Runge-Kutta 4th
vector<vector<double> > rk_a10 = {
    {0,0,0,0},
    {1/2.0,0,0,0},
    {0,1/2.0,0,0},
    {0,0,1,0}
};
vector<double> rk_b10 = {1/6.0,1/3.0,1/3.0,1/6.0};
vector<double> rk_c10 = {0,1/2.0,1/2.0,1};

//https://personal.math.ubc.ca/~cbm/mscthesis/cbm-mscthesis.pdf
// SSP 5th
vector<vector<double> > rk_a11 = {
    {0,0,0,0,0},
    {0.37727,0,0,0,0},
    {0.37727,0.37727,0,0,0},
    {0.24300,0.24300,0.24300,0,0},
    {0.15359,0.15359,0.15359,0.23846,0}
};
vector<double> rk_b11 = {0.20673,0.20673,0.11710,0.18180,0.28763};
vector<double> rk_c11 = {0,0.37727,0.75454,0.72899,0.69923};

// SSP 6th
vector<vector<double> > rk_a12 = {
    {0,0,0,0,0,0},
    {0.28422,0,0,0,0,0},
    {0.28422,0.28422,0,0,0,0},
    {0.23071,0.23071,0.23071,0,0,0},
    {0.13416,0.13416,0.13416,0.16528,0,0},
    {0.13416,0.13416,0.13416,0.16528,0.28422,0},
};
vector<double> rk_b12 = {0.17016,0.17016,0.10198,0.12563,0.21604,0.21604};
vector<double> rk_c12 = {0,0.28422,0.56844,0.69213,0.56776,0.85198};

void assignRK(vector<vector<double> > &rka1, vector<double> &rkb1, vector<double> &rkc1, vector<vector<double> > rka2,  vector<double> rkb2,  vector<double> rkc2){
    rka1 = rka2;
    rkb1 = rkb2;
    rkc1 = rkc2;
}

// EDO of double pendulum
void edoSys(double input[], double *output, double param[]){
    output[2] =  (-sin(input[0]-input[1])*(param[4]*param[1]*pow(input[2],2)*cos(input[0]-input[1])+param[4]*param[2]*pow(input[3],2)) -param[0]*((param[3]+param[4])*sin(input[0])-param[4]*sin(input[1])*cos(input[0]-input[1])) )/(param[1]*(param[3] + param[4]*pow(sin(input[0]-input[1]),2)));
    output[3] =  (sin(input[0]-input[1])*(param[4]*param[1]*pow(input[2],2)+param[4]*param[2]*pow(input[3],2)*cos(input[0]-input[1])) +param[0]*((param[3]+param[4])*sin(input[0])*cos(input[0]-input[1])-(param[3]+param[4])*sin(input[1])) )/(param[2]*(param[3] + param[4]*pow(sin(input[0]-input[1]),2)));
    output[0] = input[2];
    output[1] = input[3];
}

int main(){
    // ---------------------
    // Variable Declarations
    // ---------------------

    // Window variables
    int width = 800;
    int height = 800;
    double fps = 60;

    // Event variables
    bool aPressed = false;
    bool bPressed = false;
    bool hPressed = false;

    // Window variable and configuration
    RenderWindow window(sf::VideoMode(width, height), "Simulation #1 - Double Pendulum");
    window.setFramerateLimit(fps);
    window.setKeyRepeatEnabled(false);

    // Video variables
    bool isRecording = false;
    VideoWriter video;

    // Graphic variables
    double rad1 = 10;
    double rad2 = 10;
    double off1 = width/2-rad1;
    double off2 = width/2-rad2;
    double px = off1;
    double py = off1;
    double px2 = off2;
    double py2 = off2;
    CircleShape shape1(rad1);
    CircleShape shape2(rad2);
    sf::Vertex line1[2] = {
        sf::Vertex(sf::Vector2f(  0,  0), sf::Color::White),
        sf::Vertex(sf::Vector2f(  0,  0), sf::Color::White)
    };
    sf::Vertex line2[2] = {
        sf::Vertex(sf::Vector2f(  0,  0), sf::Color::White),
        sf::Vertex(sf::Vector2f(  0,  0), sf::Color::White)
    };

    // Text variables
    sf::Font font1;
    font1.loadFromFile("C:/dev/utilities/fonts/Roboto/Roboto-Regular.ttf");

    // UI variables
    numButton dtUI(20, height-60, 100, 40, &font1, "dt: ", 4);

    numButton gUI(20, 80, 100, 40, &font1, "G: ", 2);
    numButton l1UI(20, 140, 100, 40, &font1, "L1: ", 3);
    numButton l2UI(20, 200, 100, 40, &font1, "L2: ", 3);
    numButton m1UI(20, 260, 100, 40, &font1, "M1: ", 2);
    numButton m2UI(20, 320, 100, 40, &font1, "M2: ", 2);

    numButton a1UI(20, 380, 100, 40, &font1, "A1: ", 3);
    numButton a2UI(20, 440, 100, 40, &font1, "A2: ", 3);
    numButton w1UI(20, 500, 100, 40, &font1, "W1: ", 2);
    numButton w2UI(20, 560, 100, 40, &font1, "W2: ", 2);

    numButton fracwUI(20, 80, 100, 40, &font1, "W: ", 4);
    numButton frachUI(20, 140, 100, 40, &font1, "H: ", 4);
    numButton fracaccUI(20, 200, 100, 40, &font1, "A: ", 5);
    numButton baseUI(20, 260, 100, 40, &font1, "B: ", 5);
    numButton tileSizeUI(20, 320, 100, 40, &font1, "T: ", 3);
    float base = 1.2;

    numButton colA1UI(20, height-60*3, 100, 40, &font1, "a1: ", 4);
    numButton colA2UI(140, height-60*3, 100, 40, &font1, "a2: ", 4);
    numButton colA3UI(260, height-60*3, 100, 40, &font1, "a3: ", 4);
    numButton colB1UI(380, height-60*3, 100, 40, &font1, "b1: ", 4);
    numButton colB2UI(500, height-60*3, 100, 40, &font1, "b2: ", 4);
    numButton colB3UI(620, height-60*3, 100, 40, &font1, "b3: ", 4);
    numButton colC1UI(20, height-60*2, 100, 40, &font1, "c1: ", 4);
    numButton colC2UI(140, height-60*2, 100, 40, &font1, "c2: ", 4);
    numButton colC3UI(260, height-60*2, 100, 40, &font1, "c3: ", 4);
    numButton colD1UI(380, height-60*2, 100, 40, &font1, "d1: ", 4);
    numButton colD2UI(500, height-60*2, 100, 40, &font1, "d2: ", 4);
    numButton colD3UI(620, height-60*2, 100, 40, &font1, "d3: ", 4);
    float cA[3] = {0.0, 0.5, 0.5};
    float cB[3] = {0.0, 0.5, 0.5};
    float cC[3] = {0.0, 0.5, 0.333};
    float cD[3] = {0.0, 0.5, 0.667};

    numButton wBoundUI(20, 200, 100, 40, &font1, "W: ", 3);

    boolButton tabUI(20, 20, 100, 40, &font1, "Tab");
    bool tabChange = false;
    int tab = 1;
    tabUI.textData.setString("Tab: "+nToS(tab,1));
    tabUI.textData.setPosition(tabUI.x+tabUI.width/2-tabUI.textData.getGlobalBounds().width/2,tabUI.y+tabUI.height/2-tabUI.textData.getGlobalBounds().height);

    boolButton edoUI(140, height-60, 200, 40, &font1, "R-K 4th");
    edoUI.textData.setCharacterSize(10 + (int)100/7);
    edoUI.textData.setPosition(edoUI.x+edoUI.width/2-edoUI.textData.getGlobalBounds().width/2,edoUI.y+edoUI.height/2-edoUI.textData.getGlobalBounds().height);
    bool edoChange = false;
    int edo = 10;
    vector<vector<double> > curRKa = rk_a10;
    vector<double> curRKb = rk_b10;
    vector<double> curRKc = rk_c10;

    boolButton evolModeUI(260, 20, 100, 40, &font1, "Mode 1");
    evolModeUI.textData.setPosition(evolModeUI.x+evolModeUI.width/2-evolModeUI.textData.getGlobalBounds().width/2,evolModeUI.y+evolModeUI.height/2-evolModeUI.textData.getGlobalBounds().height);
    bool evolModeChange = false;
    int evolMode = 1;

    bool ui = true;
    boolButton resetUI(140, 20, 100, 40, &font1, "Reset");
    bool reset = false;
    boolButton resetVelUI(260, 20, 100, 40, &font1, "Reset w");
    bool resetVel = false;
    boolButton pauseUI(380, 20, 100, 40, &font1, "Pause");
    bool pause = false;
    boolButton generateFracUI(140, 20, 100, 40, &font1, "Generate");
    bool generateFrac = false;
    boolButton saveFracUI(260, 20, 100, 40, &font1, "Save");
    bool saveFrac = false;
    boolButton captureEvolUI(140, 20, 100, 40, &font1, "Record");
    bool captureEvol = false;

    // EDO variables
    double g = 9.8;
    double l1 = 100;
    double l2 = 100;
    double m1 = 1;
    double m2 = 1;
    double param[5] = {g, l1, l2, m1, m2};
    int n = 2;
    double t;
    double* outRG = new double[2*n];
    double inRG[2*n];

    double dt = 0.2;
    double t0 = 0;
    double f0[2] = {pi/2,pi/2};
    double fp0[2] = {0,0};

    t = t0;
    inRG[0] = f0[0];
    inRG[1] = f0[1];
    inRG[2] = fp0[0];
    inRG[3] = fp0[1];

    shape1.setFillColor(sf::Color::White);
    shape2.setFillColor(sf::Color::White);

    // Fractal variables
    int fracWidth = 700;
    int fracHeight = 700;
    int precision = 1000;
    int tileSize = 100;
    int fracStage = 0;

    Shader fracShader;
    Texture frac;
    RenderTexture fracCanvas;
    fracShader.loadFromFile("fracShader.frag",Shader::Fragment);

    frac.create(fracWidth, fracHeight);
    RectangleShape fracQuad(Vector2f(fracWidth, fracHeight));
    fracQuad.setPosition(width/2 - fracWidth/2, height/2 - fracHeight/2);

    vector<RectangleShape> fracQuadTree;

    //  Evolution variables
    VideoWriter evolVideo;
    Texture evol;
    Shader evolShader;
    evolShader.loadFromFile("evolShader.frag",Shader::Fragment);
    Shader evolSimpleShader;
    evolSimpleShader.loadFromFile("evolSimpleShader.frag",Shader::Fragment);

    Texture evolState;
    Shader evolStateShader;
    evolStateShader.loadFromFile("evolStateShader.frag",Shader::Fragment);

    Shader palleteShader;
    palleteShader.loadFromFile("palleteShader.frag",Shader::Fragment);
    palleteShader.setParameter("width",700);
    RectangleShape palleteQuad(Vector2f(700, 40));
    palleteQuad.setPosition(20,height-60);

    RectangleShape evolQuad(Vector2f(fracWidth, fracHeight));
    evolQuad.setPosition(width/2 - fracWidth/2, height/2 - fracHeight/2);
    int evolSteps = 0;
    double wBound = 1;

    // MAIN LOOP
    while (window.isOpen()){
        // -------------
        // Event handler
        // -------------

        Event event;
        while (window.pollEvent(event)){
            switch(event.type){
                // Closing window
                case sf::Event::Closed:
                    // Closes video if window is closed
                    if(isRecording){
                        video.release();
                        cout << "Recording ended by closing of window" << endl;

                        isRecording = false;
                    }
                    window.close();
                    break;
                // Pressing key
                case sf::Event::KeyPressed:
                    switch(event.key.code){
                    case sf::Keyboard::A:
                        aPressed = true;
                        break;
                    case sf::Keyboard::B:
                        bPressed = true;
                        break;
                    case sf::Keyboard::H:
                        hPressed = true;
                        break;
                    }
                    break;
                // Unfocused window
                case sf::Event::LostFocus:
                    focus = false;
                    break;
                // Focused window
                case sf::Event::GainedFocus:
                    focus = true;
                    break;
                case Event::MouseButtonPressed:
                    mousePressed = true;
                    break;
                case Event::MouseButtonReleased:
                    mousePressed = false;
                    break;
                default:
                    break;
            }
        }

        // ------------
        // Update cycle
        // ------------

        // Updates mouse
        mouseX = Mouse::getPosition(window).x;
        mouseY = Mouse::getPosition(window).y;

        // Opening a video file
        if(!isRecording && aPressed){
            string filename = "./output/outVideo_"+to_string(rand() % 1000 + 1)+".avi";
            video.open(filename, VideoWriter::fourcc('M','J','P','G'), fps, Size(width,height));
            cout << "-- Start of recording --" << endl << endl;

            isRecording = true;
            aPressed = false;
        }

        // Closing a video file
        if(isRecording && bPressed){
            video.release();
            cout << "-- End of recording --" << endl << endl;

            isRecording = false;
            bPressed = false;
        }

        // Hides the ui
        if(hPressed){
            ui = !ui;
            hPressed = false;
        }

        // Changes the tab number
        if(tabChange){
            tabChange = false;
            if(tab >= 3){
                tab = 1;
            }else{
                tab += 1;
            }
            tabUI.textData.setString("Tab: "+nToS(tab,1));
            tabUI.textData.setPosition(tabUI.x+tabUI.width/2-tabUI.textData.getGlobalBounds().width/2,tabUI.y+tabUI.height/2-tabUI.textData.getGlobalBounds().height);
        }

        switch(tab){
        case 1:
            {
            // Updates the parameters
            if(ui){
                tabUI.update(&tabChange);
                dtUI.update(&dt, dt, 0.0005, 0.5);
                gUI.update(&g, g, -40, 40);
                l1UI.update(&l1, l1, 1, 500);
                l2UI.update(&l2, l2, 1, 500);
                m1UI.update(&m1, m1, 1, 100);
                m2UI.update(&m2, m2, 1, 100);

                if(!pause){
                    a1UI.update(&inRG[0], modAtan(px-width/2,-py+height/2), -pi, pi);
                    a2UI.update(&inRG[1], modAtan(px2-px,-py2+py), -pi, pi);
                    w1UI.update(&inRG[2], outRG[2], -2, 2);
                    w2UI.update(&inRG[3], outRG[3], -2, 2);
                }else{
                    a1UI.update(&outRG[0], modAtan(px-width/2,-py+height/2), -pi, pi);
                    a2UI.update(&outRG[1], modAtan(px2-px,-py2+py), -pi, pi);
                    w1UI.update(&outRG[2], outRG[2], -2, 2);
                    w2UI.update(&outRG[3], outRG[3], -2, 2);
                }

                resetUI.update(&reset);
                resetVelUI.update(&resetVel);
                pauseUI.update(&pause);
                edoUI.update(&edoChange);
            }

            // Resets the EDO variables
            if(reset){
                reset = false;
                outRG[0] = f0[0];
                outRG[1] = f0[1];
                outRG[2] = fp0[0];
                outRG[3] = fp0[1];
                inRG[0] = f0[0];
                inRG[1] = f0[1];
                inRG[2] = fp0[0];
                inRG[3] = fp0[1];
            }

            // Resets the angular velocities of the pendulums
            if(resetVel){
                resetVel = false;
                outRG[2] = 0;
                outRG[3] = 0;
                inRG[2] = 0;
                inRG[3] = 0;
            }

            // Changes the EDO solver used
            if(edoChange){
                edoChange = false;
                if(edo >= 12){
                    edo = 1;
                }else{
                    edo += 1;
                }

                curRKa.clear();
                curRKb.clear();
                curRKc.clear();

                switch(edo){
                case 1:
                    edoUI.textData.setString("Forward Euler 1st");
                    assignRK(curRKa,curRKb,curRKc,rk_a1,rk_b1,rk_c1);
                    break;
                case 2:
                    edoUI.textData.setString("Mid 2nd");
                    assignRK(curRKa,curRKb,curRKc,rk_a2,rk_b2,rk_c2);
                    break;
                case 3:
                    edoUI.textData.setString("Heun's 2nd");
                    assignRK(curRKa,curRKb,curRKc,rk_a3,rk_b3,rk_c3);
                    break;
                case 4:
                    edoUI.textData.setString("Ralston's 2nd");
                    assignRK(curRKa,curRKb,curRKc,rk_a4,rk_b4,rk_c4);
                    break;
                case 5:
                    edoUI.textData.setString("Kutta's 3rd");
                    assignRK(curRKa,curRKb,curRKc,rk_a5,rk_b5,rk_c5);
                    break;
                case 6:
                    edoUI.textData.setString("Heun's 3rd");
                    assignRK(curRKa,curRKb,curRKc,rk_a6,rk_b6,rk_c6);
                    break;
                case 7:
                    edoUI.textData.setString("Wra's 3rd");
                    assignRK(curRKa,curRKb,curRKc,rk_a7,rk_b7,rk_c7);
                    break;
                case 8:
                    edoUI.textData.setString("Ralston's 3rd");
                    assignRK(curRKa,curRKb,curRKc,rk_a8,rk_b8,rk_c8);
                    break;
                case 9:
                    edoUI.textData.setString("SSPRK3 3rd");
                    assignRK(curRKa,curRKb,curRKc,rk_a9,rk_b9,rk_c9);
                    break;
                case 10:
                    edoUI.textData.setString("R-K 4th");
                    assignRK(curRKa,curRKb,curRKc,rk_a10,rk_b10,rk_c10);
                    break;
                case 11:
                    edoUI.textData.setString("SSP 5th");
                    assignRK(curRKa,curRKb,curRKc,rk_a11,rk_b11,rk_c11);
                    break;
                case 12:
                    edoUI.textData.setString("SSP 6th");
                    assignRK(curRKa,curRKb,curRKc,rk_a12,rk_b12,rk_c12);
                    break;
                }
                edoUI.textData.setPosition(edoUI.x+edoUI.width/2-edoUI.textData.getGlobalBounds().width/2,edoUI.y+edoUI.height/2-edoUI.textData.getGlobalBounds().height);
            }

            // Scales the radiuses of the pendulums according to their masses
            rad1 = 5 + m1/2;
            rad2 = 5 + m2/2;
            off1 = width/2-rad1;
            off2 = width/2-rad2;
            shape1.setRadius(rad1);
            shape2.setRadius(rad2);

            // Updates the parameter list to pass to the EDO solver
            param[0] = g;
            param[1] = l1;
            param[2] = l2;
            param[3] = m1;
            param[4] = m2;

            // Performs one step of runge-kutta
            if(!pause){
                //rungeKutta(edoSys, n, inRG, outRG, param, &t, dt);
                rk(edoSys, n, inRG, outRG, param, &t, dt, curRKa.size(), curRKa, curRKb, curRKc);
            }

            // Rewrites input EDO variables with output EDO variables
            inRG[0] = outRG[0];
            inRG[1] = outRG[1];
            inRG[2] = outRG[2];
            inRG[3] = outRG[3];

            // Updates the position of the masses
            px = off1 + l1*sin(outRG[0]);
            py = off1 + l1*cos(outRG[0]);
            px2 = off2 + l1*sin(outRG[0]) + l2*sin(outRG[1]);
            py2 = off2 + l1*cos(outRG[0]) + l2*cos(outRG[1]);

            // Updates the position of the strings
            shape1.setPosition(px,py);
            shape2.setPosition(px2,py2);
            line1[0].position = Vector2f(width/2, height/2);
            line1[1].position = Vector2f(px+rad1, py+rad1);
            line2[0].position = Vector2f(px+rad1, py+rad1);
            line2[1].position = Vector2f(px2+rad2, py2+rad2);
            }
            break;
        case 2:
            {
            // Updates the parameters
            if(ui){
                palleteShader.setParameter("a",cA[0],cA[1],cA[2]);
                palleteShader.setParameter("b",cB[0],cB[1],cB[2]);
                palleteShader.setParameter("c",cC[0],cC[1],cC[2]);
                palleteShader.setParameter("d",cD[0],cD[1],cD[2]);

                tabUI.update(&tabChange);
                tileSizeUI.update(&tileSize, tileSize, 10, 800);
                fracwUI.update(&fracWidth, fracWidth, 100, 2160);
                frachUI.update(&fracHeight, fracHeight, 100, 2160);
                fracWidth = round(fracWidth/tileSize)*tileSize;
                fracHeight = round(fracHeight/tileSize)*tileSize;
                fracaccUI.update(&precision, precision, 10, 50000);
                baseUI.update(&base, base, 1.0005, 3);

                generateFracUI.update(&generateFrac);
                saveFracUI.update(&saveFrac);

                colA1UI.update(&cA[0], cA[0], -pi, pi);
                colA2UI.update(&cA[1], cA[1], -pi, pi);
                colA3UI.update(&cA[2], cA[2], -pi, pi);
                colB1UI.update(&cB[0], cB[0], -pi, pi);
                colB2UI.update(&cB[1], cB[1], -pi, pi);
                colB3UI.update(&cB[2], cB[2], -pi, pi);
                colC1UI.update(&cC[0], cC[0], -pi, pi);
                colC2UI.update(&cC[1], cC[1], -pi, pi);
                colC3UI.update(&cC[2], cC[2], -pi, pi);
                colD1UI.update(&cD[0], cD[0], -pi, pi);
                colD2UI.update(&cD[1], cD[1], -pi, pi);
                colD3UI.update(&cD[2], cD[2], -pi, pi);
            }

            if(generateFrac){
                // Number of quads in treeFractal
                int nTree = fracWidth/tileSize * fracHeight/tileSize;

                // Start fractal generation
                if(fracStage == 0){
                    cout << "-- Fractal generation started --" << endl;

                    // Create texture and renderTexture with new sizes
                    fracCanvas.create(fracWidth, fracHeight);
                    frac.create(fracWidth,fracHeight);
                    fracQuad.setSize(Vector2f(fracWidth,fracHeight));

                    // Reset and initialize quadTree position to render correctly in renderTexture
                    fracQuadTree.clear();
                    fracQuadTree.resize(nTree);
                    int idx = 0;
                    for(int i = 0; i < fracWidth/tileSize; i++){
                        for(int j = 0; j < fracHeight/tileSize; j++){
                            fracQuadTree[idx].setPosition(i*tileSize,j*tileSize);
                            fracQuadTree[idx].setSize(Vector2f(tileSize,tileSize));
                            idx++;
                        }
                    }

                    // Clear the render texture
                    fracCanvas.clear();
                }

                //Set shader parameters
                fracShader.setParameter("fracHeight",fracHeight);
                fracShader.setParameter("fracWidth",fracWidth);
                fracShader.setParameter("dt2",(float)dt);
                fracShader.setParameter("prec",(float)precision);
                fracShader.setParameter("param1",g,l1,l2);
                fracShader.setParameter("param2",m1,m2);
                fracShader.setParameter("base",base);
                fracShader.setParameter("a",cA[0],cA[1],cA[2]);
                fracShader.setParameter("b",cB[0],cB[1],cB[2]);
                fracShader.setParameter("c",cC[0],cC[1],cC[2]);
                fracShader.setParameter("d",cD[0],cD[1],cD[2]);

                // Apply the shader to a quad
                fracCanvas.draw(fracQuadTree[fracStage], &fracShader);
                fracCanvas.display();

                cout << "Shader part " << fracStage+1 << " out of " << nTree << " calculated " << endl;

                // Set the render quad position to the center of the screen
                fracQuad.setPosition(width/2 - (double)fracWidth/2, height/2 - (double)fracHeight/2);

                // Get the texture from the renderTexture and put it in the quad
                frac = fracCanvas.getTexture();
                fracQuad.setTexture(&frac, true);

                // Render next quad
                fracStage++;

                // End fractal generation
                if(fracStage == nTree){
                    generateFrac = false;
                    fracStage = 0;
                    cout << "-- Fractal generation ended --" << endl << endl;
                }
            }

            // Save fractal as image in hard drive
            if(saveFrac){
                saveFrac = false;
                string filename = "./output/fractal_g"+nToS(g,2)+"_1l"+nToS(l1,2)+"_2l"+nToS(l2,2)+"_1m"+nToS(m1,2)+"_2m"+nToS(m2,2)+"_w"+to_string(fracWidth)+"_h"+to_string(fracHeight)+"_p"+to_string(precision)+"_"+to_string(rand() % 1000 + 1)+".png";
                frac.copyToImage().saveToFile(filename);
                cout << "-- Fractal saved as png --" << endl << endl;
            }

            }
            break;
        case 3:
            {
            // Updates UI parameters
            bool evolCaptureDetector = captureEvol;
            if(ui){
                palleteShader.setParameter("a",cA[0],cA[1],cA[2]);
                palleteShader.setParameter("b",cB[0],cB[1],cB[2]);
                palleteShader.setParameter("c",cC[0],cC[1],cC[2]);
                palleteShader.setParameter("d",cD[0],cD[1],cD[2]);

                tabUI.update(&tabChange);
                fracwUI.update(&fracWidth, fracWidth, 100, 2160);
                frachUI.update(&fracHeight, fracHeight, 100, 2160);
                wBoundUI.update(&wBound, wBound, 0.05, 20);

                evolModeUI.update(&evolModeChange);
                if(evolModeChange){
                    evolModeChange = false;
                    if(evolMode >= 2){
                        evolMode = 1;
                    }else{
                        evolMode += 1;
                    }
                    evolModeUI.textData.setString("Mode "+nToS(evolMode,1));
                    evolModeUI.textData.setPosition(evolModeUI.x+evolModeUI.width/2-evolModeUI.textData.getGlobalBounds().width/2,evolModeUI.y+evolModeUI.height/2-evolModeUI.textData.getGlobalBounds().height);
                }

                captureEvolUI.update(&captureEvol);
                if(evolCaptureDetector != captureEvol){
                    if(captureEvol){
                        cout << "-- Video capture of pendulum evolution started --" << endl;

                        // Setup video writer
                        string filename = "./output/evolVideo_"+to_string(rand() % 100000 + 1)+".avi";
                        //string filename = "./output/evolVideo_1a.avi";
                        evolVideo.open(filename, VideoWriter::fourcc('M','J','P','G'), fps, Size(fracWidth,fracHeight));

                        // Set quad size
                        evolQuad.setSize(Vector2f(fracWidth,fracHeight));

                        // Set shader parameters
                        if(evolMode == 1){
                            evolState.create(fracWidth,fracHeight);
                            evolStateShader.setParameter("fracHeight",fracHeight);
                            evolStateShader.setParameter("fracWidth",fracWidth);
                            evolStateShader.setParameter("dt",(float)dt);
                            evolStateShader.setParameter("param1",g,l1,l2);
                            evolStateShader.setParameter("param2",m1,m2);
                            evolStateShader.setParameter("tex",evolState);
                            evolStateShader.setParameter("starting",true);
                            evolStateShader.setParameter("wBound",wBound);

                            evolShader.setParameter("fracHeight",fracHeight);
                            evolShader.setParameter("a",cA[0],cA[1],cA[2]);
                            evolShader.setParameter("b",cB[0],cB[1],cB[2]);
                            evolShader.setParameter("c",cC[0],cC[1],cC[2]);
                            evolShader.setParameter("d",cD[0],cD[1],cD[2]);
                            evolShader.setParameter("tex",evolState);
                        }else{
                            evolSimpleShader.setParameter("fracHeight",fracHeight);
                            evolSimpleShader.setParameter("fracWidth",fracWidth);
                            evolSimpleShader.setParameter("dt",(float)dt);
                            evolSimpleShader.setParameter("param1",g,l1,l2);
                            evolSimpleShader.setParameter("param2",m1,m2);
                            evolSimpleShader.setParameter("a",cA[0],cA[1],cA[2]);
                            evolSimpleShader.setParameter("b",cB[0],cB[1],cB[2]);
                            evolSimpleShader.setParameter("c",cC[0],cC[1],cC[2]);
                            evolSimpleShader.setParameter("d",cD[0],cD[1],cD[2]);
                            evolSimpleShader.setParameter("nSteps",evolSteps);
                        }
                        evolSteps = 0;
                    }else{
                        cout << "-- Video capture of pendulum evolution ended --" << endl << endl;
                        evolVideo.release();
                    }
                }

                colA1UI.update(&cA[0], cA[0], -pi, pi);
                colA2UI.update(&cA[1], cA[1], -pi, pi);
                colA3UI.update(&cA[2], cA[2], -pi, pi);
                colB1UI.update(&cB[0], cB[0], -pi, pi);
                colB2UI.update(&cB[1], cB[1], -pi, pi);
                colB3UI.update(&cB[2], cB[2], -pi, pi);
                colC1UI.update(&cC[0], cC[0], -pi, pi);
                colC2UI.update(&cC[1], cC[1], -pi, pi);
                colC3UI.update(&cC[2], cC[2], -pi, pi);
                colD1UI.update(&cD[0], cD[0], -pi, pi);
                colD2UI.update(&cD[1], cD[1], -pi, pi);
                colD3UI.update(&cD[2], cD[2], -pi, pi);
            }

            if(captureEvol && evolMode == 1){
                // Create texture that is rendered and renderTexture
                RenderTexture evolCanvas;
                evolQuad.setPosition(0,0);
                evol.create(fracWidth,fracHeight);
                evolCanvas.create(fracWidth, fracHeight);

                // Clear renderTexture, update state texture and retrieve it
                evolCanvas.clear();

                auto rs = RenderStates();
                rs.blendMode = BlendNone;
                rs.shader = &evolStateShader;

                evolCanvas.draw(evolQuad,rs);
                evolCanvas.display();
                evolState = evolCanvas.getTexture();

                // Saves pure state data as img in hard drive
                if(evolSteps > 0){
                    evolState.copyToImage().saveToFile("stateInfo/state"+to_string(evolSteps)+".png");
                }

                // Set state textures in render and state shaders
                evolShader.setParameter("tex",evolState);
                evolStateShader.setParameter("tex",evolState);

                // Clear canvas and get texture to render
                evolCanvas.clear();

                evolCanvas.draw(evolQuad, &evolShader);
                evolCanvas.display();
                evol = evolCanvas.getTexture();

                // Setup the quad for window rendering
                evolQuad.setPosition(width/2 - (double)fracWidth/2, height/2 - (double)fracHeight/2);
                evolQuad.setTexture(&evol, true);

                // Gets the image from the window texture
                Image videoFrame;
                videoFrame = evol.copyToImage();

                // Transforms an SFML image into a CV image
                Size videImgSize(videoFrame.getSize().x, videoFrame.getSize().y);
                Mat vidFrameMat(videImgSize, CV_8UC4, (void*)videoFrame.getPixelsPtr(), Mat::AUTO_STEP);

                // Writes CV image as a frame into the video
                evolVideo.write(vidFrameMat);

                // Sets start to false
                evolStateShader.setParameter("starting",false);
            }

            if(captureEvol && evolMode == 2){
                // Track frame count of evolution
                evolSimpleShader.setParameter("nSteps",float(evolSteps));
                evolSteps++;

                // Create texture that is rendered and renderTexture
                RenderTexture evolCanvas;
                evolQuad.setPosition(0,0);
                evol.create(fracWidth,fracHeight);
                evolCanvas.create(fracWidth, fracHeight);

                // Clear canvas and get texture to render
                evolCanvas.clear();

                evolCanvas.draw(evolQuad, &evolSimpleShader);
                evolCanvas.display();
                evol = evolCanvas.getTexture();

                // Setup the quad for window rendering
                evolQuad.setPosition(width/2 - (double)fracWidth/2, height/2 - (double)fracHeight/2);
                evolQuad.setTexture(&evol, true);

                // Gets the image from the window texture
                Image videoFrame;
                videoFrame = evol.copyToImage();

                // Transforms an SFML image into a CV image
                Size videImgSize(videoFrame.getSize().x, videoFrame.getSize().y);
                Mat vidFrameMat(videImgSize, CV_8UC4, (void*)videoFrame.getPixelsPtr(), Mat::AUTO_STEP);

                // Writes CV image as a frame into the video
                evolVideo.write(vidFrameMat);
            }

            }
            break;
        default:
            break;
        }

        // ------------
        // Render cycle
        // ------------

        // Clears window
        window.clear();

        switch(tab){
        case 1:
            // Draws masses and strings to window
            window.draw(shape1);
            window.draw(shape2);
            window.draw(line1, 2, sf::Lines);
            window.draw(line2, 2, sf::Lines);

            // Draws the UI elements
            if(ui){
                tabUI.render(&window);
                dtUI.render(&window);
                gUI.render(&window);
                l1UI.render(&window);
                l2UI.render(&window);
                m1UI.render(&window);
                m2UI.render(&window);

                a1UI.render(&window);
                a2UI.render(&window);
                w1UI.render(&window);
                w2UI.render(&window);

                resetUI.render(&window);
                resetVelUI.render(&window);
                pauseUI.render(&window);
                edoUI.render(&window);
            }
            break;
        case 2:
            window.draw(fracQuad);

            // Draws the UI elements
            if(ui){
                tabUI.render(&window);
                generateFracUI.render(&window);
                saveFracUI.render(&window);
                tileSizeUI.render(&window);
                fracwUI.render(&window);
                frachUI.render(&window);
                fracaccUI.render(&window);
                baseUI.render(&window);

                colA1UI.render(&window);
                colA2UI.render(&window);
                colA3UI.render(&window);
                colB1UI.render(&window);
                colB2UI.render(&window);
                colB3UI.render(&window);
                colC1UI.render(&window);
                colC2UI.render(&window);
                colC3UI.render(&window);
                colD1UI.render(&window);
                colD2UI.render(&window);
                colD3UI.render(&window);

                window.draw(palleteQuad, &palleteShader);
            }
            break;
        case 3:
            // Draws quad with video in it
            window.draw(evolQuad);

            // Draws the UI elements
            if(ui){
                tabUI.render(&window);
                captureEvolUI.render(&window);
                fracwUI.render(&window);
                frachUI.render(&window);
                wBoundUI.render(&window);
                evolModeUI.render(&window);

                colA1UI.render(&window);
                colA2UI.render(&window);
                colA3UI.render(&window);
                colB1UI.render(&window);
                colB2UI.render(&window);
                colB3UI.render(&window);
                colC1UI.render(&window);
                colC2UI.render(&window);
                colC3UI.render(&window);
                colD1UI.render(&window);
                colD2UI.render(&window);
                colD3UI.render(&window);

                // Draws quad for visualization of pallete
                window.draw(palleteQuad, &palleteShader);
            }
            break;
        default:
            break;
        }

        // Displays window
        window.display();

        // Adds frame to video
        if(isRecording){
            // Gets the window contents into a texture
            Texture windowFrame;
            windowFrame.create(width,height);
            windowFrame.update(window);
            // Gets the image from the window texture
            Image videoFrame;
            videoFrame = windowFrame.copyToImage();
            // Transforms an SFML image into a CV image
            Size videImgSize(videoFrame.getSize().x, videoFrame.getSize().y);
            Mat vidFrameMat(videImgSize, CV_8UC4, (void*)videoFrame.getPixelsPtr(), Mat::AUTO_STEP);
            // Writes CV image as a frame into the video
            video.write(vidFrameMat);
        }
    }

    return 0;
}
