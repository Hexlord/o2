#include "UITestScreen.h"

#include "Render/Render.h"
#include "TestApplication.h"

UITestScreen::UITestScreen(Ptr<TestApplication> application):
	ITestScreen(application)
{
}

UITestScreen::~UITestScreen()
{
}

void UITestScreen::Load()
{
	RebuildStyle();

	mBackground.LoadFromImage("ui/UI_Background.png");
	mBackground.size = (Vec2I)o2Render.resolution;

	auto window = o2UI.CreateWidget<UIWindow>();
	o2UI.AddWidget(window);
	window->layout.size = Vec2F(300, 300);

	auto verLayout = o2UI.CreateVerLayout();
	verLayout->baseCorner = BaseCorner::Top;
	verLayout->expandHeight = false;
	verLayout->fitByChildren = true;
	verLayout->border = RectF(5, 5, 5, 5);
	verLayout->spacing = 10;
	verLayout->layout = UIWidgetLayout::Both();
	window->AddChild(verLayout);

	//button
	auto buttonContainer = o2UI.CreateVerLayout();
	buttonContainer->layout.height = 50;
	buttonContainer->AddChild(o2UI.CreateLabel("Button"));
	buttonContainer->AddChild(o2UI.CreateButton("Click me!"));
	verLayout->AddChild(buttonContainer);

	//check box
	auto checkBoxContainer = o2UI.CreateVerLayout();
	checkBoxContainer->layout.height = 50;
	checkBoxContainer->AddChild(o2UI.CreateLabel("Check box"));
	checkBoxContainer->AddChild(o2UI.CreateToggle("Toggle me!"));
	verLayout->AddChild(checkBoxContainer);

	//edit box
	auto editBoxContainer = o2UI.CreateVerLayout();
	editBoxContainer->fitByChildren = true;
	editBoxContainer->AddChild(o2UI.CreateLabel("Edit box"));
	editBoxContainer->AddChild(o2UI.CreateEditBox());
	verLayout->AddChild(editBoxContainer);

	// hor progress
	auto horProgressContainer = o2UI.CreateVerLayout();
	horProgressContainer->fitByChildren = true;
	horProgressContainer->AddChild(o2UI.CreateLabel("Horizontal progress"));
	horProgressContainer->AddChild(o2UI.CreateHorProgress());
	verLayout->AddChild(horProgressContainer);

	// ver progress
	auto verProgressContainer = o2UI.CreateVerLayout();
	verProgressContainer->fitByChildren = true;
	verProgressContainer->expandHeight = false;
	verProgressContainer->AddChild(o2UI.CreateLabel("Vertical progress"));
	auto verProgress = o2UI.CreateVerProgress();
	verProgress->layout.height = 100;
	verProgressContainer->AddChild(verProgress);
	verLayout->AddChild(verProgressContainer);

	// hor scroll bar
	auto horScrollContainer = o2UI.CreateVerLayout();
	horScrollContainer->fitByChildren = true;
	horScrollContainer->AddChild(o2UI.CreateLabel("Horizontal scroll bar"));
	horScrollContainer->AddChild(o2UI.CreateHorScrollBar());
	verLayout->AddChild(horScrollContainer);

	// ver scroll bar
	auto verScrollContainer = o2UI.CreateVerLayout();
	verScrollContainer->fitByChildren = true;
	verScrollContainer->expandHeight = false;
	verScrollContainer->AddChild(o2UI.CreateLabel("Vertical scroll bar"));
	auto verScroll = o2UI.CreateVerScrollBar();
	verScroll->layout.height = 100;
	verScrollContainer->AddChild(verScroll);
	verLayout->AddChild(verScrollContainer);

	// list
	auto listContainer = o2UI.CreateVerLayout();
	listContainer->fitByChildren = true;
	listContainer->expandHeight = false;
	listContainer->AddChild(o2UI.CreateLabel("List"));

	auto list = o2UI.CreateList();
	list->layout.height = 100;
	for (int i = 0; i < 10; i++)
		list->AddItem(String::Format("List item #%i", i + 1));

	listContainer->AddChild(list);
	verLayout->AddChild(listContainer);

	// drop down
	auto dropdownContainer = o2UI.CreateVerLayout();
	dropdownContainer->fitByChildren = true;
	dropdownContainer->AddChild(o2UI.CreateLabel("Drop down"));

	auto dropdown = o2UI.CreateDropdown();
	for (int i = 0; i < 10; i++)
		dropdown->AddItem(String::Format("Drop down item #%i", i + 1));
	dropdown->SetMaxListSizeInItems(5);

	dropdownContainer->AddChild(dropdown);
	verLayout->AddChild(dropdownContainer);
}

void UITestScreen::Unload()
{
	o2UI.RemoveAllWidgets();
}

void UITestScreen::Update(float dt)
{
	if (o2Input.IsKeyPressed('D'))
		o2Debug.Log("debug");

	if (o2Input.IsKeyPressed(VK_ESCAPE))
		mApplication->GoToScreen("MainTestScreen");
}

void UITestScreen::Draw()
{
	//o2Render.Clear(Color4::White());
	mBackground.Draw();
}

void UITestScreen::RebuildStyle()
{
	o2UI.ClearStyle();

	RebuildButtonStyle();
	RebuildCloseButtonStyle();
	RebuildArrowButtonStyle();
	RebuildHorProgressBarStyle();
	RebuildVerProgressBarStyle();
	RebuildHorScrollBarStyle();
	RebuildVerScrollBarStyle();
	RebuildCheckboxStyle();
	RebuildScrollAreaStyle();
	RebuildScrollAreaStraightBarsStyle();
	RebuildLabelStyle();
	RebuildEditBoxStyle();
	RebuildCustomListStyle();
	RebuildListStyle();
	RebuildCustomDropDownStyle();
	RebuildDropDownStyle();
	RebuildWindowStyle();

	o2UI.SaveStyle("ui_style.xml");
}

