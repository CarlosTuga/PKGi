#include "App.h"

#include "Controller.h"
#include "Graphics.h"
#include "Resource.h"
#include "Network.h"
#include "Mira.h"
#include "View.h"
#include "SourcesView.h"
#include "PackageListView.h"

Application::Application() {
	isRunning = false;

	// Initialize system componant

	printf("Initialize Controller ...\n");
	Ctrl  = new Controller();
	printf("Initialize Graphics ...\n");
	Graph = new Graphics();
	printf("Initialize Resource ...\n");
	Res   = new Resource(this);
	printf("Initialize Network ...\n");
	Net = new Network();
	printf("Initialize Mira");
	Kernel = new Mira();

	// Setup the default view
	printf("Initialize the Main View ...\n");

	SourcesView* main_view = new SourcesView(this);
	currentView = main_view;
;}

Application::~Application() {
	printf("Cleanup Application ...\n");
	delete Res;
	delete Graph;
	delete Ctrl;
	delete Net;
	delete Kernel;
}

void Application::Update() {
	Ctrl->Update();

	if (currentView)
		currentView->Update();
	else
		printf("[ERROR][%s][%d]: No view setup !\n", __FILE__, __LINE__);
}

void Application::ShowFatalReason(const char* reason) {
	Graph->WaitFlip();

	int ScreenHeight = Graph->GetScreenHeight();
	int ScreenWidth = Graph->GetScreenWidth();

	Color red = { 0xFF, 0x00, 0x00, 0xFF };
	Color white = { 0xFF, 0xFF, 0xFF, 0xFF };

	// Draw red background
	Graph->drawRectangle(0, 0, ScreenWidth, ScreenHeight, red);

	// Draw message
	char message[255];
	snprintf(message, 255, "PKGi is unloadable !\nReason: %s\n\nPlease close the Application.", reason);
	FontSize messageSize;
	Graph->setFontSize(Res->robotoFont, 54);
	Graph->getTextSize(message, Res->robotoFont, &messageSize);
	Graph->drawText(message, Res->robotoFont, ((ScreenWidth / 2) - (messageSize.width / 2)), ((ScreenHeight / 2) - (messageSize.height / 2)), red, white);
	Graph->setFontSize(Res->robotoFont, DEFAULT_FONT_SIZE);

	Graph->SwapBuffer(flipArgs);
	flipArgs++;
}

void Application::Render() {
	Graph->WaitFlip();
	
	if (currentView)
		currentView->Render();
	else
		printf("[ERROR][%s][%d]: No view setup !\n", __FILE__, __LINE__);

	Graph->SwapBuffer(flipArgs);
	flipArgs++;
}

void Application::ChangeView(View* view) {
	printf("Change view to %p !\n", view);

	if (!view)
		return;

	currentView = view;
}

void Application::Run() {
	printf("Application: RUN\n");

	flipArgs = 0;
	isRunning = true;

	// Check if Mira is available
	if (!Kernel->isAvailable()) {
		ShowFatalReason("Mira is needed.");
		return;
	}

	// Mount /data into sandbox
	Kernel->MountInSandbox("/data", 511);

	// Change Auth ID by SceShellCore
	Kernel->ChangeAuthID(SceAuthenticationId::SceShellCore);

	// Main Loop
	while (isRunning)
	{
		Update();
		Render();
	}
}