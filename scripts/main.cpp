#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <fstream>
#include <stack>

using namespace std;

// ============================================================
//  SECTION 1: ENUMS & STRUCTS
// ============================================================

enum BlockType {
    START,
    CONTROL_WAIT,
    CONTROL_REPEAT,
    CONTROL_REPEAT_END,
    CONTROL_FOREVER,
    CONTROL_FOREVER_END,
    CONTROL_IF,
    CONTROL_IF_END,
    CONTROL_IF_ELSE,
    CONTROL_ELSE,
    CONTROL_IF_ELSE_END,
    CONTROL_STOP_ALL,
    MOTION_SETX,
    MOTION_SETY,
    MOTION_MOVENSTEP,
    MOTION_TURNNDEG,
    MOTION_TURNRIGHT,
    MOTION_TURNLEFT,
    MOTION_CHANGEXBY,
    MOTION_CHANGEYBY,
    MOTION_CHANGEXYBY,
    MOTION_RANDOMPOS,
    MOTION_CHANGEDEG,
    MOTION_IFONEDGE,
    LOOKS_SAY,
    LOOKS_SAY_SECS,
    LOOKS_THINK,
    LOOKS_SHOW,
    LOOKS_HIDE,
    LOOKS_SET_SIZE,
    LOOKS_CHANGE_SIZE,
    LOOKS_CLEAR_EFFECTS,
    PEN_DOWN,
    PEN_UP,
    PEN_ERASE_ALL,
    PEN_STAMP,
    PEN_SET_SIZE,
    PEN_CHANGE_SIZE,
    SOUND_PLAY,
    SOUND_STOP_ALL,
    SOUND_PLAY_GAMEVOICE,
    SOUND_PLAY_SHEYPOR,
    SOUND_PLAY_OPENING,
    VAR_SET,
    VAR_CHANGE,
    SENSING_RESET_TIMER,
    SENSING_MOUSE_X,
    SENSING_MOUSE_Y,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_EQUAL,
    OP_LESS,
    OP_GREATER,
    OP_AND,
    OP_OR,
    OP_NOT,
    MYBLOCK_DEFINE,
    MYBLOCK_CALL,
    EVENT_KEY_PRESSED,
    // Math operators (امتیازی)
    OP_ABS,
    OP_SQRT,
    OP_FLOOR,
    OP_CEIL,
    OP_SIN,
    OP_COS,
    // String operators
    OP_STR_LEN,
    OP_STR_CHAR,
    OP_STR_JOIN,
    // Motion: go to mouse pointer
    MOTION_GOTO_MOUSE,
};

enum LogLevel { LOG_INFO, LOG_WARNING, LOG_ERROR };

struct Mouse {
    int x = 0, y = 0;
    int scrolloffsetCodeMenu = 0;
    int scrolloffsetBlocksPlayGround = 0;
    bool isMouseDown = false;
    bool isClicked   = false;
};

struct Spirit {
    string spiritName   = "spirit-1";
    int    spiritX      = 0;
    int    spiritY      = 0;
    int    direction    = 0;
    int    size         = 100;
    bool   isShow       = true;
    SDL_Texture* texture = nullptr;
    string texturePath  = "../assets/north.png";
    bool      penDown   = false;
    SDL_Color penColor  = {0, 0, 200, 255};
    int       penSize   = 2;
    string  sayText    = "";
    string  thinkText  = "";
    Uint32  sayUntil   = 0;
    Uint32  thinkUntil = 0;
    int colorEffect = 0;
};

struct Block {
    string    blockMode  = "";
    string    blockName  = "";
    SDL_Color bgColor    = {200, 200, 200, 255};
    BlockType type       = START;
    int       inputCount = 0;
    int       x = 0, y  = 0;
    string    inputValue1    = "";
    bool      isInput1Active = false;
    string    inputValue2    = "";
    bool      isInput2Active = false;
    string    inputValue3    = "";
    string    customFuncName = "";
    int       jumpTarget     = -1;
    int       jumpTarget2    = -1;
};

struct PenPoint {
    int x, y;
    SDL_Color color;
    int size;
};

struct LogEntry {
    int    cycle;
    int    line;
    string command;
    string message;
    LogLevel level;
};

struct Variable {
    string name;
    string value;
};

struct Button {
    SDL_Rect  rect;
    const char* text;
    SDL_Color normalColor, hoverColor, focusColor, textColor, focusTextColor;
    SDL_Color currentColor     = {200, 200, 200, 255};
    SDL_Color currentTextColor = {0,   0,   0,   255};
};

struct SoundSettings {
    string  name;
    string  filename;
    int     volume;
    int     pitch;
    bool    draggingVol   = false;
    bool    draggingPitch = false;
};

// ============================================================
//  SECTION 2: GLOBAL STATE
// ============================================================

SDL_Window*   m_window   = nullptr;
SDL_Renderer* m_renderer = nullptr;
TTF_Font*     font       = nullptr;
TTF_Font*     fontSmall  = nullptr;

map<string, SDL_Color> gTheme = {
        {"primary",       {137, 207, 240, 255}},
        {"secondary",     {230, 240, 255, 255}},
        {"bgSecondary",   {217, 227, 242, 255}},
        {"white",         {255, 255, 255, 255}},
        {"bgWhite",       {249, 249, 249, 255}},
        {"textPrimary",   {255, 255, 255, 255}},
        {"textSecondary", {110, 110, 110, 255}},
        {"textColor",     { 50,  50,  50, 255}},
        {"bgMotion",      { 76, 151, 255, 255}},
        {"bgLooks",       {153, 102, 255, 255}},
        {"bgSound",       {207,  99, 207, 255}},
        {"bgEvent",       {255, 191,   0, 255}},
        {"bgSensing",     { 92, 177, 214, 255}},
        {"bgControl",     {255, 171,  25, 255}},
        {"bgOperators",   { 89, 192,  89, 255}},
        {"bgVariable",    {255, 140,  26, 255}},
        {"bgMyBlocks",    {255, 102, 128, 255}},
        {"bgPen",         {  0, 180,  80, 255}},
        {"red",           {220,  50,  50, 255}},
};

int    DW = 1920, DH = 1080;
bool   gIsRunning       = true;
bool   gIsInDebug       = false;
bool   gIsExecuting     = false;
bool   gIsPaused        = false;
bool   gIsDragging      = false;
bool   gShowLog         = false;

string gSelectedMode       = "Code";
string gSelectedBlockType  = "Motion";
string gCurrentProjectName = "Untitled";

Mouse  gMouse;
Spirit gSpirit;
Block  gDraggedBlock;
int    gDraggedBlockIndex = -1;
int    gExecuteIndex      = 0;
int    gCycleCount        = 0;
Uint32 gTimerStartTick    = 0;

vector<Block>     gExecuteList;
vector<PenPoint>  gPenTrail;
vector<LogEntry>  gLogEntries;
vector<Variable>  gVariables;
stack<int>        gLoopStack;
string gLastPressedKey = "";

Mix_Chunk* gSoundGameVoice = nullptr;
Mix_Chunk* gSoundSheypor   = nullptr;
Mix_Chunk* gSoundOpening   = nullptr;

SoundSettings gSoundSettings[3] = {
        {"Game Voice", "gamevoice.wav", MIX_MAX_VOLUME, 100},
        {"Sheypor",    "sheypor.wav",   MIX_MAX_VOLUME, 100},
        {"Opening",    "opening.wav",   MIX_MAX_VOLUME, 100},
};

// ── Sound panel text-input state ───────────────────────────
struct SoundInputState {
    string volStr[3]     = {"128", "128", "128"};
    string pitchStr[3]   = {"100", "100", "100"};
    int activeVolInput   = -1;   // index of sound whose vol box is focused
    int activePitchInput = -1;   // index of sound whose pitch box is focused
} gSndInput;

int    gOpCounter = 0;

static const int WATCHDOG_LIMIT = 1000;

static bool isOpBlock(BlockType t){
    return t==OP_ADD||t==OP_SUBTRACT||t==OP_MULTIPLY||t==OP_DIVIDE||
           t==OP_EQUAL||t==OP_LESS||t==OP_GREATER||
           t==OP_AND||t==OP_OR||t==OP_NOT||
           t==OP_ABS||t==OP_SQRT||t==OP_FLOOR||t==OP_CEIL||
           t==OP_SIN||t==OP_COS||
           t==OP_STR_LEN||t==OP_STR_CHAR||t==OP_STR_JOIN;
}
static void assignOpResultName(Block& b){
    if(!isOpBlock(b.type)) return;
    if(!b.inputValue3.empty()) return;
    gOpCounter++;
    b.inputValue3 = "op_result_" + to_string(gOpCounter);
}

// ============================================================
//  SECTION 3: UTILITY
// ============================================================

SDL_Color changeBrightness(SDL_Color c, int amount) {
    c.r = (Uint8)min(255, c.r * amount / 100);
    c.g = (Uint8)min(255, c.g * amount / 100);
    c.b = (Uint8)min(255, c.b * amount / 100);
    return c;
}
bool isInRect(SDL_Rect r, int mx, int my) {
    return mx > r.x && mx < r.x + r.w && my > r.y && my < r.y + r.h;
}

// ============================================================
//  SECTION 4: DRAWING
// ============================================================

