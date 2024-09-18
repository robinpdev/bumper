#include "../../olcPixelGameEngine.h"
#include "../bump-module.h"

class testModule : public bump::Module{
protected:
  bool OnUserCreate() override{
    pos = {0, 0};
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override{
    pge->FillRect(0, 0, 100, 150, olc::GREEN);
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