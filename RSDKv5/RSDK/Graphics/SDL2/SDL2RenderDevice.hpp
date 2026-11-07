using ShaderEntry = ShaderEntryBase;

// SDL_Renderer can't run custom shaders, so this device can emulate the stock screen filters
// with plain GPU operations instead (texture filtering + a multiplied scanline overlay).
// On by default for the Wii U, where this renderer is the only one available.
#ifndef RETRO_SDL2_SCREEN_FILTERS
#define RETRO_SDL2_SCREEN_FILTERS (RETRO_PLATFORM == RETRO_WIIU)
#endif

class RenderDevice : public RenderDeviceBase
{
public:
    struct WindowInfo {
        union {
            struct {
                // i wanna do uint32 : 32 but idk if other compilers like that
                uint32 _pad;
                int32 width;
                int32 height;
                int32 refresh_rate;
            };
            SDL_DisplayMode internal;
        } * displays;
        SDL_Rect viewport;
    };
    static WindowInfo displayInfo;

    static bool Init();
    static void CopyFrameBuffer();
    static void FlipScreen();
    static void Release(bool32 isRefresh);

    static void RefreshWindow();
    static void GetWindowSize(int32 *width, int32 *height);

    static void SetupImageTexture(int32 width, int32 height, uint8 *imagePixels);
    static void SetupVideoTexture_YUV420(int32 width, int32 height, uint8 *yPlane, uint8 *uPlane, uint8 *vPlane, int32 strideY, int32 strideU,
                                         int32 strideV);
    static void SetupVideoTexture_YUV422(int32 width, int32 height, uint8 *yPlane, uint8 *uPlane, uint8 *vPlane, int32 strideY, int32 strideU,
                                         int32 strideV);
    static void SetupVideoTexture_YUV444(int32 width, int32 height, uint8 *yPlane, uint8 *uPlane, uint8 *vPlane, int32 strideY, int32 strideU,
                                         int32 strideV);

    static bool ProcessEvents();

    static void InitFPSCap();
    static bool CheckFPSCap();
    static void UpdateFPSCap();

    static bool InitShaders();
    static void LoadShader(const char *fileName, bool32 linear);

    static inline void ShowCursor(bool32 shown) { SDL_ShowCursor(shown); }
    static inline bool GetCursorPos(Vector2 *pos)
    {
        SDL_GetMouseState(&pos->x, &pos->y);
        return true;
    };

    static inline void SetWindowTitle() { SDL_SetWindowTitle(window, gameVerInfo.gameTitle); };

    static SDL_Window *window;
    static SDL_Renderer *renderer;
    static SDL_Texture *screenTexture[SCREEN_COUNT];

    static SDL_Texture *imageTexture;

#if RETRO_SDL2_SCREEN_FILTERS
    // [0] = light scanlines (CRT-Yeetron), [1] = strong scanlines (CRT-Yee64)
    static SDL_Texture *scanlineTexture[2];
#endif

private:
    static bool SetupRendering();
    static void InitVertexBuffer();
    static bool InitGraphicsAPI();

    static void GetDisplays();

#if RETRO_SDL2_SCREEN_FILTERS
    static bool CreateScanlineTextures();
    static void ApplyScreenFilter();
    static void DrawScanlines();
    static int32 GetScreenStartVerts(int32 *startVerts);
#endif

    static void ProcessEvent(SDL_Event event);

    static uint32 displayModeIndex;
    static int32 displayModeCount;

    static unsigned long long targetFreq;
    static unsigned long long curTicks;
    static unsigned long long prevTicks;

    static RenderVertex vertexBuffer[!RETRO_REV02 ? 24 : 60];

    // thingo majigo for handling video/image swapping
    static uint8 lastTextureFormat;
};