void drawRect(int x, int y, int w, int h, SDL_Color c) {
    SDL_Rect r = {x, y, w, h};
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(m_renderer, &r);
}
void drawRectBorder(int x, int y, int w, int h, SDL_Color bg, SDL_Color border, int bw) {
    drawRect(x, y, w, h, border);
    drawRect(x+bw, y+bw, w-2*bw, h-2*bw, bg);
}
void renderText(const string& text, int x, int y, SDL_Color color, TTF_Font* f = nullptr) {
    if (!f) f = font;
    if (text.empty() || !f) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(m_renderer, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}
void drawFilledCircle(int cx, int cy, int r, SDL_Color c) {
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
    for (int dy = -r; dy <= r; dy++) {
        int xl = (int)sqrt((double)(r*r - dy*dy));
        SDL_RenderDrawLine(m_renderer, cx-xl, cy+dy, cx+xl, cy+dy);
    }
}
void drawCircleWithBorder(int cx, int cy, int r, SDL_Color fill, SDL_Color border, int bw) {
    drawFilledCircle(cx, cy, r, border);
    drawFilledCircle(cx, cy, r-bw, fill);
}
void renderButton(const Button& btn) {
    drawRect(btn.rect.x, btn.rect.y, btn.rect.w, btn.rect.h, btn.currentColor);
    if (!btn.text || !strlen(btn.text)) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, btn.text, btn.currentTextColor);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
    SDL_Rect tr = {btn.rect.x+(btn.rect.w-surf->w)/2, btn.rect.y+(btn.rect.h-surf->h)/2, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(m_renderer, tex, NULL, &tr);
    SDL_DestroyTexture(tex);
}

// ============================================================
//  SECTION 5: LOGGER
// ============================================================

void logEvent(LogLevel level, int line, const string& cmd, const string& msg) {
    gLogEntries.push_back({gCycleCount, line, cmd, msg, level});
    string lv = level==LOG_INFO?"INFO":level==LOG_WARNING?"WARN":"ERROR";
    cout<<"[Cycle:"<<gCycleCount<<"][L"<<line<<"]["<<lv<<"]["<<cmd<<"] "<<msg<<"\n";
}
void renderLogOverlay() {
    if (!gShowLog) return;
    int px=DW-500, py=100, pw=490, ph=420;
    SDL_SetRenderDrawColor(m_renderer, 15,15,25,215);
    SDL_Rect bg={px,py,pw,ph}; SDL_RenderFillRect(m_renderer,&bg);
    SDL_SetRenderDrawColor(m_renderer, 80,160,255,255); SDL_RenderDrawRect(m_renderer,&bg);
    renderText("=== LOGGER (L to toggle) ===", px+8, py+5, {100,200,255,255}, fontSmall);
    int start=max(0,(int)gLogEntries.size()-16), yOff=py+28;
    for (int i=start; i<(int)gLogEntries.size(); i++) {
        auto& e=gLogEntries[i];
        SDL_Color col = e.level==LOG_INFO?SDL_Color{180,255,180,255}:e.level==LOG_WARNING?SDL_Color{255,220,80,255}:SDL_Color{255,80,80,255};
        string line="["+to_string(e.cycle)+"][L"+to_string(e.line)+"]["+e.command+"] "+e.message;
        if (line.size()>60) line=line.substr(0,57)+"...";
        renderText(line, px+8, yOff, col, fontSmall); yOff+=24;
    }
}

// ============================================================
//  SECTION 6: SAVE / LOAD
// ============================================================

string blockTypeToStr(BlockType t) {
    static map<BlockType,string> m={
            {START,"START"},{CONTROL_WAIT,"CTRL_WAIT"},{CONTROL_REPEAT,"CTRL_RPT"},
            {CONTROL_REPEAT_END,"CTRL_RPT_END"},{CONTROL_FOREVER,"CTRL_FREV"},
            {CONTROL_FOREVER_END,"CTRL_FREV_END"},{CONTROL_IF,"CTRL_IF"},
            {CONTROL_IF_END,"CTRL_IF_END"},
            {CONTROL_IF_ELSE,"CTRL_IF_ELSE"},{CONTROL_ELSE,"CTRL_ELSE"},
            {CONTROL_IF_ELSE_END,"CTRL_IF_ELSE_END"},
            {CONTROL_STOP_ALL,"CTRL_STOP"},
            {MOTION_SETX,"MOT_SETX"},{MOTION_SETY,"MOT_SETY"},{MOTION_MOVENSTEP,"MOT_MOVE"},
            {MOTION_TURNNDEG,"MOT_TURN"},{MOTION_TURNRIGHT,"MOT_TURNR"},{MOTION_TURNLEFT,"MOT_TURNL"},
            {MOTION_CHANGEXBY,"MOT_CHGX"},{MOTION_CHANGEYBY,"MOT_CHGY"},
            {MOTION_CHANGEXYBY,"MOT_CHGXY"},{MOTION_RANDOMPOS,"MOT_RAND"},
            {MOTION_CHANGEDEG,"MOT_DIR"},{MOTION_IFONEDGE,"MOT_EDGE"},
            {LOOKS_SAY,"LK_SAY"},{LOOKS_SAY_SECS,"LK_SAY_S"},{LOOKS_THINK,"LK_THINK"},
            {LOOKS_SHOW,"LK_SHOW"},{LOOKS_HIDE,"LK_HIDE"},
            {LOOKS_SET_SIZE,"LK_SETSIZE"},{LOOKS_CHANGE_SIZE,"LK_CHGSIZE"},{LOOKS_CLEAR_EFFECTS,"LK_CLEARFX"},
            {PEN_DOWN,"PEN_DOWN"},{PEN_UP,"PEN_UP"},{PEN_ERASE_ALL,"PEN_ERASE"},
            {PEN_STAMP,"PEN_STAMP"},{PEN_SET_SIZE,"PEN_SETSIZE"},{PEN_CHANGE_SIZE,"PEN_CHGSIZE"},
            {SOUND_PLAY,"SND_PLAY"},{SOUND_STOP_ALL,"SND_STOP"},
            {SOUND_PLAY_GAMEVOICE,"SND_GAMEVOICE"},
            {SOUND_PLAY_SHEYPOR,"SND_SHEYPOR"},
            {SOUND_PLAY_OPENING,"SND_OPENING"},
            {VAR_SET,"VAR_SET"},{VAR_CHANGE,"VAR_CHG"},
            {SENSING_RESET_TIMER,"SEN_RTIMER"},{SENSING_MOUSE_X,"SEN_MX"},{SENSING_MOUSE_Y,"SEN_MY"},
            {OP_ADD,"OP_ADD"},{OP_SUBTRACT,"OP_SUB"},{OP_MULTIPLY,"OP_MUL"},
            {OP_DIVIDE,"OP_DIV"},{OP_EQUAL,"OP_EQ"},{OP_LESS,"OP_LT"},
            {OP_GREATER,"OP_GT"},{OP_AND,"OP_AND"},{OP_OR,"OP_OR"},{OP_NOT,"OP_NOT"},
            {MYBLOCK_DEFINE,"MY_DEF"},{MYBLOCK_CALL,"MY_CALL"},{EVENT_KEY_PRESSED,"EVT_KEY"},
            {OP_ABS,"OP_ABS"},{OP_SQRT,"OP_SQRT"},{OP_FLOOR,"OP_FLOOR"},{OP_CEIL,"OP_CEIL"},
            {OP_SIN,"OP_SIN"},{OP_COS,"OP_COS"},
            {OP_STR_LEN,"OP_SLEN"},{OP_STR_CHAR,"OP_SCHAR"},{OP_STR_JOIN,"OP_SJOIN"},
            {MOTION_GOTO_MOUSE,"MOT_MOUSE"},
    };
    auto it=m.find(t); return it!=m.end()?it->second:"START";
}
BlockType strToBlockType(const string& s) {
    static map<string,BlockType> rev;
    if (rev.empty()) {
        rev={
                {"START",START},{"CTRL_WAIT",CONTROL_WAIT},{"CTRL_RPT",CONTROL_REPEAT},
                {"CTRL_RPT_END",CONTROL_REPEAT_END},{"CTRL_FREV",CONTROL_FOREVER},
                {"CTRL_FREV_END",CONTROL_FOREVER_END},{"CTRL_IF",CONTROL_IF},
                {"CTRL_IF_END",CONTROL_IF_END},
                {"CTRL_IF_ELSE",CONTROL_IF_ELSE},{"CTRL_ELSE",CONTROL_ELSE},
                {"CTRL_IF_ELSE_END",CONTROL_IF_ELSE_END},
                {"CTRL_STOP",CONTROL_STOP_ALL},
                {"MOT_SETX",MOTION_SETX},{"MOT_SETY",MOTION_SETY},{"MOT_MOVE",MOTION_MOVENSTEP},
                {"MOT_TURN",MOTION_TURNNDEG},{"MOT_TURNR",MOTION_TURNRIGHT},{"MOT_TURNL",MOTION_TURNLEFT},
                {"MOT_CHGX",MOTION_CHANGEXBY},{"MOT_CHGY",MOTION_CHANGEYBY},
                {"MOT_CHGXY",MOTION_CHANGEXYBY},{"MOT_RAND",MOTION_RANDOMPOS},
                {"MOT_DIR",MOTION_CHANGEDEG},{"MOT_EDGE",MOTION_IFONEDGE},
                {"LK_SAY",LOOKS_SAY},{"LK_SAY_S",LOOKS_SAY_SECS},{"LK_THINK",LOOKS_THINK},
                {"LK_SHOW",LOOKS_SHOW},{"LK_HIDE",LOOKS_HIDE},
                {"LK_SETSIZE",LOOKS_SET_SIZE},{"LK_CHGSIZE",LOOKS_CHANGE_SIZE},{"LK_CLEARFX",LOOKS_CLEAR_EFFECTS},
                {"PEN_DOWN",PEN_DOWN},{"PEN_UP",PEN_UP},{"PEN_ERASE",PEN_ERASE_ALL},
                {"PEN_STAMP",PEN_STAMP},{"PEN_SETSIZE",PEN_SET_SIZE},{"PEN_CHGSIZE",PEN_CHANGE_SIZE},
                {"SND_PLAY",SOUND_PLAY},{"SND_STOP",SOUND_STOP_ALL},
                {"SND_GAMEVOICE",SOUND_PLAY_GAMEVOICE},
                {"SND_SHEYPOR",SOUND_PLAY_SHEYPOR},
                {"SND_OPENING",SOUND_PLAY_OPENING},
                {"SND_POP",SOUND_PLAY_GAMEVOICE},{"SND_MEOW",SOUND_PLAY_SHEYPOR},{"SND_DRUM",SOUND_PLAY_OPENING},
                {"VAR_SET",VAR_SET},{"VAR_CHG",VAR_CHANGE},
                {"SEN_RTIMER",SENSING_RESET_TIMER},{"SEN_MX",SENSING_MOUSE_X},{"SEN_MY",SENSING_MOUSE_Y},
                {"OP_ADD",OP_ADD},{"OP_SUB",OP_SUBTRACT},{"OP_MUL",OP_MULTIPLY},
                {"OP_DIV",OP_DIVIDE},{"OP_EQ",OP_EQUAL},{"OP_LT",OP_LESS},
                {"OP_GT",OP_GREATER},{"OP_AND",OP_AND},{"OP_OR",OP_OR},{"OP_NOT",OP_NOT},
                {"MY_DEF",MYBLOCK_DEFINE},{"MY_CALL",MYBLOCK_CALL},{"EVT_KEY",EVENT_KEY_PRESSED},
                {"OP_ABS",OP_ABS},{"OP_SQRT",OP_SQRT},{"OP_FLOOR",OP_FLOOR},{"OP_CEIL",OP_CEIL},
                {"OP_SIN",OP_SIN},{"OP_COS",OP_COS},
                {"OP_SLEN",OP_STR_LEN},{"OP_SCHAR",OP_STR_CHAR},{"OP_SJOIN",OP_STR_JOIN},
                {"MOT_MOUSE",MOTION_GOTO_MOUSE},
        };
    }
    auto it=rev.find(s); return it!=rev.end()?it->second:START;
}
static vector<string> splitPipe(const string& s) {
    vector<string> parts; string tok;
    for (size_t i=0; i<s.size(); i++) {
        if (s[i]=='\\'&&i+1<s.size()&&s[i+1]=='|'){tok+='|';i++;}
        else if (s[i]=='|'){parts.push_back(tok);tok.clear();}
        else tok+=s[i];
    }
    parts.push_back(tok); return parts;
}
static string ep(const string& s){string r;for(char c:s){if(c=='|')r+="\\|";else r+=c;}return r;}

// Save feedback timer
static Uint32 gSaveNotifyTick = 0;
static string gSaveNotifyMsg  = "";

bool saveProject(const string& filename) {
    ofstream f(filename); if(!f){cerr<<"Cannot save: "<<filename<<"\n";return false;}
    // ── Serialization: human-readable pipe-delimited format ──────────────
    // FORMAT:
    //   SCRATCH_V1                          <- version header
    //   NAME|<project_name>
    //   SPIRIT|name|x|y|dir|size|show|texturePath
    //   SND_CFG|idx|volume|pitch            <- per-sound settings
    //   VARS|<count>
    //   V|name|value                        <- one per variable
    //   BLOCKS|<count>
    //   B|mode|name|typeStr|inputCount|v1|v2|v3|r|g|b|a  <- one per block
    f<<"SCRATCH_V1\n";
    f<<"NAME|"<<ep(gCurrentProjectName)<<"\n";
    f<<"SPIRIT|"<<ep(gSpirit.spiritName)<<"|"<<gSpirit.spiritX<<"|"<<gSpirit.spiritY<<"|"
     <<gSpirit.direction<<"|"<<gSpirit.size<<"|"<<(gSpirit.isShow?1:0)<<"|"<<ep(gSpirit.texturePath)<<"\n";
    for(int i=0;i<3;i++)
        f<<"SND_CFG|"<<i<<"|"<<gSoundSettings[i].volume<<"|"<<gSoundSettings[i].pitch<<"\n";
    f<<"VARS|"<<gVariables.size()<<"\n";
    for(auto& v:gVariables) f<<"V|"<<ep(v.name)<<"|"<<ep(v.value)<<"\n";
    f<<"BLOCKS|"<<gExecuteList.size()<<"\n";
    for(auto& b:gExecuteList)
        f<<"B|"<<ep(b.blockMode)<<"|"<<ep(b.blockName)<<"|"<<blockTypeToStr(b.type)<<"|"<<b.inputCount<<"|"
         <<ep(b.inputValue1)<<"|"<<ep(b.inputValue2)<<"|"<<ep(b.inputValue3)<<"|"
         <<(int)b.bgColor.r<<"|"<<(int)b.bgColor.g<<"|"<<(int)b.bgColor.b<<"|"<<(int)b.bgColor.a<<"\n";
    gSaveNotifyMsg  = "Saved: " + filename;
    gSaveNotifyTick = SDL_GetTicks();
    cout<<"Saved: "<<filename<<"\n";
    return true;
}
bool loadSpiritTexture();
bool loadProject(const string& filename) {
    ifstream f(filename); if(!f){cerr<<"Cannot load: "<<filename<<"\n";return false;}
    string header; getline(f,header);
    if(header!="SCRATCH_V1"){cerr<<"Invalid file format\n";return false;}
    gExecuteList.clear();gVariables.clear();gPenTrail.clear();
    gExecuteIndex=0;gIsExecuting=false;gIsPaused=false;
    while(!gLoopStack.empty())gLoopStack.pop();
    string line;
    while(getline(f,line)){
        auto p=splitPipe(line); if(p.empty()) continue;
        if(p[0]=="NAME"&&p.size()>=2) gCurrentProjectName=p[1];
        else if(p[0]=="SPIRIT"&&p.size()>=8){
            gSpirit.spiritName=p[1];gSpirit.spiritX=stoi(p[2]);gSpirit.spiritY=stoi(p[3]);
            gSpirit.direction=stoi(p[4]);gSpirit.size=stoi(p[5]);gSpirit.isShow=(p[6]=="1");
            gSpirit.texturePath=p[7];
            if(gSpirit.texture)SDL_DestroyTexture(gSpirit.texture);
            loadSpiritTexture();
        }
        else if(p[0]=="SND_CFG"&&p.size()>=4){
            int i=stoi(p[1]);
            if(i>=0&&i<3){
                gSoundSettings[i].volume=stoi(p[2]);
                gSoundSettings[i].pitch=stoi(p[3]);
                gSndInput.volStr[i]   = to_string(gSoundSettings[i].volume);
                gSndInput.pitchStr[i] = to_string(gSoundSettings[i].pitch);
            }
        }
        else if(p[0]=="V"&&p.size()>=3) gVariables.push_back({p[1],p[2]});
        else if(p[0]=="B"&&p.size()>=11){
            Block b; b.blockMode=p[1];b.blockName=p[2];b.type=strToBlockType(p[3]);
            b.inputCount=stoi(p[4]);b.inputValue1=p[5];b.inputValue2=p[6];
            if(p.size()>=12){b.inputValue3=p[7];b.bgColor={(Uint8)stoi(p[8]),(Uint8)stoi(p[9]),(Uint8)stoi(p[10]),(Uint8)stoi(p[11])};}
            else{b.bgColor={(Uint8)stoi(p[7]),(Uint8)stoi(p[8]),(Uint8)stoi(p[9]),(Uint8)stoi(p[10])};}
            gExecuteList.push_back(b);
        }
    }
    cout<<"Loaded: "<<filename<<"\n"; return true;
}
void newProject(){
    gExecuteList.clear();gVariables.clear();gPenTrail.clear();gLogEntries.clear();
    gExecuteIndex=0;gIsExecuting=false;gIsPaused=false;
    while(!gLoopStack.empty())gLoopStack.pop();
    gSpirit=Spirit();gCurrentProjectName="Untitled";gTimerStartTick=SDL_GetTicks();loadSpiritTexture();
}

// ============================================================
//  SECTION 7: SPRITE FUNCTIONS
// ============================================================

bool loadSpiritTexture(){
    SDL_Surface* surf=IMG_Load(gSpirit.texturePath.c_str());
    if(!surf){cout<<"IMG_Load Error: "<<IMG_GetError()<<"\n";return false;}
    gSpirit.texture=SDL_CreateTextureFromSurface(m_renderer,surf);SDL_FreeSurface(surf);
    return gSpirit.texture!=nullptr;
}
void validatePosition(){gSpirit.spiritX=max(0,min(DW-1,gSpirit.spiritX));gSpirit.spiritY=max(0,min(DH-1,gSpirit.spiritY));}
void validateDirection(){gSpirit.direction%=360;if(gSpirit.direction<0)gSpirit.direction+=360;}
void recordPenPoint(){gPenTrail.push_back({gSpirit.spiritX,gSpirit.spiritY,gSpirit.penColor,gSpirit.penSize});}

void setSpiritX(int x){gSpirit.spiritX=x;validatePosition();logEvent(LOG_INFO,gExecuteIndex,"SET_X","X="+to_string(gSpirit.spiritX));}
void setSpiritY(int y){gSpirit.spiritY=y;validatePosition();logEvent(LOG_INFO,gExecuteIndex,"SET_Y","Y="+to_string(gSpirit.spiritY));}
void moveNSteps(int n){
    double rad=gSpirit.direction*M_PI/180.0;
    gSpirit.spiritX+=(int)(n*cos(rad));gSpirit.spiritY-=(int)(n*sin(rad));
    validatePosition();if(gSpirit.penDown)recordPenPoint();
    logEvent(LOG_INFO,gExecuteIndex,"MOVE",to_string(n)+" steps");
}
void turnNDeg(int n){gSpirit.direction+=n;validateDirection();}
void turnRight(){gSpirit.direction=0;}
void turnLeft(){gSpirit.direction=180;}
void goToXY(int x,int y){gSpirit.spiritX=x;gSpirit.spiritY=y;validatePosition();if(gSpirit.penDown)recordPenPoint();}
void pointInDir(int d){gSpirit.direction=d;validateDirection();}
void changeXBy(int x){gSpirit.spiritX+=x;validatePosition();if(gSpirit.penDown)recordPenPoint();}
void changeYBy(int y){gSpirit.spiritY+=y;validatePosition();if(gSpirit.penDown)recordPenPoint();}
void goRandom(){gSpirit.spiritX=rand()%DW;gSpirit.spiritY=rand()%DH;if(gSpirit.penDown)recordPenPoint();}
void ifOnEdge(){
    bool edge=(gSpirit.spiritX==0||gSpirit.spiritX==DW-1||gSpirit.spiritY==0||gSpirit.spiritY==DH-1);
    if(edge){gSpirit.direction=(gSpirit.direction+180)%360;logEvent(LOG_WARNING,gExecuteIndex,"EDGE","Bounced");}
}
void setSay(const string& txt,int ms=0){gSpirit.sayText=txt;gSpirit.sayUntil=ms>0?SDL_GetTicks()+ms:0;}
void setThink(const string& txt,int ms=0){gSpirit.thinkText=txt;gSpirit.thinkUntil=ms>0?SDL_GetTicks()+ms:0;}
void setSize(int pct){gSpirit.size=max(1,min(500,pct));}
void changeSize(int d){setSize(gSpirit.size+d);}
void clearEffects(){gSpirit.colorEffect=0;}
void penDown(){gSpirit.penDown=true;recordPenPoint();}
void penUp(){gSpirit.penDown=false;}
void penErase(){gPenTrail.clear();}
void penStamp(){gPenTrail.push_back({gSpirit.spiritX,gSpirit.spiritY,gSpirit.penColor,48});}
void penSetSize(int s){gSpirit.penSize=max(1,min(100,s));}
void penChgSize(int d){penSetSize(gSpirit.penSize+d);}

void setVar(const string& name,const string& val){
    for(auto& v:gVariables){if(v.name==name){v.value=val;return;}}
    gVariables.push_back({name,val});
}
void changeVar(const string& name,float delta){
    for(auto& v:gVariables){if(v.name==name){float cur=0;try{cur=stof(v.value);}catch(...){}v.value=to_string(cur+delta);return;}}
}
string getVar(const string& name){for(auto& v:gVariables)if(v.name==name)return v.value;return "";}

// ============================================================
//  SECTION 8: SOUND PLAYBACK WITH PITCH SUPPORT
// ============================================================

Mix_Chunk* createPitchedChunk(Mix_Chunk* src, int pitchPct){
    if(!src || pitchPct==100) return nullptr;
    float ratio = pitchPct / 100.0f;
    int srcSamples = src->alen / 2;
    int dstSamples = (int)(srcSamples / ratio);
    if(dstSamples<=0) return nullptr;
    Uint8* dstBuf = (Uint8*)SDL_malloc(dstSamples * 2);
    if(!dstBuf) return nullptr;
    Sint16* srcData = (Sint16*)src->abuf;
    Sint16* dstData = (Sint16*)dstBuf;
    for(int i=0;i<dstSamples;i++){
        float srcIdx = i * ratio;
        int   s0 = (int)srcIdx;
        int   s1 = min(s0+1, srcSamples-1);
        float t  = srcIdx - s0;
        dstData[i] = (Sint16)(srcData[s0]*(1-t) + srcData[s1]*t);
    }
    Mix_Chunk* chunk = (Mix_Chunk*)SDL_malloc(sizeof(Mix_Chunk));
    chunk->abuf      = dstBuf;
    chunk->alen      = dstSamples * 2;
    chunk->allocated = 1;
    chunk->volume    = src->volume;
    return chunk;
}

void playSound(int sndIdx){
    Mix_Chunk* chunks[3] = {gSoundGameVoice, gSoundSheypor, gSoundOpening};
    Mix_Chunk* src = chunks[sndIdx];
    if(!src) return;
    SoundSettings& s = gSoundSettings[sndIdx];
    Mix_Chunk* pitched = createPitchedChunk(src, s.pitch);
    Mix_Chunk* toPlay  = pitched ? pitched : src;
    Mix_VolumeChunk(toPlay, s.volume);
    Mix_PlayChannel(-1, toPlay, 0);
    static Mix_Chunk* lastPitched[3] = {nullptr,nullptr,nullptr};
    if(lastPitched[sndIdx]) Mix_FreeChunk(lastPitched[sndIdx]);
    lastPitched[sndIdx] = pitched;
}

// ============================================================
//  SECTION 9: EXPRESSION EVALUATOR
// ============================================================

static string strim(const string& s){
    size_t a=s.find_first_not_of(" \t\r\n"),b=s.find_last_not_of(" \t\r\n");
    return a==string::npos?"":s.substr(a,b-a+1);
}
static float toFloat(const string& token){
    string t=strim(token);
    try{return stof(t);}catch(...){}
    string v=getVar(t);try{return stof(v);}catch(...){}
    return 0.0f;
}
static bool tokenToBool(const string& token){
    string t=strim(token);
    if(t=="true"||t=="1") return true;
    if(t=="false"||t=="0") return false;
    string v=getVar(t);
    if(!v.empty()){if(v=="true"||v=="1")return true;try{return stof(v)!=0.0f;}catch(...){}return false;}
    try{return stof(t)!=0.0f;}catch(...){}
    return false;
}
// ── helpers for new operators ─────────────────────────────────
// بررسی اینکه آیا توکن عدد است (مستقیم یا از متغیر)
static bool isNumericToken(const string& token){
    string t=strim(token);
    try{stof(t);return true;}catch(...){}
    string v=getVar(t);if(!v.empty()){try{stof(v);return true;}catch(...){}return false;}
    return false;
}
// دریافت مقدار string توکن (اگر متغیر باشد، مقدار متغیر)
static string resolveStr(const string& token){
    string t=strim(token);
    string v=getVar(t);
    return v.empty()?t:v;
}
// لاگ خطای validation با نمایش در overlay
static void opError(int idx, const string& msg){
    logEvent(LOG_ERROR,idx,"OP_INVALID",msg);
}

bool evalCondition(const string& expr){
    string e=strim(expr);
    if(e.empty()) return false;
    if(e.size()>4&&(e.substr(0,4)=="NOT "||e.substr(0,4)=="not "))
        return !evalCondition(e.substr(4));
    auto findLast=[&](const string& op, bool wordOnly)->int{
        int olen=(int)op.size();
        for(int i=(int)e.size()-olen;i>0;i--){
            if(e.substr(i,olen)==op){
                if(wordOnly){
                    bool lOk=(i==0||!isalnum(e[i-1])&&e[i-1]!='_');
                    bool rOk=(i+olen>=(int)e.size()||(!isalnum(e[i+olen])&&e[i+olen]!='_'));
                    if(!lOk||!rOk) continue;
                }
                return i;
            }
        }
        return -1;
    };
    for(auto& wop:vector<string>{"OR","or"}){
        int p=findLast(wop,true);
        if(p>0){return evalCondition(strim(e.substr(0,p)))||evalCondition(strim(e.substr(p+(int)wop.size())));}
    }
    for(auto& wop:vector<string>{"AND","and"}){
        int p=findLast(wop,true);
        if(p>0){return evalCondition(strim(e.substr(0,p)))&&evalCondition(strim(e.substr(p+(int)wop.size())));}
    }
    for(auto& op:vector<string>{"==","!=","<=",">="}){
        int p=findLast(op,false);
        if(p>0){
            float lv=toFloat(strim(e.substr(0,p)));float rv=toFloat(strim(e.substr(p+2)));
            if(op=="==") return lv==rv;if(op=="!=") return lv!=rv;
            if(op=="<=") return lv<=rv;if(op==">=") return lv>=rv;
        }
    }
    for(auto& op:vector<string>{"<",">"}){
        int p=findLast(op,false);
        if(p>0){
            float lv=toFloat(strim(e.substr(0,p)));float rv=toFloat(strim(e.substr(p+1)));
            if(op=="<") return lv<rv;if(op==">") return lv>rv;
        }
    }
    return tokenToBool(e);
}

// ============================================================
//  SECTION 10: PRE-PROCESSING
// ============================================================

void preProcessBlocks(){
    stack<pair<int,BlockType>> cs;
    int n=(int)gExecuteList.size();
    for(auto& b:gExecuteList){b.jumpTarget=-1;b.jumpTarget2=-1;}
    for(int i=0;i<n;i++){
        BlockType t=gExecuteList[i].type;
        if(t==CONTROL_IF||t==CONTROL_REPEAT||t==CONTROL_FOREVER||t==CONTROL_IF_ELSE)
            cs.push({i,t});
        else if(t==CONTROL_IF_END){
            if(!cs.empty()&&cs.top().second==CONTROL_IF){
                int o=cs.top().first;cs.pop();
                gExecuteList[o].jumpTarget=i+1;gExecuteList[i].jumpTarget=o;
            }
        }
        else if(t==CONTROL_ELSE){
            if(!cs.empty()&&cs.top().second==CONTROL_IF_ELSE){
                int o=cs.top().first;
                gExecuteList[o].jumpTarget2=i+1;gExecuteList[i].jumpTarget=o;
            }
        }
        else if(t==CONTROL_IF_ELSE_END){
            if(!cs.empty()&&cs.top().second==CONTROL_IF_ELSE){
                int o=cs.top().first;cs.pop();
                gExecuteList[o].jumpTarget=i+1;
                if(gExecuteList[o].jumpTarget2<0)gExecuteList[o].jumpTarget2=i+1;
                for(int j=o+1;j<i;j++){
                    if(gExecuteList[j].type==CONTROL_ELSE&&gExecuteList[j].jumpTarget==o){
                        gExecuteList[j].jumpTarget=i+1;break;
                    }
                }
                gExecuteList[i].jumpTarget=o;
            }
        }
        else if(t==CONTROL_REPEAT_END){
            if(!cs.empty()&&cs.top().second==CONTROL_REPEAT){
                int o=cs.top().first;cs.pop();
                gExecuteList[i].jumpTarget=o;gExecuteList[o].jumpTarget=i+1;
            }
        }
        else if(t==CONTROL_FOREVER_END){
            if(!cs.empty()&&cs.top().second==CONTROL_FOREVER){
                int o=cs.top().first;cs.pop();
                gExecuteList[i].jumpTarget=o;gExecuteList[o].jumpTarget=i+1;
            }
        }
    }
}

// ============================================================
//  SECTION 11: EXECUTION ENGINE
// ============================================================

void executeBlock(BlockType type,int idx){
    gCycleCount++;
    if(gCycleCount>WATCHDOG_LIMIT){
        logEvent(LOG_ERROR,idx,"WATCHDOG","Infinite loop detected - halted!");
        gIsExecuting=false;gCycleCount=0;return;
    }
    auto& list=gExecuteList;
    auto& ei=gExecuteIndex;
    switch(type){
        case START: logEvent(LOG_INFO,idx,"START","Execution started"); break;
        case CONTROL_WAIT:{int ms=1000;try{ms=(int)(stof(list[idx].inputValue1)*1000);}catch(...){}SDL_Delay(max(0,ms));break;}
        case CONTROL_REPEAT:{
            int count=1;try{count=stoi(list[idx].inputValue1);}catch(...){}
            if(count<=0){if(list[idx].jumpTarget>=0)ei=list[idx].jumpTarget-1;}
            else gLoopStack.push(count);
            break;
        }
        case CONTROL_REPEAT_END:{
            if(!gLoopStack.empty()){
                int rem=gLoopStack.top()-1;
                if(rem>0){gLoopStack.top()=rem;ei=list[idx].jumpTarget;}
                else gLoopStack.pop();
            }
            break;
        }
        case CONTROL_FOREVER: break;
        case CONTROL_FOREVER_END: ei=list[idx].jumpTarget; break;
        case CONTROL_IF:{
            bool ok=evalCondition(list[idx].inputValue1);
            if(!ok){if(list[idx].jumpTarget>=0)ei=list[idx].jumpTarget-1;logEvent(LOG_INFO,idx,"IF","FALSE");}
            else logEvent(LOG_INFO,idx,"IF","TRUE");
            break;
        }
        case CONTROL_IF_END: break;
        case CONTROL_IF_ELSE:{
            bool ok=evalCondition(list[idx].inputValue1);
            if(ok){logEvent(LOG_INFO,idx,"IF_ELSE","TRUE");}
            else{if(list[idx].jumpTarget2>=0)ei=list[idx].jumpTarget2-1;logEvent(LOG_INFO,idx,"IF_ELSE","FALSE");}
            break;
        }
        case CONTROL_ELSE:{if(list[idx].jumpTarget>=0)ei=list[idx].jumpTarget-1;break;}
        case CONTROL_IF_ELSE_END: break;
        case CONTROL_STOP_ALL: gIsExecuting=false;ei=0;break;
        case MOTION_SETX:      try{setSpiritX(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_SETY:      try{setSpiritY(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_MOVENSTEP: try{moveNSteps(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_TURNNDEG:  try{turnNDeg(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_TURNRIGHT: turnRight();break;
        case MOTION_TURNLEFT:  turnLeft();break;
        case MOTION_CHANGEXBY: try{changeXBy(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_CHANGEYBY: try{changeYBy(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_CHANGEXYBY:try{goToXY(stoi(list[idx].inputValue1),stoi(list[idx].inputValue2));}catch(...){}break;
        case MOTION_RANDOMPOS: goRandom();break;
        case MOTION_CHANGEDEG: try{pointInDir(stoi(list[idx].inputValue1));}catch(...){}break;
        case MOTION_IFONEDGE:  ifOnEdge();break;
        case LOOKS_SAY:        setSay(list[idx].inputValue1);break;
        case LOOKS_SAY_SECS:{int ms=2000;try{ms=(int)(stof(list[idx].inputValue2)*1000);}catch(...){}setSay(list[idx].inputValue1,ms);break;}
        case LOOKS_THINK:      setThink(list[idx].inputValue1);break;
        case LOOKS_SHOW:       gSpirit.isShow=true;break;
        case LOOKS_HIDE:       gSpirit.isShow=false;break;
        case LOOKS_SET_SIZE:   try{setSize(stoi(list[idx].inputValue1));}catch(...){}break;
        case LOOKS_CHANGE_SIZE:try{changeSize(stoi(list[idx].inputValue1));}catch(...){}break;
        case LOOKS_CLEAR_EFFECTS: clearEffects();break;
        case PEN_DOWN:     penDown();break;
        case PEN_UP:       penUp();break;
        case PEN_ERASE_ALL:penErase();break;
        case PEN_STAMP:    penStamp();break;
        case PEN_SET_SIZE: try{penSetSize(stoi(list[idx].inputValue1));}catch(...){}break;
        case PEN_CHANGE_SIZE:try{penChgSize(stoi(list[idx].inputValue1));}catch(...){}break;
        case VAR_SET:    setVar(list[idx].inputValue1,list[idx].inputValue2);break;
        case VAR_CHANGE: try{changeVar(list[idx].inputValue1,stof(list[idx].inputValue2));}catch(...){}break;
        case SOUND_PLAY_GAMEVOICE:
            playSound(0);
            logEvent(LOG_INFO,idx,"SND_GAMEVOICE","vol="+to_string(gSoundSettings[0].volume)+" pitch="+to_string(gSoundSettings[0].pitch));
            break;
        case SOUND_PLAY_SHEYPOR:
            playSound(1);
            logEvent(LOG_INFO,idx,"SND_SHEYPOR","vol="+to_string(gSoundSettings[1].volume)+" pitch="+to_string(gSoundSettings[1].pitch));
            break;
        case SOUND_PLAY_OPENING:
            playSound(2);
            logEvent(LOG_INFO,idx,"SND_OPENING","vol="+to_string(gSoundSettings[2].volume)+" pitch="+to_string(gSoundSettings[2].pitch));
            break;
        case SENSING_RESET_TIMER: gTimerStartTick=SDL_GetTicks();break;
        case SENSING_MOUSE_X: setVar("mouse_x",to_string(gMouse.x));break;
        case SENSING_MOUSE_Y: setVar("mouse_y",to_string(gMouse.y));break;
        case EVENT_KEY_PRESSED:{
            string wk=list[idx].inputValue1;
            const Uint8* ks=SDL_GetKeyboardState(NULL);
            bool pressed=false;
            if(wk=="space") pressed=ks[SDL_SCANCODE_SPACE];
            else if(wk=="up")    pressed=ks[SDL_SCANCODE_UP];
            else if(wk=="down")  pressed=ks[SDL_SCANCODE_DOWN];
            else if(wk=="left")  pressed=ks[SDL_SCANCODE_LEFT];
            else if(wk=="right") pressed=ks[SDL_SCANCODE_RIGHT];
            else if(wk=="enter") pressed=ks[SDL_SCANCODE_RETURN];
            else if(wk.size()==1&&wk[0]>='a'&&wk[0]<='z') pressed=ks[SDL_SCANCODE_A+(wk[0]-'a')];
            else if(wk.size()==1&&wk[0]>='0'&&wk[0]<='9') pressed=ks[SDL_SCANCODE_0+(wk[0]-'0')];
            setVar("key_"+wk,pressed?"true":"false");
            setVar("last_key",gLastPressedKey);
            break;
        }
        case OP_ADD:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);setVar(rv,to_string(a+b));break;}
        case OP_SUBTRACT:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);setVar(rv,to_string(a-b));break;}
        case OP_MULTIPLY:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);setVar(rv,to_string(a*b));break;}
        case OP_DIVIDE:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);if(b==0)break;setVar(rv,to_string(a/b));break;}
        case OP_EQUAL:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);setVar(rv,(a==b)?"true":"false");break;}
        case OP_LESS:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);setVar(rv,(a<b)?"true":"false");break;}
        case OP_GREATER:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;float a=toFloat(list[idx].inputValue1),b=toFloat(list[idx].inputValue2);setVar(rv,(a>b)?"true":"false");break;}
        case OP_AND:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;bool a=evalCondition(list[idx].inputValue1),b=evalCondition(list[idx].inputValue2);setVar(rv,(a&&b)?"true":"false");break;}
        case OP_OR:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;bool a=evalCondition(list[idx].inputValue1),b=evalCondition(list[idx].inputValue2);setVar(rv,(a||b)?"true":"false");break;}
        case OP_NOT:{string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;bool a=evalCondition(list[idx].inputValue1);setVar(rv,(!a)?"true":"false");break;}
            // ── Math single-input operators ──────────────────────────
        case OP_ABS:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            if(!isNumericToken(list[idx].inputValue1)){opError(idx,"ABS: ورودی باید عدد باشد");break;}
            setVar(rv,to_string(fabs(toFloat(list[idx].inputValue1))));break;}
        case OP_SQRT:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            if(!isNumericToken(list[idx].inputValue1)){opError(idx,"SQRT: ورودی باید عدد باشد");break;}
            float v=toFloat(list[idx].inputValue1);
            if(v<0){opError(idx,"SQRT: جذر عدد منفی تعریف نشده است (ورودی="+to_string(v)+")");break;}
            setVar(rv,to_string(sqrt(v)));break;}
        case OP_FLOOR:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            if(!isNumericToken(list[idx].inputValue1)){opError(idx,"FLOOR: ورودی باید عدد باشد");break;}
            setVar(rv,to_string((int)floor(toFloat(list[idx].inputValue1))));break;}
        case OP_CEIL:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            if(!isNumericToken(list[idx].inputValue1)){opError(idx,"CEIL: ورودی باید عدد باشد");break;}
            setVar(rv,to_string((int)ceil(toFloat(list[idx].inputValue1))));break;}
        case OP_SIN:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            if(!isNumericToken(list[idx].inputValue1)){opError(idx,"SIN: ورودی باید درجه عددی باشد");break;}
            float deg=toFloat(list[idx].inputValue1);
            setVar(rv,to_string(sin(deg*M_PI/180.0)));break;}
        case OP_COS:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            if(!isNumericToken(list[idx].inputValue1)){opError(idx,"COS: ورودی باید درجه عددی باشد");break;}
            float deg=toFloat(list[idx].inputValue1);
            setVar(rv,to_string(cos(deg*M_PI/180.0)));break;}
            // ── String operators ─────────────────────────────────────
        case OP_STR_LEN:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            string s=resolveStr(list[idx].inputValue1);
            setVar(rv,to_string((int)s.size()));break;}
        case OP_STR_CHAR:{
            // inputValue1 = رشته  |  inputValue2 = ایندکس (از ۱)
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            string s=resolveStr(list[idx].inputValue1);
            if(!isNumericToken(list[idx].inputValue2)){opError(idx,"CHAR_AT: ایندکس باید عدد باشد");break;}
            int n=(int)toFloat(list[idx].inputValue2);
            if(n<1||n>(int)s.size()){
                opError(idx,"CHAR_AT: ایندکس "+to_string(n)+" خارج از محدوده رشته (طول="+to_string(s.size())+")");break;}
            setVar(rv,string(1,s[n-1]));break;}
        case OP_STR_JOIN:{
            string rv=list[idx].inputValue3.empty()?"op_result":list[idx].inputValue3;
            string a=resolveStr(list[idx].inputValue1);
            string b=resolveStr(list[idx].inputValue2);
            setVar(rv,a+b);break;}
            // ── Motion: go to mouse ───────────────────────────────────
        case MOTION_GOTO_MOUSE:{
            // تبدیل مختصات موس در PREVIEW به مختصات صحنه
            int mx=gMouse.x,my=gMouse.y;
            int sx=(int)((mx-1320)/(float)580*DW);
            int sy=(int)((my-190)/(float)420*DH);
            sx=max(0,min(DW-1,sx));sy=max(0,min(DH-1,sy));
            goToXY(sx,sy);
            logEvent(LOG_INFO,idx,"GOTO_MOUSE","→ ("+to_string(sx)+","+to_string(sy)+")");break;}
        default: logEvent(LOG_WARNING,idx,"UNKNOWN","Block "+to_string((int)type));break;
    }
}

