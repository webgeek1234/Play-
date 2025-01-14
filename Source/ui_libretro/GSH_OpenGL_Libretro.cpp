#include "GSH_OpenGL_Libretro.h"
#include "Log.h"

#define LOG_NAME "LIBRETRO"

extern int g_res_factor;
extern CGSHandler::PRESENTATION_MODE g_presentation_mode;
extern retro_video_refresh_t g_video_cb;
extern struct retro_hw_render_callback g_hw_render;

CGSH_OpenGL_Libretro::CGSH_OpenGL_Libretro()
{
	m_mailBox.SetCanWait(false);
	m_mailBox.SendCall([this] { m_threadDone = true; }, true, true);
	m_thread.join();
}

CGSH_OpenGL_Libretro::~CGSH_OpenGL_Libretro()
{
}

CGSH_OpenGL::FactoryFunction CGSH_OpenGL_Libretro::GetFactoryFunction()
{
	return []() { return new CGSH_OpenGL_Libretro(); };
}

void CGSH_OpenGL_Libretro::InitializeImpl()
{
	fprintf(stderr, "%s\n", __FUNCTION__);

#if defined(USE_GLEW)
	glewExperimental = GL_TRUE;
	auto result = glewInit();
	CLog::GetInstance().Warn(LOG_NAME, "glewInit %d\n", result == GLEW_OK);
	assert(result == GLEW_OK);
	if(result != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(result));
		CLog::GetInstance().Warn(LOG_NAME, "Error: %s\n", glewGetErrorString(result));
		return;
	}

#endif

	if(g_hw_render.get_current_framebuffer)
		m_presentFramebuffer = g_hw_render.get_current_framebuffer();

	UpdatePresentationImpl();

	CGSH_OpenGL::InitializeImpl();
}

void CGSH_OpenGL_Libretro::UpdatePresentation()
{
	m_mailBox.SendCall([this]() { UpdatePresentationImpl(); });
}

void CGSH_OpenGL_Libretro::UpdatePresentationImpl()
{
	PRESENTATION_PARAMS presentationParams;
	presentationParams.mode = g_presentation_mode;
	presentationParams.windowWidth = GetCrtWidth() * g_res_factor;
	presentationParams.windowHeight = GetCrtHeight() * g_res_factor;

	SetPresentationParams(presentationParams);
	NotifyPreferencesChanged();
}

void CGSH_OpenGL_Libretro::Reset()
{
	m_mailBox.Reset();
	ResetBase();
	CGSH_OpenGL::ReleaseImpl();
	InitializeImpl();
}

void CGSH_OpenGL_Libretro::Release()
{
	m_mailBox.Release();
	ResetBase();
	CGSH_OpenGL::ReleaseImpl();
}

void CGSH_OpenGL_Libretro::FlipImpl()
{
	CLog::GetInstance().Print(LOG_NAME, "%s\n", __FUNCTION__);

	if(g_hw_render.get_current_framebuffer)
		m_presentFramebuffer = g_hw_render.get_current_framebuffer();
	else
		return;

	CGSH_OpenGL::FlipImpl();
}

void CGSH_OpenGL_Libretro::PresentBackbuffer()
{
	CLog::GetInstance().Print(LOG_NAME, "%s\n", __FUNCTION__);

	if(g_video_cb)
		g_video_cb(RETRO_HW_FRAME_BUFFER_VALID, GetCrtWidth() * g_res_factor, GetCrtHeight() * g_res_factor, 0);
}
