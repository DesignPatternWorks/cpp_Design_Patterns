#pragma once

namespace Decorator {
	class Object {
	public:
		virtual void what() = 0;
	};
	
	class Gift : public Object {
	public:
		virtual ~Gift() {}
	};
	
	class Perfumes : public Gift {
	public:
		Perfumes() {}
		virtual ~Perfumes() {}
		virtual void what() {
			std::cout << "Perfumes";
		}
	};
	
	// component
	class Wrapper : public Object {
	public:
		virtual ~Wrapper() {}
	};

	// concrete component
	class PlasticBag : public Wrapper {
		Gift *gift;
	public:
		PlasticBag(Gift *aGift = nullptr) {
			gift = aGift;
		}
		virtual ~PlasticBag() {
			if (gift)
				delete gift;
			gift = nullptr;
		}
		virtual void what() {
			std::cout << "Plastic Bag with [";
			if (gift)
				gift -> what();
			std::cout << "]";
			std::cout << std::endl;
		}
	};
		
	// decorator
	class GiftWrapper : public Wrapper {
		Wrapper *wrapper;
	public:
		GiftWrapper(Wrapper *aWrapper = nullptr) {
			wrapper = aWrapper;
		}
		virtual ~GiftWrapper() {
			if (wrapper)
				delete wrapper;
			wrapper = nullptr;
		}
		virtual void what() {
			if (wrapper)
				wrapper -> what();
		}
	};

	class Box : public GiftWrapper {
	public:
		Box(Wrapper *aWrapper) : GiftWrapper(aWrapper) {}
		virtual ~Box() {}
		virtual void what() {
			GiftWrapper::what();
			std::cout << "inside Box";
			std::cout << std::endl;
		}
	};

	class Paper : public GiftWrapper {
	public:
		Paper(Wrapper *aWrapper) : GiftWrapper(aWrapper) {}
		virtual ~Paper() {}
		virtual void what() {
			GiftWrapper::what();
			std::cout << "wrapped with Paper";
			std::cout << std::endl;
		}
	};
		
	class Ribbon : public GiftWrapper {
	public:
		Ribbon(Wrapper *aWrapper) : GiftWrapper(aWrapper) {}
		virtual ~Ribbon() {}
		virtual void what() {
			GiftWrapper::what();
			std::cout << "tied with Ribbon";
			std::cout << std::endl;
		}
	};
	
	void TestSuite() {
		Wrapper *w = new Paper(new Ribbon(new Box(new PlasticBag(new Perfumes()))));
		w -> what();
		delete w;
	}
}