// ============================================================
//  SECTION 12: BLOCK LIBRARY
// ============================================================

vector<pair<string,vector<Block>>>& getBlockLibrary(){
    static vector<pair<string,vector<Block>>> lib={
            {"Motion",{
                              {"Motion","Set X",      gTheme["bgMotion"],MOTION_SETX,1},
                              {"Motion","Set Y",      gTheme["bgMotion"],MOTION_SETY,1},
                              {"Motion","Move Steps", gTheme["bgMotion"],MOTION_MOVENSTEP,1},
                              {"Motion","Turn Deg",   gTheme["bgMotion"],MOTION_TURNNDEG,1},
                              {"Motion","Point Dir",  gTheme["bgMotion"],MOTION_CHANGEDEG,1},
                              {"Motion","Turn Right", gTheme["bgMotion"],MOTION_TURNRIGHT},
                              {"Motion","Turn Left",  gTheme["bgMotion"],MOTION_TURNLEFT},
                              {"Motion","Chg X By",   gTheme["bgMotion"],MOTION_CHANGEXBY,1},
                              {"Motion","Chg Y By",   gTheme["bgMotion"],MOTION_CHANGEYBY,1},
                              {"Motion","Go To X&Y",  gTheme["bgMotion"],MOTION_CHANGEXYBY,2},
                              {"Motion","Rand Pos",   gTheme["bgMotion"],MOTION_RANDOMPOS},
                              {"Motion","If On Edge", gTheme["bgMotion"],MOTION_IFONEDGE},
                              {"Motion","Goto Mouse", gTheme["bgMotion"],MOTION_GOTO_MOUSE},
                      }},
            {"Looks",{
                              {"Looks","Say",         gTheme["bgLooks"],LOOKS_SAY,1},
                              {"Looks","Say Secs",    gTheme["bgLooks"],LOOKS_SAY_SECS,2},
                              {"Looks","Think",       gTheme["bgLooks"],LOOKS_THINK,1},
                              {"Looks","Show",        gTheme["bgLooks"],LOOKS_SHOW},
                              {"Looks","Hide",        gTheme["bgLooks"],LOOKS_HIDE},
                              {"Looks","Set Size %",  gTheme["bgLooks"],LOOKS_SET_SIZE,1},
                              {"Looks","Chg Size",    gTheme["bgLooks"],LOOKS_CHANGE_SIZE,1},
                              {"Looks","Clear FX",    gTheme["bgLooks"],LOOKS_CLEAR_EFFECTS},
                      }},
            {"Sound",{
                              {"Sound","Play",          gTheme["bgSound"],SOUND_PLAY},
                              {"Sound","Stop All",      gTheme["bgSound"],SOUND_STOP_ALL},
                              {"Sound","Game Voice",    gTheme["bgSound"],SOUND_PLAY_GAMEVOICE},
                              {"Sound","Sheypor",       gTheme["bgSound"],SOUND_PLAY_SHEYPOR},
                              {"Sound","Opening",       gTheme["bgSound"],SOUND_PLAY_OPENING},
                      }},
            {"Event",{
                              {"Event","Start",       gTheme["bgEvent"],START},
                              {"Event","Key Press",   gTheme["bgEvent"],EVENT_KEY_PRESSED,1},
                      }},
            {"Control",{
                              {"Control","Wait",        gTheme["bgControl"],CONTROL_WAIT,1},
                              {"Control","Repeat",      gTheme["bgControl"],CONTROL_REPEAT,1},
                              {"Control","Repeat End",  gTheme["bgControl"],CONTROL_REPEAT_END},
                              {"Control","Forever",     gTheme["bgControl"],CONTROL_FOREVER},
                              {"Control","Frevr End",   gTheme["bgControl"],CONTROL_FOREVER_END},
                              {"Control","If",          gTheme["bgControl"],CONTROL_IF,1},
                              {"Control","If End",      gTheme["bgControl"],CONTROL_IF_END},
                              {"Control","If-Else",     gTheme["bgControl"],CONTROL_IF_ELSE,1},
                              {"Control","Else",        gTheme["bgControl"],CONTROL_ELSE},
                              {"Control","IfElse End",  gTheme["bgControl"],CONTROL_IF_ELSE_END},
                              {"Control","Stop All",    gTheme["bgControl"],CONTROL_STOP_ALL},
                      }},
            {"Sensing",{
                              {"Sensing","Reset Timer", gTheme["bgSensing"],SENSING_RESET_TIMER},
                              {"Sensing","Mouse X",     gTheme["bgSensing"],SENSING_MOUSE_X},
                              {"Sensing","Mouse Y",     gTheme["bgSensing"],SENSING_MOUSE_Y},
                      }},
            {"Operators",{
                              {"Operators","+ Add",    gTheme["bgOperators"],OP_ADD,2},
                              {"Operators","- Sub",    gTheme["bgOperators"],OP_SUBTRACT,2},
                              {"Operators","* Mul",    gTheme["bgOperators"],OP_MULTIPLY,2},
                              {"Operators","/ Div",    gTheme["bgOperators"],OP_DIVIDE,2},
                              {"Operators","= Equal",  gTheme["bgOperators"],OP_EQUAL,2},
                              {"Operators","< Less",   gTheme["bgOperators"],OP_LESS,2},
                              {"Operators","> Greater",gTheme["bgOperators"],OP_GREATER,2},
                              {"Operators","AND",      gTheme["bgOperators"],OP_AND,2},
                              {"Operators","OR",       gTheme["bgOperators"],OP_OR,2},
                              {"Operators","NOT",      gTheme["bgOperators"],OP_NOT,1},
                              {"Operators","Abs",      gTheme["bgOperators"],OP_ABS,1},
                              {"Operators","Sqrt",     gTheme["bgOperators"],OP_SQRT,1},
                              {"Operators","Floor",    gTheme["bgOperators"],OP_FLOOR,1},
                              {"Operators","Ceil",     gTheme["bgOperators"],OP_CEIL,1},
                              {"Operators","Sin(deg)", gTheme["bgOperators"],OP_SIN,1},
                              {"Operators","Cos(deg)", gTheme["bgOperators"],OP_COS,1},
                              {"Operators","Str Len",  gTheme["bgOperators"],OP_STR_LEN,1},
                              {"Operators","Char At",  gTheme["bgOperators"],OP_STR_CHAR,2},
                              {"Operators","Join Str", gTheme["bgOperators"],OP_STR_JOIN,2},
                      }},
            {"Variable",{
                              {"Variable","Set Var",   gTheme["bgVariable"],VAR_SET,2},
                              {"Variable","Chg Var",   gTheme["bgVariable"],VAR_CHANGE,2},
                      }},
            {"MyBlocks",{
                              {"MyBlocks","Define",    gTheme["bgMyBlocks"],MYBLOCK_DEFINE,1},
                              {"MyBlocks","Call",      gTheme["bgMyBlocks"],MYBLOCK_CALL,1},
                      }},
            {"Pen",{
                              {"Pen","Pen Down",       gTheme["bgPen"],PEN_DOWN},
                              {"Pen","Pen Up",         gTheme["bgPen"],PEN_UP},
                              {"Pen","Erase All",      gTheme["bgPen"],PEN_ERASE_ALL},
                              {"Pen","Stamp",          gTheme["bgPen"],PEN_STAMP},
                              {"Pen","Set Size",       gTheme["bgPen"],PEN_SET_SIZE,1},
                              {"Pen","Chg Size",       gTheme["bgPen"],PEN_CHANGE_SIZE,1},
                      }},
    };
    return lib;
}

