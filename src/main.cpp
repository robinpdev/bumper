#define PGE_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "../extensions/imgui_impl_pge.h"

#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"

#include "bump-module.h"
#include "testmodule.h"

#include <dlfcn.h>


#define INSTANCE_DEFAULT_WIDTH 256
#define INSTANCE_DEFAULT_HEIGHT 240

void * library_handle = 0;
bump::Module* (* makeinst)(olc::PixelGameEngine*);
std::shared_ptr<bump::Module> moduleinst;

class Bumper : public olc::PixelGameEngine
{
private:
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GameLayer;

	std::vector<testModule> instances;

	
public:
	Bumper() : pge_imgui(false)
	{
		sAppName = "Bumper Compositor";
		
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

		// Called once at the start, so create things here
		/*testModule testinst = testModule(this);
		instances.push_back(testinst);
		for(testModule& instance : instances){
			instance.Create(INSTANCE_DEFAULT_WIDTH, INSTANCE_DEFAULT_HEIGHT);
		}*/

		std::string path = "./build/modules/testmodule.so";

		if(library_handle){
			dlclose(library_handle);
			if(dlerror() != NULL){
				fprintf(stderr, "Error unloading library: %s\n", dlerror());
				exit(1);
			}
			library_handle = 0;
			makeinst = 0;
			printf("Library unloaded\n");
		}

		library_handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LAZY);
		if (!library_handle){
			fprintf(stderr, "Error loading library: %s\n", dlerror());
			exit(1);
		}
		printf("Library loaded\n");
		using allocClass = bump::Module *(*)(olc::PixelGameEngine*);
		using deleteClass = void (*)(bump::Module *);

		auto allocator = reinterpret_cast<allocClass>(dlsym(library_handle, "allocator"));
		auto deleter = reinterpret_cast<deleteClass>(dlsym(library_handle, "deleter"));
		if (!allocator || !deleter){
			fprintf(stderr, "Error loading symbol: %s\n", dlerror());
			exit(1);
		}
		printf("%p\n", (void*)makeinst);

		olc::PixelGameEngine* engine = this;
      	printf("main engine: %p\n", engine);

		moduleinst = std::shared_ptr<bump::Module>(allocator(engine), [deleter](bump::Module* p){deleter(p);});
		printf("made module instance\n");
		moduleinst->Create(INSTANCE_DEFAULT_WIDTH, INSTANCE_DEFAULT_HEIGHT);
		printf("created buffer\n");


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		/*for(testModule& instance : instances){
			instance.Update(fElapsedTime);
		}*/
		moduleinst->Update(fElapsedTime);

		Clear(olc::BLACK);
		SetDrawTarget((uint8_t)m_GameLayer);

		/*for(testModule& instance : instances){
			instance.draw();
		}*/
		moduleinst->draw();

		ImGui::ShowDemoWindow();
		
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
