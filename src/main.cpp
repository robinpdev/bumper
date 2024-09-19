#define PGE_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "../extensions/imgui_impl_pge.h"

#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"

#include "bump-module.h"

#include <dlfcn.h>
#include "src/DLLoader/unix/DLLoader.h"


#define INSTANCE_DEFAULT_WIDTH 256
#define INSTANCE_DEFAULT_HEIGHT 240

using ModuleLoader = dlloader::DLLoader<bump::Module, olc::PixelGameEngine*>;

class Bumper : public olc::PixelGameEngine
{
private:
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GameLayer;

	const std::string modulepaths[1] = {"./build/modules/testmodule.so"};
	const std::string compileCommand = "bash ./makemodules.sh";
	std::map<std::string, std::shared_ptr<ModuleLoader>> loaders;
	std::map<std::string, std::shared_ptr<bump::Module>> instances;
	float totalTime = 0;

	
public:
	Bumper() : pge_imgui(false)
	{
		sAppName = "Bumper Compositor";
	}

	~Bumper(){
		instances.clear();

		for(auto const& loader : loaders){
			loader.second->DLCloseLib();
		}
	}

public:
	bool OnUserCreate() override
	{
		//Create a new Layer which will be used for the game
		m_GameLayer = CreateLayer();
		//The layer is not enabled by default,  so we need to enable it
		EnableLayer(m_GameLayer, true);

		//Set a custom render function on layer 0.  Since DrawUI is a member of
		//our class, we need to use std::bind
		//If the pge_imgui was constructed with _register_handler = true, this line is not needed
		SetLayerCustomRenderFunction(0, std::bind(&Bumper::DrawUI, this));

		olc::PixelGameEngine* engine = this;
      	printf("main engine: %p\n", engine);

		for(std::string path : modulepaths){
			std::cout << path << std::endl;
			std::shared_ptr<ModuleLoader> loader(new ModuleLoader(path));
			//loader->DLOpenLib();
			loaders[path] = loader;
		}

		reloadModules();
		
		return true;
	}

	void reloadModules(){
		instances.clear();
		for(std::string path : modulepaths){
			loaders[path]->reloadLib();
		}

		for(std::string path : modulepaths){
			instances[path] = loaders[path]->DLGetInstance(this);
		}

		for(auto const& instance : instances){
			instance.second->Create(INSTANCE_DEFAULT_WIDTH, INSTANCE_DEFAULT_HEIGHT);
		}
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		totalTime += fElapsedTime;

		if(GetKey(olc::R).bPressed){
			reloadModules();
		}else if(GetKey(olc::T).bPressed){
			printf("compiling...\n");
			int exitcode = system(compileCommand.c_str());
			if(exitcode != 0) printf("compiling failed!\n");
			reloadModules();
		}

		// called once per frame
		for(auto const& instance : instances){
			instance.second->Update(totalTime, fElapsedTime);
		}

		Clear(olc::BLACK);
		SetDrawTarget((uint8_t)m_GameLayer);

		for(auto const& instance : instances){
			instance.second->draw();
		}

		//-----------IMGUI-------------//

		ImGui::ShowDemoWindow();

		ImGui::Begin("Demo window");
		ImGui::Button("Hello!");
		ImGui::End();
		
		return true;
	}

	void DrawUI(void) {
		//This finishes the Dear ImGui and renders it to the screen
		pge_imgui.ImGui_ImplPGE_Render();
	}
};


int main()
{
	Bumper compositor;
	if (compositor.Construct(256 * 4, 240 * 4, 1, 1, false, true))
		compositor.Start();

	return 0;
}