// ============================================================
//  SECTION 13: RENDERING
// ============================================================

static bool isConditionBlock(BlockType t){return t==CONTROL_IF||t==CONTROL_IF_ELSE;}

static void renderInputBox(int x,int y,int w,int h,const string& val,bool active){
    SDL_Color white={255,255,255,255},black={0,0,0,255};
    SDL_Color border=active?SDL_Color{255,200,0,255}:black;
    drawRectBorder(x,y,w,h,white,border,2);
    if(!val.empty()){int vw,vh;TTF_SizeUTF8(font,val.c_str(),&vw,&vh);renderText(val,x+(w-vw)/2,y+(h-vh)/2,black);}
}

void renderBlock(Block& block,int x,int y,bool isInMenu){
    block.x=x;block.y=y;
    int tw,th;TTF_SizeUTF8(font,block.blockName.c_str(),&tw,&th);
    SDL_Color bg=block.bgColor;
    if(block.type==CONTROL_ELSE)         bg=changeBrightness(bg,72);
    if(block.type==CONTROL_IF_ELSE_END)  bg=changeBrightness(bg,80);
    drawRectBorder(x+25,y+30,240,80,bg,changeBrightness(bg,85),2);
    if(block.inputCount>0){
        renderText(block.blockName,x+35,y+30+(80-th)/2,gTheme["textPrimary"]);
        bool isCond=isConditionBlock(block.type);
        int inW  = isCond ? 110 : (block.inputCount==1?70:45);
        int in1X = isCond ? (x+120) : (block.inputCount==1?(x+160):(x+140));
        int inY  = y+45;
        renderInputBox(in1X,inY,inW,50,block.inputValue1,block.isInput1Active);
        if(block.inputCount>=2&&!isCond)
            renderInputBox(x+195,inY,45,50,block.inputValue2,block.isInput2Active);
        if(!isInMenu&&isCond)
            renderText("cond: x>5  a==b  flag AND ok  NOT done",x+28,y+115,{90,90,90,200},fontSmall);
        if(!isInMenu&&isOpBlock(block.type)&&!block.inputValue3.empty()){
            string label="-> "+block.inputValue3;
            renderText(label,x+55,y+112,{30,160,30,230},fontSmall);
        }
    } else {
        renderText(block.blockName,x+25+(240-tw)/2,y+30+(80-th)/2,gTheme["textPrimary"]);
    }
    if(!isInMenu&&gIsExecuting&&gExecuteIndex<(int)gExecuteList.size()){
        for(int i=0;i<(int)gExecuteList.size();i++){
            if(&gExecuteList[i]==&block&&i==gExecuteIndex){
                SDL_SetRenderDrawColor(m_renderer,255,255,0,100);
                SDL_Rect hl={x+25,y+30,240,80};SDL_RenderFillRect(m_renderer,&hl);break;
            }
        }
    }
    if(isInMenu){
        bool inside=isInRect({x+25,y+30,240,80},gMouse.x,gMouse.y);
        if(inside&&gMouse.isClicked){gIsDragging=true;gDraggedBlock=block;gDraggedBlockIndex=-1;gMouse.isClicked=false;}
    }
    if(!isInMenu){
        bool inside=isInRect({x+25,y+30,240,80},gMouse.x,gMouse.y);
        bool inInput=false;
        if(block.inputCount>0){
            bool isCond=isConditionBlock(block.type);
            int inW=isCond?110:(block.inputCount==1?70:45);
            int in1X=isCond?(block.x+120):(block.inputCount==1?(block.x+160):(block.x+140));
            inInput=isInRect({in1X,block.y+45,inW,50},gMouse.x,gMouse.y);
            if(block.inputCount>=2&&!isCond)
                inInput=inInput||isInRect({block.x+195,block.y+45,45,50},gMouse.x,gMouse.y);
        }
        if(inside&&!inInput&&gMouse.isClicked&&!gIsDragging){
            gIsDragging=true;gDraggedBlock=block;
            for(int i=0;i<(int)gExecuteList.size();i++)
                if(&gExecuteList[i]==&block){gDraggedBlockIndex=i;break;}
            gMouse.isClicked=false;
        }
    }
}

