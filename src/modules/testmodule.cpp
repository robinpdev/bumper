#include "../../olcPixelGameEngine.h"
#include "../bump-module.h"

class testModule : public bump::Module{
protected:
  bool OnUserCreate() override{
    pos = {0, 0};
    return true;
  }

  bool OnUserUpdate(float totalTime, float fElapsedTime) override{
    int intime = (int) (totalTime * 100);
    pge->FillRect(0, 0, 100, 150, olc::Pixel((intime + 150) % 180, (intime + 120) % 200, intime % 255));
    pge->FillRect(0, 100, 100, 150, olc::RED);

    return true;
  }
public:
    testModule(olc::PixelGameEngine* engine) : bump::Module(engine){}
};

extern "C"{
    testModule* allocator(olc::PixelGameEngine* engine){
        //return 0;
        return new testModule(engine);
    }

    void deleter(testModule* ptr){
      delete ptr;
    }
}