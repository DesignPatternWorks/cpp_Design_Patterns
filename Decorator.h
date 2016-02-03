#pragma once

namespace Decorator {
	class Object {
	public:
		virtual void what() = 0;
	};
	
	class Gift : public Object {
	public:
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
			gift -> what();
			std::cout << "]";
			std::cout << std::endl;
		}
	};
		
	// decorator
	class GiftWrapper : public Wrapper {
	protected:
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
	};

	class Box : public GiftWrapper {
	public:
		Box(Wrapper *aWrapper) : GiftWrapper(aWrapper) {}
		virtual ~Box() {}
		virtual void what() {
			wrapper -> what();
			std::cout << "inside Box";
			std::cout << std::endl;
		}
	};

	class Paper : public GiftWrapper {
	public:
		Paper(Wrapper *aWrapper) : GiftWrapper(aWrapper) {}
		virtual ~Paper() {}
		virtual void what() {
			wrapper -> what();
			std::cout << "wrapped with Paper";
			std::cout << std::endl;
		}
	};
		
	class Ribbon : public GiftWrapper {
	public:
		Ribbon(Wrapper *aWrapper) : GiftWrapper(aWrapper) {}
		virtual ~Ribbon() {}
		virtual void what() {
			wrapper -> what();
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