static const SDL_Rect PREVIEW={1320,190,580,420};

void renderPenTrails(){
    for(int i=1;i<(int)gPenTrail.size();i++){
        auto& a=gPenTrail[i-1];auto& b=gPenTrail[i];
        SDL_SetRenderDrawColor(m_renderer,b.color.r,b.color.g,b.color.b,b.color.a);
        int ax=PREVIEW.x+a.x*PREVIEW.w/DW,ay=PREVIEW.y+a.y*PREVIEW.h/DH;
        int bx=PREVIEW.x+b.x*PREVIEW.w/DW,by=PREVIEW.y+b.y*PREVIEW.h/DH;
        int hs=b.size/2;
        for(int s=-hs;s<=hs;s++){SDL_RenderDrawLine(m_renderer,ax+s,ay,bx+s,by);SDL_RenderDrawLine(m_renderer,ax,ay+s,bx,by+s);}
    }
}
void renderSpeechBubble(){
    Uint32 now=SDL_GetTicks();
    if(gSpirit.sayUntil>0&&now>gSpirit.sayUntil){gSpirit.sayText="";gSpirit.sayUntil=0;}
    if(gSpirit.thinkUntil>0&&now>gSpirit.thinkUntil){gSpirit.thinkText="";gSpirit.thinkUntil=0;}
    string txt=!gSpirit.sayText.empty()?gSpirit.sayText:gSpirit.thinkText;
    if(txt.empty()) return;
    int sx=PREVIEW.x+(int)(gSpirit.spiritX/(float)DW*PREVIEW.w);
    int sy=PREVIEW.y+(int)(gSpirit.spiritY/(float)DH*PREVIEW.h);
    int bw=(int)txt.size()*8+20,bh=36;
    int bx=max(PREVIEW.x+5,min(PREVIEW.x+PREVIEW.w-bw-5,sx+10));
    int by=max(PREVIEW.y+5,min(PREVIEW.y+PREVIEW.h-bh-5,sy-60));
    drawRectBorder(bx,by,bw,bh,{255,255,255,240},{0,0,0,255},2);
    renderText(txt,bx+8,by+8,{0,0,0,255},fontSmall);
}
void renderBlockTypeIcon(const string& name,int x,int y,int r,SDL_Color color){
    drawCircleWithBorder(x,y,r,color,changeBrightness(color,85),3);
    int tw=0,th=0;if(fontSmall)TTF_SizeUTF8(fontSmall,name.c_str(),&tw,&th);
    renderText(name,x-tw/2,y+r+6,color,fontSmall);
}
void renderScrollableMenu(int x,int y,int w,int h){
    drawRectBorder(x,y,w,h,gTheme["bgWhite"],gTheme["bgSecondary"],2);
    SDL_Rect clip={x,y,w,h};SDL_RenderSetClipRect(m_renderer,&clip);
    int cy=y-gMouse.scrolloffsetCodeMenu;
    for(auto& cat:getBlockLibrary()){
        if(cat.first!=gSelectedBlockType) continue;
        for(auto& block:cat.second){renderBlock(const_cast<Block&>(block),x,cy,true);cy+=100;}
        break;
    }
    SDL_RenderSetClipRect(m_renderer,NULL);
}
void renderPlayground(int x,int y,int w,int h){
    drawRectBorder(x,y,w,h,gTheme["bgWhite"],gTheme["bgSecondary"],2);
    SDL_Rect clip={x,y,w,h};SDL_RenderSetClipRect(m_renderer,&clip);
    int cy=y-gMouse.scrolloffsetBlocksPlayGround;
    int cx=x+w/2-120;
    for(int i=0;i<(int)gExecuteList.size();i++){
        if(gIsDragging&&i==gDraggedBlockIndex){cy+=80;continue;}
        int bh=isConditionBlock(gExecuteList[i].type)?110:80;
        renderBlock(gExecuteList[i],cx,cy,false);
        cy+=bh;
    }
    if(gIsDragging){
        int relY=gMouse.y-y+gMouse.scrolloffsetBlocksPlayGround;
        int idx=max(0,min((int)gExecuteList.size(),relY/80));
        int iy=y-gMouse.scrolloffsetBlocksPlayGround+idx*80;
        SDL_SetRenderDrawColor(m_renderer,0,120,255,200);
        SDL_Rect ind={x+50,iy+20,w-100,4};SDL_RenderFillRect(m_renderer,&ind);
    }
    SDL_RenderSetClipRect(m_renderer,NULL);
}
void renderCodePanel(){
    drawRectBorder(20,190,100,840,gTheme["bgWhite"],gTheme["bgSecondary"],2);
    int spacer=0;
    for(auto& cat:getBlockLibrary()){
        SDL_Color cc=cat.second.empty()?gTheme["bgSecondary"]:cat.second[0].bgColor;
        if(cat.first==gSelectedBlockType) drawRect(20,205+spacer,100,50,{200,220,255,100});
        renderBlockTypeIcon(cat.first,65,230+spacer,22,cc);
        if(isInRect({20,205+spacer,100,50},gMouse.x,gMouse.y)&&gMouse.isClicked){
            gSelectedBlockType=cat.first;gMouse.scrolloffsetCodeMenu=0;
        }
        spacer+=75;
    }
    renderScrollableMenu(110,190,300,840);
    renderPlayground(400,190,900,840);
    if(gIsDragging) renderBlock(gDraggedBlock,gMouse.x-120,gMouse.y-40,false);
    if(!gVariables.empty()){
        int vx=410,vy=200;
        for(auto& v:gVariables){
            drawRectBorder(vx,vy,200,28,{255,220,50,210},{200,150,0,255},1);
            renderText(v.name+" = "+v.value,vx+5,vy+5,{0,0,0,255},fontSmall);vy+=32;
        }
    }
    renderText("If cond: x>5  a==b  AND  NOT  OR  or type op_result_N from operator block",
               415,165,{80,80,80,255},fontSmall);
}

