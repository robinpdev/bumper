#pragma once

#include <iostream>
#include <dlfcn.h>
#include "src/DLLoader/IDLLoader.h"

namespace dlloader
{
	template <class T, typename S>
	class DLLoader : public IDLLoader<T, S>
	{

	private:

		void			*_handle;
		std::string		_pathToLib;
		std::string		_allocClassSymbol;
		std::string		_deleteClassSymbol;

	public:

		DLLoader(std::string const &pathToLib,
			std::string const &allocClassSymbol = "allocator",
			std::string const &deleteClassSymbol = "deleter") :
			_handle(nullptr), _pathToLib(pathToLib),
			_allocClassSymbol(allocClassSymbol), _deleteClassSymbol(deleteClassSymbol)
		{
		}

		~DLLoader(){
			DLCloseLib();
		};

		void DLOpenLib() override
		{
			if (!(_handle = dlopen(_pathToLib.c_str(), RTLD_NOW | RTLD_LAZY))) {
				std::cerr << dlerror() << std::endl;
			}
		}

		void reloadLib() override{
			DLCloseLib();
			DLOpenLib();
		}

		std::shared_ptr<T> DLGetInstance(S input) override
		{
			using allocClass = T *(*)(S);
			using deleteClass = void (*)(T *);


			auto allocFunc = reinterpret_cast<allocClass>(
					dlsym(_handle, _allocClassSymbol.c_str()));
			auto deleteFunc = reinterpret_cast<deleteClass>(
					dlsym(_handle, _deleteClassSymbol.c_str()));

			if (!allocFunc || !deleteFunc) {
				std::cerr << dlerror() << std::endl;
				DLCloseLib();
			}

			return std::shared_ptr<T>(
					allocFunc(input),
					[deleteFunc, this](T *p){ deleteFunc(p); printf("deleted %s at %p\n", _pathToLib.c_str(), p); });
		}

		void DLCloseLib() override
		{
			if (_handle && dlclose(_handle) != 0) {
				std::cerr << dlerror() << std::endl;
			}
			_handle = 0;
		}

	};
}