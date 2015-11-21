#pragma once

#include "ITestScreen.h"

class MainTestScreen: public ITestScreen
{
public:
	MainTestScreen(Ptr<TestApplication> application);
	~MainTestScreen();

	void Load();
	void Unload();

	void Update(float dt);
	void Draw();
	String GetId() const;
};