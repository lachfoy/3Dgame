#include "Input.h"

Input::Input()
{
	// find the number of keys
	SDL_GetKeyboardState(&m_numKeys);
	m_keyboardState = new bool[m_numKeys];
	m_lastKeyboardState = new bool[m_numKeys];

	// intialize the states as false
	for (int i = 0; i < m_numKeys; i++) {
		m_keyboardState[i] = false;
		m_lastKeyboardState[i] = false;
	}
}

Input::~Input()
{
	// delete arrays
	delete[] m_keyboardState;
	delete[] m_lastKeyboardState;
}

bool Input::HandleEvents()
{
	// handle input events
	while (SDL_PollEvent(&m_event) != 0) {
		switch(m_event.type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			m_keyboardState[m_event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			m_keyboardState[m_event.key.keysym.scancode] = false;
			break;
		case SDL_MOUSEMOTION:
			m_mouseAbsPos = glm::vec2(static_cast<float>(m_event.motion.x), static_cast<float>(m_event.motion.y));
			//m_mouseAbsPos = (m_mouseAbsPos / glm::vec2(800.0f, 600.0f)); //*vVirtualSize;
			break;
		}
	}

	int relX, relY;
	SDL_GetRelativeMouseState(&relX, &relY);
	m_mouseRelPos = glm::vec2(static_cast<float>(relX), static_cast<float>(relY));
	//m_mouseRelPos = (m_mouseRelPos / glm::vec2(800.0f, 600.0f));//*vVirtualSize;

	return true;
}

void Input::Update()
{
	// update last state
	for (int i = 0; i < m_numKeys; i++) {
		m_lastKeyboardState[i] = m_keyboardState[i];
	}
}

bool Input::IsKeyPressed(Uint8 key) const
{
	return m_keyboardState[key] && !m_lastKeyboardState[key];
}

bool Input::IsKeyHeld(Uint8 key) const
{
	return m_keyboardState[key];
}

glm::vec2 Input::GetMouseAbsPos() const
{
	return m_mouseAbsPos;
}

glm::vec2 Input::GetMouseRelPos()
{
	glm::vec2 pos = m_mouseRelPos;
	m_mouseRelPos = glm::vec2(0.0f);
	return pos;
}
