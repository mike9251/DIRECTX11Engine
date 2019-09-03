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
	float dt = timer.GetMillisecondsElapsed();
	timer.Restart();

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
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0.0f);
				std::string msg = "X: ";
				msg += std::to_string(me.GetPosX());
				msg += " ";
				msg += std::to_string(me.GetPosY());
				msg += "\n";
				OutputDebugStringA(msg.c_str());
			}
		}
	}

	const float cameraSpeed = 0.01f;

	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z'))
	{
		this->gfx.camera.AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
	}
}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}