void UITestScreen::RebuildButtonStyle()
{
	Ptr<UIButton> sample = mnew UIButton();
	sample->layout.minSize = Vec2F(20, 20);

	sample->AddLayer("regular", mnew Sprite("ui/UI_button_regular.png"),
					 Layout::Both(-9, -9, -10, -10));

	auto selectLayer = sample->AddLayer("select", mnew Sprite("ui/UI_button_select.png"),
										Layout::Both(-9, -9, -10, -10));

	auto pressedLayer = sample->AddLayer("pressed", mnew Sprite("ui/UI_button_pressed.png"),
										 Layout::Both(-9, -9, -10, -10));

	auto focusLayer = sample->AddLayer("focus", mnew Sprite("ui/UI_button_focus.png"),
									   Layout::Both(-9, -9, -10, -10));

	Ptr<Text> captionText = mnew Text("arial.ttf");
	captionText->text = "Button";
	captionText->horAlign = Text::HorAlign::Middle;
	captionText->verAlign = Text::VerAlign::Middle;
	sample->AddLayer("caption", captionText);

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue<float>(&selectLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.1f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue<float>(&pressedLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.05f);

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 0.5f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildCloseButtonStyle()
{
	Ptr<UIButton> sample = mnew UIButton();
	sample->layout.minSize = Vec2F(5, 5);
	auto regularLayer = sample->AddLayer("regular", mnew Sprite("ui/UI_Close_button_regular.png"),
										 Layout(Vec2F(0.5f, 0.5f), Vec2F(0.5f, 0.5f), Vec2F(-10, -10), Vec2F(10, 10)));

	auto selectLayer = sample->AddLayer("select", mnew Sprite("ui/UI_Close_button_select.png"),
										Layout(Vec2F(0.5f, 0.5f), Vec2F(0.5f, 0.5f), Vec2F(-10, -10), Vec2F(10, 10)));

	auto pressedLayer = sample->AddLayer("pressed", mnew Sprite("ui/UI_Close_button_pressed.png"),
										 Layout(Vec2F(0.5f, 0.5f), Vec2F(0.5f, 0.5f), Vec2F(-10, -10), Vec2F(10, 10)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue<float>(&selectLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.1f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue<float>(&pressedLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.05f);

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 0.5f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "close");
}

void UITestScreen::RebuildArrowButtonStyle()
{
	Ptr<UIButton> sample = mnew UIButton();
	sample->layout.minSize = Vec2F(5, 5);
	auto regularLayer = sample->AddLayer("regular", mnew Sprite("ui/UI_Options_button_regular.png"),
										 Layout(Vec2F(0.5f, 0.5f), Vec2F(0.5f, 0.5f), Vec2F(-10, -10), Vec2F(10, 10)));

	auto selectLayer = sample->AddLayer("select", mnew Sprite("ui/UI_Options_button_select.png"),
										Layout(Vec2F(0.5f, 0.5f), Vec2F(0.5f, 0.5f), Vec2F(-10, -10), Vec2F(10, 10)));

	auto pressedLayer = sample->AddLayer("pressed", mnew Sprite("ui/UI_Options_button_pressed.png"),
										 Layout(Vec2F(0.5f, 0.5f), Vec2F(0.5f, 0.5f), Vec2F(-10, -10), Vec2F(10, 10)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue<float>(&selectLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.1f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue<float>(&pressedLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.05f);

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 0.5f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "arrow");
}

void UITestScreen::RebuildHorProgressBarStyle()
{
	Ptr<UIHorizontalProgress> sample = mnew UIHorizontalProgress();
	sample->layout.minSize = Vec2F(5, 5);
	auto backLayer = sample->AddLayer("back", nullptr);
	auto spriteBackLayer = backLayer->AddChildLayer("sprite", mnew Sprite("ui/UI_Hor_scrollbar_bk.png"),
													Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
														   Vec2F(-2, -4), Vec2F(2, 5)));
	backLayer->interactableLayout = Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f), Vec2F(0, -4), Vec2F(0, 4));

	auto barLayer = sample->AddLayer("bar", nullptr);
	auto barRegularSprite = barLayer->AddChildLayer("regular", mnew Sprite("ui/UI_Hor_scrollhandle_regular.png"),
													Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
														   Vec2F(-2, -4), Vec2F(2, 5)));

	auto barSelectSprite = barLayer->AddChildLayer("select", mnew Sprite("ui/UI_Hor_scrollhandle_select.png"),
												   Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
														  Vec2F(-2, -4), Vec2F(2, 5)));

	auto barPressedSprite = barLayer->AddChildLayer("pressed", mnew Sprite("ui/UI_Hor_scrollhandle_pressed.png"),
													Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
														   Vec2F(-2, -4), Vec2F(2, 5)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue(&barSelectSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.1f);

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue(&barPressedSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.05f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 1.0f/2.0f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	sample->SetOrientation(UIHorizontalProgress::Orientation::Right);

	o2UI.AddWidgetStyle(sample, "standard");
}


void UITestScreen::RebuildVerProgressBarStyle()
{
	Ptr<UIVerticalProgress> sample = mnew UIVerticalProgress();
	sample->layout.minSize = Vec2F(5, 5);
	auto backLayer = sample->AddLayer("back", nullptr);
	auto spriteBackLayer = backLayer->AddChildLayer("sprite", mnew Sprite("ui/UI_Ver_scrollbar_bk.png"),
													Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
														   Vec2F(-4, -2), Vec2F(5, 2)));
	backLayer->interactableLayout = Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f), Vec2F(-4, 0), Vec2F(4, 0));

	auto barLayer = sample->AddLayer("bar", nullptr);
	auto barRegularSprite = barLayer->AddChildLayer("regular", mnew Sprite("ui/UI_Ver_scrollhandle_regular.png"),
													Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
														   Vec2F(-4, -2), Vec2F(5, 2)));

	auto barSelectSprite = barLayer->AddChildLayer("select", mnew Sprite("ui/UI_Ver_scrollhandle_select.png"),
												   Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
														  Vec2F(-4, -2), Vec2F(5, 2)));

	auto barPressedSprite = barLayer->AddChildLayer("pressed", mnew Sprite("ui/UI_Ver_scrollhandle_pressed.png"),
													Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
														   Vec2F(-4, -2), Vec2F(5, 2)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue(&barSelectSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.1f);

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue(&barPressedSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.05f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 1.0f/2.0f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildHorScrollBarStyle()
{
	Ptr<UIHorizontalScrollBar> sample = mnew UIHorizontalScrollBar();
	sample->layout.minSize = Vec2F(5, 5);
	sample->SetScrollSense(0.25f);
	auto backLayer = sample->AddLayer("back", nullptr);
	backLayer->interactableLayout = Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f), Vec2F(0, -4), Vec2F(0, 4));
	auto spriteBackLayer = backLayer->AddChildLayer("sprite", mnew Sprite("ui/UI_Hor_scrollbar_bk.png"),
													Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
														   Vec2F(-2, -4), Vec2F(2, 5)));

	auto handleLayer = sample->AddLayer("handle", nullptr);
	handleLayer->interactableLayout = Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f), Vec2F(0, -4), Vec2F(0, 4));
	auto barRegularSprite = handleLayer->AddChildLayer("regular", mnew Sprite("ui/UI_Hor_scrollhandle_regular.png"),
													   Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
															  Vec2F(-2, -4), Vec2F(2, 5)));

	auto barSelectSprite = handleLayer->AddChildLayer("select", mnew Sprite("ui/UI_Hor_scrollhandle_select.png"),
													  Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
															 Vec2F(-2, -4), Vec2F(2, 5)));

	auto barPressedSprite = handleLayer->AddChildLayer("pressed", mnew Sprite("ui/UI_Hor_scrollhandle_pressed.png"),
													   Layout(Vec2F(0.0f, 0.5f), Vec2F(1.0f, 0.5f),
															  Vec2F(-2, -4), Vec2F(2, 5)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue(&barSelectSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.1f);

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue(&barPressedSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.05f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 1.0f/2.0f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildVerScrollBarStyle()
{
	Ptr<UIVerticalScrollBar> sample = mnew UIVerticalScrollBar();
	sample->layout.minSize = Vec2F(5, 5);
	sample->SetScrollSense(0.25f);
	auto backLayer = sample->AddLayer("back", nullptr);
	backLayer->interactableLayout = Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f), Vec2F(-4, 0), Vec2F(4, 0));
	auto spriteBackLayer = backLayer->AddChildLayer("sprite", mnew Sprite("ui/UI_Ver_scrollbar_bk.png"),
													Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
														   Vec2F(-4, -2), Vec2F(5, 2)));

	auto handleLayer = sample->AddLayer("handle", nullptr);
	handleLayer->interactableLayout = Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f), Vec2F(-4, 0), Vec2F(4, 0));
	auto barRegularSprite = handleLayer->AddChildLayer("regular", mnew Sprite("ui/UI_Ver_scrollhandle_regular.png"),
													   Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
															  Vec2F(-4, -2), Vec2F(5, 2)));

	auto barSelectSprite = handleLayer->AddChildLayer("select", mnew Sprite("ui/UI_Ver_scrollhandle_select.png"),
													  Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
															 Vec2F(-4, -2), Vec2F(5, 2)));

	auto barPressedSprite = handleLayer->AddChildLayer("pressed", mnew Sprite("ui/UI_Ver_scrollhandle_pressed.png"),
													   Layout(Vec2F(0.5f, 0.0f), Vec2F(0.5f, 1.0f),
															  Vec2F(-4, -2), Vec2F(5, 2)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue(&barSelectSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.1f);

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue(&barPressedSprite->transparency) = AnimatedValue<float>::EaseInOut(0.0f, 1.0f, 0.05f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 1.0f/2.0f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildCheckboxStyle()
{
	Ptr<UIToggle> sample = mnew UIToggle();
	sample->layout.minSize = Vec2F(20, 20);
	auto backLayer = sample->AddLayer("back", mnew Sprite("ui/UI_Check_bk.png"),
									  Layout(Vec2F(0.0f, 0.5f), Vec2F(0.0f, 0.5f), Vec2F(0, -10), Vec2F(20, 10)));

	auto selectLayer = sample->AddLayer("backSelect", mnew Sprite("ui/UI_Check_bk_select.png"),
										Layout(Vec2F(0.0f, 0.5f), Vec2F(0.0f, 0.5f), Vec2F(0, -10), Vec2F(20, 10)));

	auto pressedLayer = sample->AddLayer("backPressed", mnew Sprite("ui/UI_Check_bk_pressed.png"),
										 Layout(Vec2F(0.0f, 0.5f), Vec2F(0.0f, 0.5f), Vec2F(0, -10), Vec2F(20, 10)));

	auto checkLayer = sample->AddLayer("check", mnew Sprite("ui/UI_Ckeck.png"),
									   Layout(Vec2F(0.0f, 0.5f), Vec2F(0.0f, 0.5f), Vec2F(1, -11), Vec2F(21, 10)));

	Ptr<Text> captionText = mnew Text("arial.ttf");
	captionText->text = "Checkbox";
	captionText->horAlign = Text::HorAlign::Left;
	captionText->verAlign = Text::VerAlign::Middle;
	sample->AddLayer("caption", captionText, Layout(Vec2F(0, 0), Vec2F(1, 1), Vec2F(20, 0), Vec2F(0, 0)));

	Animation selectStateAnim;
	selectStateAnim.SetTarget(sample);
	*selectStateAnim.AddAnimationValue<float>(&selectLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.1f);

	auto selectState = sample->AddState("select", selectStateAnim);
	selectState->offStateAnimationSpeed = 1.0f/4.0f;

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue<float>(&pressedLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.05f);

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 0.5f;

	Animation valueStateAnim;
	valueStateAnim.SetTarget(sample);
	*valueStateAnim.AddAnimationValue<float>(&checkLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.1f);
	sample->AddState("value", valueStateAnim);

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildScrollAreaStraightBarsStyle()
{
	Ptr<UIScrollArea> sample = mnew UIScrollArea();
	sample->layout.minSize = Vec2F(30, 30);
	sample->SetClippingLayout(Layout::Both(1, 2, 1, 1));
	sample->SetViewLayout(Layout::Both(5, 5, 5, 5));
	sample->SetEnableScrollsHiding(false);
	sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));

	Ptr<UIHorizontalScrollBar> horScrollBar = o2UI.CreateHorScrollBar();
	horScrollBar->layout.anchorMin = Vec2F(0, 0);
	horScrollBar->layout.anchorMax = Vec2F(1, 0);
	horScrollBar->layout.offsetMin = Vec2F(5, 0);
	horScrollBar->layout.offsetMax = Vec2F(-15, 15);
	sample->SetHorizontalScrollBar(horScrollBar);

	Ptr<UIVerticalScrollBar> verScrollBar = o2UI.CreateVerScrollBar();
	verScrollBar->layout.anchorMin = Vec2F(1, 0);
	verScrollBar->layout.anchorMax = Vec2F(1, 1);
	verScrollBar->layout.offsetMin = Vec2F(-15, 15);
	verScrollBar->layout.offsetMax = Vec2F(0, -5);
	sample->SetVerticalScrollBar(verScrollBar);

	Animation enableHorScrollAnim;
	enableHorScrollAnim.SetTarget(sample);
	*enableHorScrollAnim.AddAnimationValue<float>(&sample->GetVerticalScrollbar()->layout.offsetBottom) =
		AnimatedValue<float>::EaseInOut(5, 15, 0.2f);
	*enableHorScrollAnim.AddAnimationValue<Vec2F>("mViewAreaLayout/offsetMin") =
		AnimatedValue<Vec2F>::EaseInOut(Vec2F(5, 5), Vec2F(5, 15), 0.2f);

	auto enableHorScrollState = sample->AddState("enableHorBar", enableHorScrollAnim);

	Animation enableVerScrollAnim;
	enableVerScrollAnim.SetTarget(sample);
	*enableVerScrollAnim.AddAnimationValue<float>(&sample->GetHorizontalScrollbar()->layout.offsetRight) =
		AnimatedValue<float>::EaseInOut(-5, -15, 0.2f);
	*enableVerScrollAnim.AddAnimationValue<Vec2F>("mViewAreaLayout/offsetMax") =
		AnimatedValue<Vec2F>::EaseInOut(Vec2F(-5, -5), Vec2F(-15, -5), 0.2f);

	auto enableVerScrollState = sample->AddState("enableVerBar", enableVerScrollAnim);

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "straightBars");
}

void UITestScreen::RebuildScrollAreaStyle()
{
	Ptr<UIScrollArea> sample = mnew UIScrollArea();
	sample->layout.minSize = Vec2F(20, 20);
	sample->SetClippingLayout(Layout::Both(1, 2, 1, 1));
	sample->SetViewLayout(Layout::Both(5, 5, 5, 5));
	sample->SetEnableScrollsHiding(true);
	sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));

	Ptr<UIHorizontalScrollBar> horScrollBar = o2UI.CreateHorScrollBar();
	horScrollBar->layout.anchorMin = Vec2F(0, 0);
	horScrollBar->layout.anchorMax = Vec2F(1, 0);
	horScrollBar->layout.offsetMin = Vec2F(5, 0);
	horScrollBar->layout.offsetMax = Vec2F(-15, 15);
	sample->SetHorizontalScrollBar(horScrollBar);

	Ptr<UIVerticalScrollBar> verScrollBar = o2UI.CreateVerScrollBar();
	verScrollBar->layout.anchorMin = Vec2F(1, 0);
	verScrollBar->layout.anchorMax = Vec2F(1, 1);
	verScrollBar->layout.offsetMin = Vec2F(-15, 15);
	verScrollBar->layout.offsetMax = Vec2F(0, -5);
	sample->SetVerticalScrollBar(verScrollBar);

	Animation enableHorScrollAnim;
	enableHorScrollAnim.SetTarget(sample);
	*enableHorScrollAnim.AddAnimationValue<float>(&sample->GetVerticalScrollbar()->layout.offsetBottom) =
		AnimatedValue<float>::EaseInOut(5, 15, 0.2f);

	auto enableHorScrollState = sample->AddState("enableHorBar", enableHorScrollAnim);

	Animation enableVerScrollAnim;
	enableVerScrollAnim.SetTarget(sample);
	*enableVerScrollAnim.AddAnimationValue<float>(&sample->GetHorizontalScrollbar()->layout.offsetRight) =
		AnimatedValue<float>::EaseInOut(-5, -15, 0.2f);

	auto enableVerScrollState = sample->AddState("enableVerBar", enableVerScrollAnim);

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildLabelStyle()
{
	Ptr<UILabel> sample = mnew UILabel();
	sample->layout.minSize = Vec2F(20, 20);
	Ptr<Text> captionText = mnew Text("arial.ttf");
	captionText->text = "Checkbox";
	captionText->horAlign = Text::HorAlign::Middle;
	captionText->verAlign = Text::VerAlign::Middle;
	sample->AddLayer("text", captionText);

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildEditBoxStyle()
{
	Ptr<UIEditBox> sample = mnew UIEditBox();
	sample->SetClippingLayout(Layout::Both(1, 2, 1, 1));
	sample->SetViewLayout(Layout::Both(5, 5, 5, 5));
	sample->SetEnableScrollsHiding(false);
	sample->SetCaretBlinkingDelay(0.85f);
	sample->layout.minSize = Vec2F(30, 40);

	auto backLayer = sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));
	auto selectLayer = sample->AddLayer("select", mnew Sprite("ui/UI_Editbox_select.png"), Layout::Both(-9, -9, -9, -9));
	auto focusLayer = sample->AddLayer("select", mnew Sprite("ui/UI_Editbox_focus.png"), Layout::Both(-9, -9, -9, -9));

	Ptr<UIHorizontalScrollBar> horScrollBar = o2UI.CreateHorScrollBar();
	horScrollBar->layout.anchorMin = Vec2F(0, 0);
	horScrollBar->layout.anchorMax = Vec2F(1, 0);
	horScrollBar->layout.offsetMin = Vec2F(5, 0);
	horScrollBar->layout.offsetMax = Vec2F(-15, 15);
	sample->SetHorizontalScrollBar(horScrollBar);

	Ptr<UIVerticalScrollBar> verScrollBar = o2UI.CreateVerScrollBar();
	verScrollBar->layout.anchorMin = Vec2F(1, 0);
	verScrollBar->layout.anchorMax = Vec2F(1, 1);
	verScrollBar->layout.offsetMin = Vec2F(-15, 15);
	verScrollBar->layout.offsetMax = Vec2F(0, -5);
	sample->SetVerticalScrollBar(verScrollBar);

	Animation enableHorScrollAnim;
	enableHorScrollAnim.SetTarget(sample);
	*enableHorScrollAnim.AddAnimationValue<float>(&sample->GetVerticalScrollbar()->layout.offsetBottom) =
		AnimatedValue<float>::EaseInOut(5, 15, 0.2f);
	*enableHorScrollAnim.AddAnimationValue<Vec2F>("mViewAreaLayout/offsetMin") =
		AnimatedValue<Vec2F>::EaseInOut(Vec2F(5, 5), Vec2F(5, 15), 0.2f);

	auto enableHorScrollState = sample->AddState("enableHorBar", enableHorScrollAnim);

	Animation enableVerScrollAnim;
	enableVerScrollAnim.SetTarget(sample);
	*enableVerScrollAnim.AddAnimationValue<float>(&sample->GetHorizontalScrollbar()->layout.offsetRight) =
		AnimatedValue<float>::EaseInOut(-5, -15, 0.2f);
	*enableVerScrollAnim.AddAnimationValue<Vec2F>("mViewAreaLayout/offsetMax") =
		AnimatedValue<Vec2F>::EaseInOut(Vec2F(-5, -5), Vec2F(-15, -5), 0.2f);

	auto enableVerScrollState = sample->AddState("enableVerBar", enableVerScrollAnim);

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	Ptr<Text> textDrawable = sample->GetTextDrawable();
	textDrawable->verAlign = Text::VerAlign::Top;
	textDrawable->horAlign = Text::HorAlign::Left;
	textDrawable->font = FontRef("arial.ttf");

	Ptr<Sprite> caretDrawable = sample->GetCaretDrawable();
	*caretDrawable = Sprite();
	caretDrawable->size = Vec2F(1, textDrawable->GetFont()->GetHeight()*1.7f);
	caretDrawable->pivot = Vec2F(0, 0.16f);
	caretDrawable->color = Color4::Black();

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildCustomListStyle()
{
	Ptr<UICustomList> sample = mnew UICustomList();
	sample->layout.minSize = Vec2F(20, 20);
	sample->SetClippingLayout(Layout::Both(1, 2, 1, 1));
	sample->SetViewLayout(Layout::Both(5, 5, 5, 5));
	sample->SetEnableScrollsHiding(true);
	sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));

	Ptr<Sprite> selection = mnew Sprite("ui/UI_ListBox_selection_regular.png");
	*sample->GetSelectionDrawable() = *selection;
	sample->SetSelectionDrawableLayout(Layout::Both(-10, -16, -10, -16));

	Ptr<Sprite> hover = mnew Sprite("ui/UI_ListBox_selection_hover.png");
	*sample->GetHoverDrawable() = *hover;
	sample->SetHoverDrawableLayout(Layout::Both(-10, -16, -10, -16));
	
	Ptr<UIHorizontalScrollBar> horScrollBar = o2UI.CreateHorScrollBar();
	horScrollBar->layout.anchorMin = Vec2F(0, 0);
	horScrollBar->layout.anchorMax = Vec2F(1, 0);
	horScrollBar->layout.offsetMin = Vec2F(5, 0);
	horScrollBar->layout.offsetMax = Vec2F(-15, 15);
	sample->SetHorizontalScrollBar(horScrollBar);

	Ptr<UIVerticalScrollBar> verScrollBar = o2UI.CreateVerScrollBar();
	verScrollBar->layout.anchorMin = Vec2F(1, 0);
	verScrollBar->layout.anchorMax = Vec2F(1, 1);
	verScrollBar->layout.offsetMin = Vec2F(-15, 15);
	verScrollBar->layout.offsetMax = Vec2F(0, -5);
	sample->SetVerticalScrollBar(verScrollBar);

	Animation enableHorScrollAnim;
	enableHorScrollAnim.SetTarget(sample);
	*enableHorScrollAnim.AddAnimationValue<float>(&sample->GetVerticalScrollbar()->layout.offsetBottom) =
		AnimatedValue<float>::EaseInOut(5, 15, 0.2f);

	auto enableHorScrollState = sample->AddState("enableHorBar", enableHorScrollAnim);

	Animation enableVerScrollAnim;
	enableVerScrollAnim.SetTarget(sample);
	*enableVerScrollAnim.AddAnimationValue<float>(&sample->GetHorizontalScrollbar()->layout.offsetRight) =
		AnimatedValue<float>::EaseInOut(-5, -15, 0.2f);

	auto enableVerScrollState = sample->AddState("enableVerBar", enableVerScrollAnim);

	Animation hoverStateAnim;
	hoverStateAnim.SetTarget(sample);
	*hoverStateAnim.AddAnimationValue<float>(&sample->GetHoverDrawable()->transparency) =
		AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto hoverState = sample->AddState("hover", hoverStateAnim);
	hoverState->offStateAnimationSpeed = 0.5;

	Animation selectedStateAnim;
	selectedStateAnim.SetTarget(sample);
	*selectedStateAnim.AddAnimationValue<float>(&sample->GetSelectionDrawable()->transparency) =
		AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto selectedState = sample->AddState("selected", selectedStateAnim);
	selectedState->offStateAnimationSpeed = 0.5;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildListStyle()
{
	Ptr<UIList> sample = mnew UIList();
	sample->layout.minSize = Vec2F(20, 20);
	sample->SetClippingLayout(Layout::Both(1, 2, 1, 1));
	sample->SetViewLayout(Layout::Both(5, 5, 5, 5));
	sample->SetEnableScrollsHiding(true);
	sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));

	Ptr<Sprite> selection = mnew Sprite("ui/UI_ListBox_selection_regular.png");
	*sample->GetSelectionDrawable() = *selection;
	sample->SetSelectionDrawableLayout(Layout::Both(-10, -16, -10, -16));

	Ptr<Sprite> hover = mnew Sprite("ui/UI_ListBox_selection_hover.png");
	*sample->GetHoverDrawable() = *hover;
	sample->SetHoverDrawableLayout(Layout::Both(-10, -16, -10, -16));

	Ptr<UILabel> itemSample = o2UI.CreateLabel("empty");
	itemSample->layout.size = Vec2F(20, 0);
	itemSample->horAlign = Text::HorAlign::Left;
	itemSample->verAlign = Text::VerAlign::Middle;
	sample->SetItemSample(itemSample);

	Ptr<UIHorizontalScrollBar> horScrollBar = o2UI.CreateHorScrollBar();
	horScrollBar->layout.anchorMin = Vec2F(0, 0);
	horScrollBar->layout.anchorMax = Vec2F(1, 0);
	horScrollBar->layout.offsetMin = Vec2F(5, 0);
	horScrollBar->layout.offsetMax = Vec2F(-15, 15);
	sample->SetHorizontalScrollBar(horScrollBar);

	Ptr<UIVerticalScrollBar> verScrollBar = o2UI.CreateVerScrollBar();
	verScrollBar->layout.anchorMin = Vec2F(1, 0);
	verScrollBar->layout.anchorMax = Vec2F(1, 1);
	verScrollBar->layout.offsetMin = Vec2F(-15, 15);
	verScrollBar->layout.offsetMax = Vec2F(0, -5);
	sample->SetVerticalScrollBar(verScrollBar);

	Animation enableHorScrollAnim;
	enableHorScrollAnim.SetTarget(sample);
	*enableHorScrollAnim.AddAnimationValue<float>(&sample->GetVerticalScrollbar()->layout.offsetBottom) =
		AnimatedValue<float>::EaseInOut(5, 15, 0.2f);

	auto enableHorScrollState = sample->AddState("enableHorBar", enableHorScrollAnim);

	Animation enableVerScrollAnim;
	enableVerScrollAnim.SetTarget(sample);
	*enableVerScrollAnim.AddAnimationValue<float>(&sample->GetHorizontalScrollbar()->layout.offsetRight) =
		AnimatedValue<float>::EaseInOut(-5, -15, 0.2f);

	auto enableVerScrollState = sample->AddState("enableVerBar", enableVerScrollAnim);

	Animation hoverStateAnim;
	hoverStateAnim.SetTarget(sample);
	*hoverStateAnim.AddAnimationValue<float>(&sample->GetHoverDrawable()->transparency) =
		AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto hoverState = sample->AddState("hover", hoverStateAnim);
	hoverState->offStateAnimationSpeed = 0.5;

	Animation selectedStateAnim;
	selectedStateAnim.SetTarget(sample);
	*selectedStateAnim.AddAnimationValue<float>(&sample->GetSelectionDrawable()->transparency) =
		AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto selectedState = sample->AddState("selected", selectedStateAnim);
	selectedState->offStateAnimationSpeed = 0.5;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildCustomDropDownStyle()
{
	Ptr<UICustomDropDown> sample = mnew UICustomDropDown();
	sample->layout.minSize = Vec2F(20, 20);
	auto backLayer = sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));
	auto selectLayer = sample->AddLayer("select", mnew Sprite("ui/UI_Editbox_select.png"), Layout::Both(-9, -9, -9, -9));
	auto pressedLayer = sample->AddLayer("pressed", mnew Sprite("ui/UI_Editbox_pressed.png"), Layout::Both(-9, -9, -9, -9));
	auto arrowLayer = sample->AddLayer("arrow", mnew Sprite("ui/UI_Down_icn.png"),
									   Layout(Vec2F(1.0f, 0.5f), Vec2F(1.0f, 0.5f), Vec2F(-20, -10), Vec2F(0, 10)));

	sample->SetClippingLayout(Layout::Both(4, 2, 20, 2));

	Ptr<UIWidget> itemSample = mnew UIWidget();
	itemSample->layout.size = Vec2F(20, 20);
	sample->SetItemSample(itemSample);

	auto list = sample->GetListView();
	*list = *o2UI.GetWidgetStyle<UICustomList>("standard");
	list->layer["back"]->drawable.Release();
	list->layer["back"]->drawable = mnew Sprite("ui/UI_Box_regular.png");
	list->layout.pivot = Vec2F(0.5f, 1.0f);
	list->layout.anchorMin = Vec2F(0, 0);
	list->layout.anchorMax = Vec2F(1, 0);
	list->layout.offsetMin = Vec2F(-1, -60);
	list->layout.offsetMax = Vec2F(0, 3);

	Animation selectedStateAnim;
	selectedStateAnim.SetTarget(sample);
	*selectedStateAnim.AddAnimationValue<float>(&selectLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto selectedState = sample->AddState("select", selectedStateAnim);
	selectedState->offStateAnimationSpeed = 0.5;

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue<float>(&pressedLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.05f);

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 0.5f;

	Animation openAnimStateAnim;
	openAnimStateAnim.SetTarget(sample);
	*openAnimStateAnim.AddAnimationValue<Vec2F>(&arrowLayer->drawable->scale) =
		AnimatedValue<Vec2F>::EaseInOut(Vec2F(1, 1), Vec2F(1, -1), 0.2f);

	auto openedState = sample->AddState("opened", openAnimStateAnim);
	//openedState->offStateAnimationSpeed = 2.0f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildDropDownStyle()
{
	Ptr<UIDropDown> sample = mnew UIDropDown();
	sample->layout.minSize = Vec2F(20, 20);
	auto backLayer = sample->AddLayer("back", mnew Sprite("ui/UI_Editbox_regular.png"), Layout::Both(-9, -9, -9, -9));
	auto selectLayer = sample->AddLayer("select", mnew Sprite("ui/UI_Editbox_select.png"), Layout::Both(-9, -9, -9, -9));
	auto pressedLayer = sample->AddLayer("pressed", mnew Sprite("ui/UI_Editbox_pressed.png"), Layout::Both(-9, -9, -9, -9));
	auto arrowLayer = sample->AddLayer("arrow", mnew Sprite("ui/UI_Down_icn.png"),
									   Layout(Vec2F(1.0f, 0.5f), Vec2F(1.0f, 0.5f), Vec2F(-20, -10), Vec2F(0, 10)));

	sample->SetClippingLayout(Layout::Both(4, 2, 20, 2));

	auto list = sample->GetListView();
	*list = *o2UI.GetWidgetStyle<UICustomList>("standard");
	list->layer["back"]->drawable.Release();
	list->layer["back"]->drawable = mnew Sprite("ui/UI_Box_regular.png");
	list->layout.pivot = Vec2F(0.5f, 1.0f);
	list->layout.anchorMin = Vec2F(0, 0);
	list->layout.anchorMax = Vec2F(1, 0);
	list->layout.offsetMin = Vec2F(-1, -60);
	list->layout.offsetMax = Vec2F(0, 3);

	Ptr<UILabel> itemSample = o2UI.CreateLabel("empty");
	itemSample->horAlign = Text::HorAlign::Left;
	sample->SetItemSample(itemSample);

	Animation selectedStateAnim;
	selectedStateAnim.SetTarget(sample);
	*selectedStateAnim.AddAnimationValue<float>(&selectLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto selectedState = sample->AddState("select", selectedStateAnim);
	selectedState->offStateAnimationSpeed = 0.5;

	Animation pressedStateAnim;
	pressedStateAnim.SetTarget(sample);
	*pressedStateAnim.AddAnimationValue<float>(&pressedLayer->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.05f);

	auto pressedState = sample->AddState("pressed", pressedStateAnim);
	pressedState->offStateAnimationSpeed = 0.5f;

	Animation openAnimStateAnim;
	openAnimStateAnim.SetTarget(sample);
	*openAnimStateAnim.AddAnimationValue<Vec2F>(&arrowLayer->drawable->scale) =
		AnimatedValue<Vec2F>::EaseInOut(Vec2F(1, 1), Vec2F(1, -1), 0.2f);

	auto openedState = sample->AddState("opened", openAnimStateAnim);
	//openedState->offStateAnimationSpeed = 2.0f;

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);
	visibleState->offStateAnimationSpeed = 0.5;

	o2UI.AddWidgetStyle(sample, "standard");
}

void UITestScreen::RebuildWindowStyle()
{
	Ptr<UIWindow> sample = mnew UIWindow();

	sample->layout.minSize = Vec2F(100, 50);

	auto regularBackLayer = sample->AddLayer("regularBack", mnew Sprite("ui/UI_window_frame_regular.png"),
											 Layout::Both(-12, -12, -14, -14));

	auto iconLayer = sample->AddLayer("icon", mnew Sprite("ui/UI_o2_sign.png"),
									  Layout(Vec2F(0.0f, 1.0f), Vec2F(0.0f, 1.0f), Vec2F(3, -14), Vec2F(17, 1)));

	Ptr<Text> captionText = mnew Text("arial.ttf");
	captionText->text = "Window";
	captionText->horAlign = Text::HorAlign::Left;
	captionText->verAlign = Text::VerAlign::Middle;
	auto textLayer = sample->AddLayer("caption", captionText,
									  Layout(Vec2F(0.0f, 1.0f), Vec2F(1.0f, 1.0f), Vec2F(19, -13), Vec2F(-28, 3)));

	sample->SetClippingLayout(Layout::Both(0, 1, -1, 15));
	sample->SetViewLayout(Layout::Both(5, 5, 5, 20));
	sample->SetEnableScrollsHiding(true);

	Ptr<UIHorizontalScrollBar> horScrollBar = o2UI.CreateHorScrollBar();
	horScrollBar->layout.anchorMin = Vec2F(0, 0);
	horScrollBar->layout.anchorMax = Vec2F(1, 0);
	horScrollBar->layout.offsetMin = Vec2F(5, 0);
	horScrollBar->layout.offsetMax = Vec2F(-15, 15);
	sample->SetHorizontalScrollBar(horScrollBar);

	Ptr<UIVerticalScrollBar> verScrollBar = o2UI.CreateVerScrollBar();
	verScrollBar->layout.anchorMin = Vec2F(1, 0);
	verScrollBar->layout.anchorMax = Vec2F(1, 1);
	verScrollBar->layout.offsetMin = Vec2F(-15, 15);
	verScrollBar->layout.offsetMax = Vec2F(0, -20);
	sample->SetVerticalScrollBar(verScrollBar);

	Animation enableHorScrollAnim;
	enableHorScrollAnim.SetTarget(sample);
	*enableHorScrollAnim.AddAnimationValue<float>(&sample->GetVerticalScrollbar()->layout.offsetBottom) =
		AnimatedValue<float>::EaseInOut(5, 15, 0.2f);

	auto enableHorScrollState = sample->AddState("enableHorBar", enableHorScrollAnim);

	Animation enableVerScrollAnim;
	enableVerScrollAnim.SetTarget(sample);
	*enableVerScrollAnim.AddAnimationValue<float>(&sample->GetHorizontalScrollbar()->layout.offsetRight) =
		AnimatedValue<float>::EaseInOut(-5, -15, 0.2f);

	auto enableVerScrollState = sample->AddState("enableVerBar", enableVerScrollAnim);

	Animation visibleStateAnim;
	visibleStateAnim.SetTarget(sample);
	*visibleStateAnim.AddAnimationValue<float>(&sample->transparency) = AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

	auto visibleState = sample->AddState("visible", visibleStateAnim);

	Ptr<UIButton> closeBtn = o2UI.CreateWidget<UIButton>("close");
	closeBtn->name = "closeButton";
	closeBtn->layout.anchorMin = Vec2F(1, 1);
	closeBtn->layout.anchorMax = Vec2F(1, 1);
	closeBtn->layout.offsetMin = Vec2F(-19, -15);
	closeBtn->layout.offsetMax = Vec2F(1, 3);
	sample->AddWindowElement(closeBtn);

	Ptr<UIButton> optionsBtn = o2UI.CreateWidget<UIButton>("arrow");
	optionsBtn->name = "optionsButton";
	optionsBtn->layout.anchorMin = Vec2F(1, 1);
	optionsBtn->layout.anchorMax = Vec2F(1, 1);
	optionsBtn->layout.offsetMin = Vec2F(-34, -15);
	optionsBtn->layout.offsetMax = Vec2F(-16, 3);
	sample->AddWindowElement(optionsBtn);

	sample->SetDragAreaLayouts(Layout(Vec2F(0, 1), Vec2F(1, 1), Vec2F(5, -15), Vec2F(-5, -2)),    // head
							   Layout(Vec2F(0, 1), Vec2F(1, 1), Vec2F(5, -2), Vec2F(-5, 5)),      // top
							   Layout(Vec2F(0, 0), Vec2F(1, 0), Vec2F(5, -5), Vec2F(-5, 5)),      // bottom
							   Layout(Vec2F(0, 0), Vec2F(0, 1), Vec2F(-5, 5), Vec2F(5, -5)),      // left
							   Layout(Vec2F(1, 0), Vec2F(1, 1), Vec2F(-5, 5), Vec2F(5, -5)),      // right
							   Layout(Vec2F(0, 1), Vec2F(0, 1), Vec2F(-5, -5), Vec2F(5, 5)),      // left top
							   Layout(Vec2F(1, 1), Vec2F(1, 1), Vec2F(-5, -5), Vec2F(5, 5)),      // right top
							   Layout(Vec2F(0, 0), Vec2F(0, 0), Vec2F(-5, -5), Vec2F(5, 5)),      // left bottom 
							   Layout(Vec2F(1, 0), Vec2F(1, 0), Vec2F(-5, -5), Vec2F(5, 5)));     // right bottom

	o2UI.AddWidgetStyle(sample, "standard");
}

String UITestScreen::GetId() const
{
	return "UITestScreen";
}