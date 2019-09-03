#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
{
	//this->keyboard.EnableAutoRepeatChars();
	if (!this->renderWindow.Initialize(this, hInstance, windowTitle, windowClass, width, height))
		return false;

	if (!gfx.Initialize(renderWindow.GetHandle(), width, height))
		return false;

	return true;
}

bool Engine::ProcessMessages()
{
	return this->renderWindow.ProcessMessages();
}

void Engine::Update()
{
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
		/*std::string outmsg1 = "Char: ";
		outmsg1 += ch;
		outmsg1 += "\n";
		OutputDebugStringA(outmsg1.c_str());*/
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
		/*std::string outmsg = "";
		if (kbe.isPress())
		{
			outmsg += "Key press: ";
		}
		if (kbe.isRelease())
		{
			outmsg += "Key release: ";
		}
		outmsg += keycode;
		outmsg += "\n";
		OutputDebugStringA(outmsg.c_str());*/
	}
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();

		if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			std::string msg = "X: ";
			msg += std::to_string(me.GetPosX());
			msg += " ";
			msg += std::to_string(me.GetPosY());
			msg += "\n";
			OutputDebugStringA(msg.c_str());
		}
	}
}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}
