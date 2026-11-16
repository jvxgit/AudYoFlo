#include "jvxGlFunctions.h"
#include "IjvxGlContext.h"

#include <string>
#include <atomic>

/*
 * Offscreen OpenGL context on Windows: a hidden window provides the device context, rendering
 * happens into a framebuffer object. The image is read back and handed over to the UI (e.g. the
 * Flutter pixel buffer texture), hence, no context sharing with the UI is required.
 *
 * wglCreateContext returns a compatibility profile context which supports the GLSL 1.10 style
 * shaders of jvxVideoRenderCore (attribute/varying/texture2D/gl_FragColor).
 */
class CjvxGlContextWgl : public IjvxGlContext
{
	HWND hwnd = NULL;
	HDC hdc = NULL;
	HGLRC hglrc = NULL;
	HMODULE hOpenGl = NULL;

	static const wchar_t* className() { return L"JvxGlPlotHiddenWindow"; };

	static bool register_class()
	{
		static std::atomic<bool> registered(false);
		if (!registered)
		{
			WNDCLASSEXW wc = { 0 };
			wc.cbSize = sizeof(wc);
			wc.style = CS_OWNDC;
			wc.lpfnWndProc = DefWindowProcW;
			wc.hInstance = GetModuleHandleW(NULL);
			wc.lpszClassName = className();
			if ((RegisterClassExW(&wc) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
			{
				return false;
			}
			registered = true;
		}
		return true;
	};

public:

	bool create(std::string& errDescr) override
	{
		if (!register_class())
		{
			errDescr = "Failed to register window class for OpenGL context.";
			return false;
		}

		hwnd = CreateWindowExW(0, className(), L"jvxGlPlot", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, 16, 16, NULL, NULL, GetModuleHandleW(NULL), NULL);
		if (hwnd == NULL)
		{
			errDescr = "Failed to create hidden window for OpenGL context.";
			return false;
		}

		hdc = GetDC(hwnd);

		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 8;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int pf = ChoosePixelFormat(hdc, &pfd);
		if ((pf == 0) || (!SetPixelFormat(hdc, pf, &pfd)))
		{
			errDescr = "Failed to set pixel format for OpenGL context.";
			destroy();
			return false;
		}

		hglrc = wglCreateContext(hdc);
		if (hglrc == NULL)
		{
			errDescr = "Failed to create OpenGL context.";
			destroy();
			return false;
		}

		if (!wglMakeCurrent(hdc, hglrc))
		{
			errDescr = "Failed to activate OpenGL context.";
			destroy();
			return false;
		}

		hOpenGl = LoadLibraryW(L"opengl32.dll");
		return true;
	};

	void destroy() override
	{
		if (hglrc)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			hglrc = NULL;
		}
		if (hdc)
		{
			ReleaseDC(hwnd, hdc);
			hdc = NULL;
		}
		if (hwnd)
		{
			DestroyWindow(hwnd);
			hwnd = NULL;
		}
		if (hOpenGl)
		{
			FreeLibrary(hOpenGl);
			hOpenGl = NULL;
		}
		process_events();
	};

	void* get_proc_address(const char* name) override
	{
		// wglGetProcAddress only returns extension / > 1.1 functions, some drivers return small
		// integer values instead of NULL on failure
		void* p = (void*)wglGetProcAddress(name);
		intptr_t pi = (intptr_t)p;
		if ((pi == 0) || (pi == 1) || (pi == 2) || (pi == 3) || (pi == -1))
		{
			p = nullptr;
			if (hOpenGl)
			{
				p = (void*)GetProcAddress(hOpenGl, name);
			}
		}
		return p;
	};

	void process_events() override
	{
		// The hidden window is owned by the render thread: keep its message queue empty so that
		// broadcast messages to all top level windows do not block.
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	};
};

IjvxGlContext*
jvx_gl_context_allocate()
{
	return new CjvxGlContextWgl;
}

void
jvx_gl_context_deallocate(IjvxGlContext* ctx)
{
	delete ctx;
}