// ─────────────────────────────────────────────────────────────
//  SOUNDS TAB  –  clean card layout with text inputs
//  Click a field → type number → Enter to apply
// ─────────────────────────────────────────────────────────────

// Helper: small labeled text-input box. Returns true if just clicked.
static bool renderLabeledInput(const string& label,
                               int x, int y, int w,
                               const string& val, bool active,
                               SDL_Color accentColor)
{
    renderText(label, x, y, {70,70,70,255}, fontSmall);
    SDL_Rect box = {x + 70, y - 4, w, 26};
    SDL_Color border = active ? accentColor : SDL_Color{190,190,190,255};
    drawRectBorder(box.x, box.y, box.w, box.h, {255,255,255,255}, border, 2);
    SDL_Color tc = val.empty() ? SDL_Color{200,200,200,255} : SDL_Color{30,30,30,255};
    renderText(val.empty() ? "..." : val, box.x+6, box.y+5, tc, fontSmall);
    return isInRect(box, gMouse.x, gMouse.y) && gMouse.isClicked;
}

void renderSoundsPanel(){
    drawRectBorder(110, 190, 1190, 840, gTheme["bgWhite"], gTheme["bgSecondary"], 2);

    const int CARD_X = 130, CARD_Y = 230;
    const int CARD_W = 330, CARD_H = 160, GAP = 36;
    SDL_Color purpleBorder = gTheme["bgSound"];

    for(int i = 0; i < 3; i++){
        SoundSettings& s = gSoundSettings[i];
        int cx = CARD_X + i * (CARD_W + GAP);
        int cy = CARD_Y;

        SDL_Color cardBg = {248, 238, 255, 255};
        drawRectBorder(cx, cy, CARD_W, CARD_H, cardBg, purpleBorder, 2);

        // Sound name only (no filepath)
        renderText(s.name, cx+12, cy+14, {60, 0, 100, 255});

        // Play button – text only, no ▶ rectangle artifact
        SDL_Rect playBtn = {cx + CARD_W - 68, cy + 8, 58, 26};
        bool phov = isInRect(playBtn, gMouse.x, gMouse.y);
        SDL_Color pCol = phov ? SDL_Color{55,195,75,255} : SDL_Color{89,192,89,255};
        drawRectBorder(playBtn.x, playBtn.y, playBtn.w, playBtn.h, pCol, {35,140,35,255}, 2);
        int ptw,pth; TTF_SizeUTF8(fontSmall,"Play",&ptw,&pth);
        renderText("Play", playBtn.x+(playBtn.w-ptw)/2, playBtn.y+(playBtn.h-pth)/2, {255,255,255,255}, fontSmall);
        if(phov && gMouse.isClicked){ gMouse.isClicked = false; playSound(i); }

        bool volActive   = (gSndInput.activeVolInput   == i);
        bool pitchActive = (gSndInput.activePitchInput == i);

        // Volume row  – label only on left, no hint text on right
        if(renderLabeledInput("Volume:", cx+12, cy+68, 68,
                              gSndInput.volStr[i], volActive, {200,100,200,255}))
        {
            gMouse.isClicked = false;
            gSndInput.activeVolInput   = i;
            gSndInput.activePitchInput = -1;
            SDL_StartTextInput();
        }

        // Pitch row
        if(renderLabeledInput("Pitch:", cx+12, cy+108, 68,
                              gSndInput.pitchStr[i], pitchActive, {80,140,220,255}))
        {
            gMouse.isClicked = false;
            gSndInput.activePitchInput = i;
            gSndInput.activeVolInput   = -1;
            SDL_StartTextInput();
        }
    }
}

