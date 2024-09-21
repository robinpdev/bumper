
#include "../extensions/imgui_impl_pge.h"


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

	#ifdef __EMSCRIPTEN__
		const std::string modulepaths[1] = {"./build/modules/testmodule.wasm"};
	#else
		const std::string modulepaths[1] = {"./build/modules/testmodule.so"};
	#endif

	const std::string compileCommand = "bash ./makemodules.sh";

	std::map<std::string, std::shared_ptr<ModuleLoader>> loaders;
	std::map<std::string, std::shared_ptr<bump::Module>> instances;
	float totalTime = 0;

	olc::vi2d imguivp = {0, 0};
	olc::vi2d pgevp = {0, 0};

	ImVec2 windowpos = ImVec2(0,0);

	
public:
	Bumper() : pge_imgui(false)
	{
		sAppName = "Bumper Compositor but how fast";
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

		pgevp = GetWindowSize();

		

		//imguivp = pgevp;

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
			windowpos.x = instance.second->pos.x;
			windowpos.y = instance.second->pos.y;
		}

	}

	int map(int x, int in_min, int in_max, int out_min, int out_max) {
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}


	bool ptrue = true;
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

		SetDrawTarget((uint8_t)m_GameLayer);
		Clear(olc::BLACK);

		for(auto const& instance : instances){
			instance.second->draw();
		}

		//-----------IMGUI-------------//

		ImGui::ShowDemoWindow();

		ImGuiViewport* vp = ImGui::GetMainViewport();
		imguivp.x =  vp->WorkSize.x;
		imguivp.y =  vp->WorkSize.y;

		for(auto const& instance : instances){
			bump::Module& inst = *instance.second;
			ImGui::SetNextWindowPos(ImVec2(inst.pos.x, inst.pos.y), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(inst.size.x, inst.size.y), ImGuiCond_FirstUseEver);
			ImGui::Begin(instance.first.c_str(), &ptrue, ImGuiWindowFlags_NoBackground);

			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			ImVec2 vMax = ImGui::GetWindowContentRegionMax();
			ImVec2 windowpos = ImGui::GetWindowPos();
			ImVec2 windowsize = ImGui::GetWindowSize();
			ImGui::End();


			inst.pos.x = map(windowpos.x + vMin.x - 8, 0, imguivp.x, 0, pgevp.x);
			inst.pos.y = map(windowpos.y + vMin.y - 8, 0, imguivp.y, 0, pgevp.y);
			inst.setSize(map(windowsize.x, 0, imguivp.x, 0, pgevp.x), map(windowsize.y - vMin.y + 8, 0, pgevp.y, 0, pgevp.y));
		}
		
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
