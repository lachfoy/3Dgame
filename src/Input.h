#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

class Input
{
public:
	Input();
	~Input();

	bool HandleEvents();
	void Update();

	bool IsKeyPressed(Uint8 key) const;
	bool IsKeyHeld(Uint8 key) const;
	glm::vec2 GetMouseAbsPos() const;
	glm::vec2 GetMouseRelPos();

private:
	SDL_Event m_event;
	int m_numKeys;
	bool *m_keyboardState;
	bool *m_lastKeyboardState;

	// mouse
	glm::vec2 m_mouseAbsPos;
	glm::vec2 m_mouseRelPos;

};