void renderModeSelector(vector<Button>& modes){
    int spacer=20;
    for(Button& btn:modes){
        int origX=btn.rect.x;btn.rect.x=origX+spacer;
        if(string(btn.text)==gSelectedMode){btn.currentColor=btn.focusColor;btn.currentTextColor=btn.focusTextColor;}
        else{bool hov=isInRect(btn.rect,gMouse.x,gMouse.y);btn.currentColor=hov?btn.hoverColor:btn.normalColor;btn.currentTextColor=btn.textColor;}
        renderButton(btn);
        if(isInRect(btn.rect,gMouse.x,gMouse.y)&&gMouse.isMouseDown) gSelectedMode=btn.text;
        spacer+=btn.rect.w+10;btn.rect.x=origX;
    }
}
void renderGamePreview(){
    drawRectBorder(PREVIEW.x,PREVIEW.y,PREVIEW.w,PREVIEW.h,gTheme["bgWhite"],gTheme["bgSecondary"],2);
    renderPenTrails();
    if(gSpirit.isShow&&gSpirit.texture){
        float sc=gSpirit.size/100.0f,fw=48.0f*sc,fh=48.0f*sc;
        SDL_FRect dst={PREVIEW.x+(gSpirit.spiritX/(float)DW)*PREVIEW.w-fw/2,PREVIEW.y+(gSpirit.spiritY/(float)DH)*PREVIEW.h-fh/2,fw,fh};
        SDL_RenderCopyExF(m_renderer,gSpirit.texture,nullptr,&dst,-(gSpirit.direction-90.0),nullptr,SDL_FLIP_NONE);
    }
    renderSpeechBubble();
    renderText("Preview",PREVIEW.x+5,PREVIEW.y+4,gTheme["textSecondary"],fontSmall);
}
void renderSpiritStats(){
    int px=1320,py=630,pw=580,ph=400;
    drawRectBorder(px,py,pw,ph,gTheme["bgWhite"],gTheme["bgSecondary"],2);
    SDL_Color tc=gTheme["textColor"];
    renderText("Name: "+gSpirit.spiritName,px+30,py+30,tc);
    renderText("X: "+to_string(gSpirit.spiritX),px+30,py+80,tc);
    renderText("Y: "+to_string(gSpirit.spiritY),px+210,py+80,tc);
    renderText("Dir: "+to_string(gSpirit.direction),px+30,py+130,tc);
    renderText("Size: "+to_string(gSpirit.size)+"%",px+210,py+130,tc);
    float ts=(SDL_GetTicks()-gTimerStartTick)/1000.0f;
    renderText("Timer: "+to_string((int)ts)+"s",px+30,py+180,tc);
    if(gIsPaused) renderText("[ PAUSED ]",px+220,py+180,gTheme["bgEvent"]);
    Button showBtn={{px+30,py+230,220,55},gSpirit.isShow?"Shown":"Hidden",
                    gTheme["bgSecondary"],gTheme["bgSecondary"],gTheme["primary"],gTheme["textSecondary"],gTheme["textPrimary"]};
    showBtn.currentColor=gSpirit.isShow?gTheme["primary"]:gTheme["bgSecondary"];showBtn.currentTextColor=gTheme["textPrimary"];
    if(isInRect(showBtn.rect,gMouse.x,gMouse.y)&&gMouse.isClicked){gMouse.isClicked=false;gSpirit.isShow^=true;}
    renderButton(showBtn);
    Button logBtn={{px+270,py+230,260,55},gShowLog?"Log ON":"Log OFF",
                   gTheme["bgSecondary"],gTheme["bgSecondary"],{50,50,200,255},gTheme["textSecondary"],gTheme["textPrimary"]};
    logBtn.currentColor=gShowLog?SDL_Color{50,50,200,255}:gTheme["bgSecondary"];logBtn.currentTextColor=gTheme["textPrimary"];
    if(isInRect(logBtn.rect,gMouse.x,gMouse.y)&&gMouse.isClicked){gMouse.isClicked=false;gShowLog^=true;}
    renderButton(logBtn);
    if(!gVariables.empty()){
        renderText("Variables:",px+30,py+305,tc,fontSmall);int vy=py+330;
        for(auto& v:gVariables){renderText(v.name+" = "+v.value,px+30,vy,tc,fontSmall);vy+=22;if(vy>py+ph-20)break;}
    }
}
static bool gShowFileMenu=false;
void renderMenu(){
    drawRect(0,0,DW,70,gTheme["primary"]);
    renderText("Scratch  Group 36",DW/2-90,22,gTheme["textPrimary"]);
    SDL_Rect fRect={10,15,80,40};
    SDL_Color fCol=gShowFileMenu?changeBrightness(gTheme["primary"],130):changeBrightness(gTheme["primary"],75);
    drawRectBorder(fRect.x,fRect.y,fRect.w,fRect.h,fCol,changeBrightness(fCol,70),2);
    renderText("File",fRect.x+18,fRect.y+10,gTheme["textPrimary"],fontSmall);
    if(isInRect(fRect,gMouse.x,gMouse.y)&&gMouse.isClicked){gMouse.isClicked=false;gShowFileMenu^=true;}
}
// Called LAST so dropdown renders above all other panels
void renderFileDropdown(){
    if(!gShowFileMenu) return;
    const char* opts[]={"New Project","Save  (F5)","Load  (F6)"};
    for(int i=0;i<3;i++){
        SDL_Rect r={10,71+i*45,160,44};bool hov=isInRect(r,gMouse.x,gMouse.y);
        drawRectBorder(r.x,r.y,r.w,r.h,hov?gTheme["primary"]:gTheme["bgWhite"],gTheme["bgSecondary"],1);
        renderText(opts[i],r.x+10,r.y+12,hov?gTheme["textPrimary"]:gTheme["textColor"],fontSmall);
        if(hov&&gMouse.isClicked){gMouse.isClicked=false;gShowFileMenu=false;
            if(i==0) newProject();
            else if(i==1) saveProject("project.scratch");
            else          loadProject("project.scratch");
        }
    }
}
void renderRunButtons(){
    SDL_Color runCol=gIsPaused?gTheme["bgEvent"]:gIsExecuting?SDL_Color{50,200,50,255}:gTheme["bgOperators"];
    drawCircleWithBorder(PREVIEW.x+25,150,25,runCol,changeBrightness(runCol,75),2);
    renderText(gIsPaused?"||":gIsExecuting?"...":"Go!",PREVIEW.x+10,140,gTheme["textPrimary"],fontSmall);
    if(isInRect({PREVIEW.x,125,50,50},gMouse.x,gMouse.y)&&gMouse.isClicked){
        gMouse.isClicked=false;
        if(!gIsExecuting){gIsExecuting=true;gIsPaused=false;gExecuteIndex=0;gCycleCount=0;
            while(!gLoopStack.empty())gLoopStack.pop();preProcessBlocks();}
        else if(gIsPaused)gIsPaused=false;else gIsPaused=true;
    }
    drawCircleWithBorder(PREVIEW.x+80,150,20,gTheme["red"],changeBrightness(gTheme["red"],75),2);
    renderText("Stop",PREVIEW.x+62,140,gTheme["textPrimary"],fontSmall);
    if(isInRect({PREVIEW.x+60,130,40,40},gMouse.x,gMouse.y)&&gMouse.isClicked){
        gMouse.isClicked=false;gIsExecuting=false;gIsPaused=false;gExecuteIndex=0;gCycleCount=0;
        while(!gLoopStack.empty())gLoopStack.pop();
    }
    Button dbg={{DW-220,115,200,60},gIsInDebug?"Debug ON":"Rush",
                gTheme["bgSecondary"],gTheme["bgWhite"],{160,60,255,255},gTheme["textSecondary"],gTheme["textPrimary"]};
    dbg.currentColor=gIsInDebug?SDL_Color{160,60,255,255}:gTheme["bgSecondary"];dbg.currentTextColor=gTheme["textPrimary"];
    if(isInRect(dbg.rect,gMouse.x,gMouse.y)&&gMouse.isClicked){gMouse.isClicked=false;gIsInDebug^=true;}
    renderButton(dbg);
    if(gIsExecuting)
        renderText(gIsPaused?"PAUSED @ block "+to_string(gExecuteIndex):"Running block "+to_string(gExecuteIndex),
                   PREVIEW.x+135,143,gTheme["textSecondary"],fontSmall);
}

// ============================================================
//  SECTION 14: EVENT HANDLING
// ============================================================

void handleEvents(SDL_Event& e){
    if(e.type==SDL_QUIT){gIsRunning=false;return;}

    // ── Sound panel: text input ───────────────────────────────
    if(e.type==SDL_TEXTINPUT && gSelectedMode=="Sounds"){
        char c=e.text.text[0];
        if(isdigit(c)){
            if(gSndInput.activeVolInput>=0){
                auto& s=gSndInput.volStr[gSndInput.activeVolInput];
                if(s.size()<4) s+=c;
            }
            if(gSndInput.activePitchInput>=0){
                auto& s=gSndInput.pitchStr[gSndInput.activePitchInput];
                if(s.size()<4) s+=c;
            }
        }
    }

    if(e.type==SDL_MOUSEBUTTONDOWN&&e.button.button==SDL_BUTTON_LEFT){
        int mx=e.button.x,my=e.button.y;bool hitInput=false;

        // ── کلیک روی اسپیریت در Preview → شروع اجرا ────────
        if(gSpirit.isShow&&gSpirit.texture&&isInRect(PREVIEW,mx,my)&&!gIsExecuting){
            float sc=gSpirit.size/100.0f,fw=48.0f*sc,fh=48.0f*sc;
            int sx=(int)(PREVIEW.x+(gSpirit.spiritX/(float)DW)*PREVIEW.w);
            int sy=(int)(PREVIEW.y+(gSpirit.spiritY/(float)DH)*PREVIEW.h);
            SDL_Rect sprRect={(int)(sx-fw/2),(int)(sy-fh/2),(int)fw,(int)fh};
            if(isInRect(sprRect,mx,my)){
                gIsExecuting=true;gIsPaused=false;gExecuteIndex=0;gCycleCount=0;
                while(!gLoopStack.empty())gLoopStack.pop();
                preProcessBlocks();
                logEvent(LOG_INFO,0,"SPRITE_CLICK","اجرا با کلیک روی اسپیریت شروع شد");
                gMouse.isClicked=false;
            }
        }

        for(auto& b:gExecuteList){
            if(b.inputCount<=0) continue;
            bool isCond=isConditionBlock(b.type);
            int inW=isCond?110:(b.inputCount==1?70:45);
            int in1X=isCond?(b.x+120):(b.inputCount==1?(b.x+160):(b.x+140));
            int inY=b.y+45;
            bool h1=isInRect({in1X,inY,inW,50},mx,my);
            bool h2=b.inputCount>=2&&!isCond&&isInRect({b.x+195,inY,45,50},mx,my);
            if(h1||h2){b.isInput1Active=h1;b.isInput2Active=h2;SDL_StartTextInput();hitInput=true;break;}
            else{b.isInput1Active=b.isInput2Active=false;}
        }
        if(!hitInput && gSelectedMode!="Sounds") SDL_StopTextInput();
    }
    else if(e.type==SDL_MOUSEBUTTONUP&&e.button.button==SDL_BUTTON_LEFT){
        if(gIsDragging){
            SDL_Rect playArea={400,190,900,840},menuArea={110,190,300,840};
            bool inPlay=isInRect(playArea,gMouse.x,gMouse.y);
            bool inMenu=isInRect(menuArea,gMouse.x,gMouse.y);
            if(gDraggedBlockIndex==-1){
                if(inPlay){int relY=gMouse.y-190+gMouse.scrolloffsetBlocksPlayGround;int idx=max(0,min((int)gExecuteList.size(),relY/80));assignOpResultName(gDraggedBlock);gExecuteList.insert(gExecuteList.begin()+idx,gDraggedBlock);}
            } else {
                if(inMenu){gExecuteList.erase(gExecuteList.begin()+gDraggedBlockIndex);}
                else if(inPlay){
                    int relY=gMouse.y-190+gMouse.scrolloffsetBlocksPlayGround;
                    int ni=max(0,min((int)gExecuteList.size()-1,relY/80)),oi=gDraggedBlockIndex;
                    if(ni!=oi){Block mv=gExecuteList[oi];gExecuteList.erase(gExecuteList.begin()+oi);if(ni>oi)ni--;gExecuteList.insert(gExecuteList.begin()+ni,mv);}
                }
                gDraggedBlockIndex=-1;
            }
            gIsDragging=false;
        }
    }
    else if(e.type==SDL_TEXTINPUT && gSelectedMode!="Sounds"){
        for(auto& b:gExecuteList){
            if(!b.isInput1Active&&!b.isInput2Active) continue;
            string* act=b.isInput1Active?&b.inputValue1:&b.inputValue2;
            char c=e.text.text[0];
            bool isCond=isConditionBlock(b.type)&&b.isInput1Active;
            bool isText=(b.type==LOOKS_SAY||b.type==LOOKS_SAY_SECS||b.type==LOOKS_THINK||
                         b.type==VAR_SET||b.type==VAR_CHANGE||b.type==MYBLOCK_DEFINE||b.type==MYBLOCK_CALL);
            if(isCond){
                bool ok=isalnum(c)||c==' '||c=='>'||c=='<'||c=='='||c=='!'||c=='.'||c=='-'||c=='_'||c=='('||c==')';
                if(ok&&(int)act->size()<30)*act+=c;
            } else if(isText){
                if((int)act->size()<24)*act+=c;
            } else {
                bool ok=isdigit(c)||(c=='-'&&act->empty())||(c=='.'&&act->find('.')==string::npos);
                if(ok&&(int)act->size()<8)*act+=c;
            }
            break;
        }
    }
    else if(e.type==SDL_KEYDOWN){
        // ── Sound panel key handling ──────────────────────────
        if(gSelectedMode=="Sounds"){
            if(e.key.keysym.sym==SDLK_BACKSPACE){
                if(gSndInput.activeVolInput>=0){
                    auto& s=gSndInput.volStr[gSndInput.activeVolInput];
                    if(!s.empty()) s.pop_back();
                }
                if(gSndInput.activePitchInput>=0){
                    auto& s=gSndInput.pitchStr[gSndInput.activePitchInput];
                    if(!s.empty()) s.pop_back();
                }
            }
            if(e.key.keysym.sym==SDLK_RETURN||e.key.keysym.sym==SDLK_KP_ENTER){
                // Apply all three sounds at once
                for(int i=0;i<3;i++){
                    try{int v=stoi(gSndInput.volStr[i]);   gSoundSettings[i].volume=max(0,min(128,v));}catch(...){}
                    try{int p=stoi(gSndInput.pitchStr[i]); gSoundSettings[i].pitch =max(50,min(200,p));}catch(...){}
                }
                gSndInput.activeVolInput=-1;
                gSndInput.activePitchInput=-1;
                SDL_StopTextInput();
            }
        }

        for(auto& b:gExecuteList){
            if(!b.isInput1Active&&!b.isInput2Active) continue;
            string* act=b.isInput1Active?&b.inputValue1:&b.inputValue2;
            if(e.key.keysym.sym==SDLK_BACKSPACE&&!act->empty()) act->pop_back();
            else if(e.key.keysym.sym==SDLK_RETURN||e.key.keysym.sym==SDLK_KP_ENTER){b.isInput1Active=b.isInput2Active=false;SDL_StopTextInput();}
            break;
        }
        if(e.key.repeat==0){
            SDL_Keycode sym=e.key.keysym.sym;
            if(sym==SDLK_SPACE) gLastPressedKey="space";
            else if(sym==SDLK_UP)    gLastPressedKey="up";
            else if(sym==SDLK_DOWN)  gLastPressedKey="down";
            else if(sym==SDLK_LEFT)  gLastPressedKey="left";
            else if(sym==SDLK_RIGHT) gLastPressedKey="right";
            else if(sym==SDLK_RETURN)gLastPressedKey="enter";
            else if(sym>=SDLK_a&&sym<=SDLK_z) gLastPressedKey=string(1,(char)('a'+sym-SDLK_a));
            else if(sym>=SDLK_0&&sym<=SDLK_9) gLastPressedKey=string(1,(char)('0'+sym-SDLK_0));
        }
        if(e.key.repeat==0){
            if(e.key.keysym.sym==SDLK_DELETE&&gSelectedMode=="Code"&&!gExecuteList.empty()) gExecuteList.pop_back();
            if(e.key.keysym.sym==SDLK_F5) saveProject("project.scratch");
            if(e.key.keysym.sym==SDLK_F6) loadProject("project.scratch");
            if(e.key.keysym.sym==SDLK_l)  gShowLog^=true;
            if(e.key.keysym.sym==SDLK_ESCAPE) gIsRunning=false;
            if(e.key.keysym.sym==SDLK_SPACE&&gIsExecuting&&gIsInDebug&&gIsPaused) gIsPaused=false;
        }
    }
}

// ============================================================
//  SECTION 15: INIT & MAIN LOOP
// ============================================================

bool init(){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO)<0) return false;
    if(SDL_CreateWindowAndRenderer(1920,1080,SDL_WINDOW_SHOWN|SDL_WINDOW_MAXIMIZED,&m_window,&m_renderer)<0) return false;
    if(TTF_Init()<0) return false;
    font=TTF_OpenFont("../assets/NotoSans-Medium.ttf",22);
    fontSmall=TTF_OpenFont("../assets/NotoSans-Medium.ttf",13);
    if(!font||!fontSmall) return false;
    IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);
    SDL_RaiseWindow(m_window);
    SDL_DisplayMode dm;SDL_GetCurrentDisplayMode(0,&dm);DW=dm.w;DH=dm.h;
    SDL_SetRenderDrawBlendMode(m_renderer,SDL_BLENDMODE_BLEND);
    if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,1,2048)<0)
        cout<<"Mix_OpenAudio Error: "<<Mix_GetError()<<"\n";
    else{
        gSoundGameVoice = Mix_LoadWAV("../assets/gamevoice.wav");
        gSoundSheypor   = Mix_LoadWAV("../assets/sheypor.wav");
        gSoundOpening   = Mix_LoadWAV("../assets/opening.wav");
        if(!gSoundGameVoice) cout<<"gamevoice load fail: "<<Mix_GetError()<<"\n";
        if(!gSoundSheypor)   cout<<"sheypor load fail: "  <<Mix_GetError()<<"\n";
        if(!gSoundOpening)   cout<<"opening load fail: "  <<Mix_GetError()<<"\n";
    }
    gTimerStartTick=SDL_GetTicks();loadSpiritTexture();
    return true;
}

int main(int argc,char* args[]){
    if(!init()){cout<<"Init failed!\n";return 1;}
    vector<Button> modes={
            {{0,80,150,50},"Code",     gTheme["bgSecondary"],changeBrightness(gTheme["bgSecondary"],110),gTheme["white"],gTheme["textSecondary"],gTheme["primary"]},
            {{0,80,150,50},"Costumes", gTheme["bgSecondary"],changeBrightness(gTheme["bgSecondary"],110),gTheme["white"],gTheme["textSecondary"],gTheme["primary"]},
            {{0,80,150,50},"Sounds",   gTheme["bgSecondary"],changeBrightness(gTheme["bgSecondary"],110),gTheme["white"],gTheme["textSecondary"],gTheme["primary"]},
    };
    bool isMouseDown=false;
    while(gIsRunning){
        if(gIsExecuting&&!gIsPaused){
            bool wSay=(gSpirit.sayUntil>0&&SDL_GetTicks()<gSpirit.sayUntil);
            bool wThink=(gSpirit.thinkUntil>0&&SDL_GetTicks()<gSpirit.thinkUntil);
            if(!wSay&&!wThink){
                if(gExecuteIndex<(int)gExecuteList.size()){
                    Block& cur=gExecuteList[gExecuteIndex];
                    if(cur.type!=START&&gExecuteIndex==0){gIsExecuting=false;gExecuteIndex=0;}
                    else{executeBlock(cur.type,gExecuteIndex);gExecuteIndex++;}
                    if(gIsInDebug) gIsPaused=true;
                } else {
                    cout<<"[Execution complete]\n";gIsExecuting=false;gExecuteIndex=0;gCycleCount=0;
                }
            }
        }
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            handleEvents(event);
            if(event.type==SDL_MOUSEBUTTONDOWN){isMouseDown=true;gMouse.isClicked=true;}
            if(event.type==SDL_MOUSEBUTTONUP){isMouseDown=false;gMouse.isClicked=false;}
            if(event.type==SDL_MOUSEWHEEL&&gSelectedMode=="Code"){
                if(isInRect({110,190,300,840},gMouse.x,gMouse.y)){gMouse.scrolloffsetCodeMenu-=event.wheel.y*20;gMouse.scrolloffsetCodeMenu=max(0,gMouse.scrolloffsetCodeMenu);}
                if(isInRect({400,190,900,840},gMouse.x,gMouse.y)){gMouse.scrolloffsetBlocksPlayGround-=event.wheel.y*20;gMouse.scrolloffsetBlocksPlayGround=max(0,gMouse.scrolloffsetBlocksPlayGround);}
            }
        }
        SDL_GetMouseState(&gMouse.x,&gMouse.y);
        gMouse.isMouseDown=isMouseDown;
        SDL_Color bg=gTheme["secondary"];SDL_SetRenderDrawColor(m_renderer,bg.r,bg.g,bg.b,bg.a);SDL_RenderClear(m_renderer);
        renderMenu();renderModeSelector(modes);
        if(gSelectedMode=="Code"){renderCodePanel();}
        else if(gSelectedMode=="Sounds"){renderSoundsPanel();}
        else{drawRectBorder(110,190,1190,840,gTheme["bgWhite"],gTheme["bgSecondary"],2);renderText(gSelectedMode+" panel - coming soon",500,500,gTheme["textSecondary"]);}
        renderRunButtons();renderGamePreview();renderSpiritStats();renderLogOverlay();
        // Save notification toast (always on top, 2 sec fade)
        if(gSaveNotifyTick>0&&SDL_GetTicks()-gSaveNotifyTick<2000){
            Uint32 elapsed=SDL_GetTicks()-gSaveNotifyTick;
            SDL_SetRenderDrawBlendMode(m_renderer,SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(m_renderer,30,160,60,200);
            SDL_Rect toast={10,75,200,34};SDL_RenderFillRect(m_renderer,&toast);
            renderText(gSaveNotifyMsg,toast.x+8,toast.y+8,{255,255,255,255},fontSmall);
            (void)elapsed;
        }
        renderFileDropdown();  // draw last so dropdown is always on top
        SDL_RenderPresent(m_renderer);
    }
    if(m_renderer) SDL_DestroyRenderer(m_renderer);
    if(m_window)   SDL_DestroyWindow(m_window);
    if(gSpirit.texture) SDL_DestroyTexture(gSpirit.texture);
    if(gSoundGameVoice) Mix_FreeChunk(gSoundGameVoice);
    if(gSoundSheypor)   Mix_FreeChunk(gSoundSheypor);
    if(gSoundOpening)   Mix_FreeChunk(gSoundOpening);
    Mix_CloseAudio();
    TTF_Quit();IMG_Quit();SDL_Quit();
    return 0;
}