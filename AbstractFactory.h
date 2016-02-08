#pragma once

namespace AbstractFactory {
	// platform dependent products
	class Component {
		std::string stamp;
	public:
		virtual void setStamp(const std::string &aStamp) {
			if (stamp.empty())
				stamp = aStamp;
		}
		friend std::ostream &operator<<(std::ostream &os, const Component &cmp);
	};
	std::ostream &operator<<(std::ostream &os, const Component &cmp) {
		os << cmp.stamp;
		return os;
	}
	class Frame : public Component {
	public:
		virtual void setStamp(const std::string &aStamp) {
			std::string stmp = aStamp + " Frame";
			Component::setStamp(stmp);
		}
	};
	class HondaFrame : public Frame {
	public:
	};
	class BMWFrame : public Frame {
	public:
	};
	
	class Weel : public Component {
	public:
		virtual void setStamp(const std::string &aStamp) {
			std::string stmp = aStamp + " Weel";
			Component::setStamp(stmp);
		}
	};
	class HondaWeel : public Weel {
	public:
	};
	class BMWWeel : public Weel {
	public:
	};
	
	// platform dependent part of abstract factory
	class Stamper {
	protected:
		std::string tmp;
	public:
		Stamper(std::string aTmp) {
			tmp = aTmp;
		}
		void setStamp(Component *component) {
			component -> setStamp(tmp);
		}
	};
	class HondaStamper : public Stamper {
	public:
		HondaStamper() : Stamper("Honda") {}
	};
	class BMWStamper : public Stamper {
	public:
		BMWStamper() : Stamper("BMW") {}
	};
	
	// abstract factory
	class AutoFactory {
	protected:
		Stamper *stamper;
	public:
		AutoFactory(Stamper *aStamper) {
			stamper = aStamper;
			if (!stamper) throw "Error!";
		}
		virtual ~AutoFactory() {
			delete stamper;
			stamper = nullptr;
		}
		virtual Frame *createFrame() = 0;
		virtual Weel *createWeel() = 0;
	};
	class Honda : public AutoFactory {
	public:
		Honda() : AutoFactory(new HondaStamper()) {}
		virtual ~Honda() {}
		virtual Frame *createFrame() {
			Frame *frame = new HondaFrame();
			stamper -> setStamp(frame);
			return frame;
		}
		virtual Weel *createWeel() {
			Weel *weel = new HondaWeel();
			stamper -> setStamp(weel);
			return weel;
		}
	};
	class BMW : public AutoFactory {
	public:
		BMW() : AutoFactory(new BMWStamper()) {}
		virtual ~BMW() {}
		virtual Frame *createFrame() {
			Frame *frame = new BMWFrame();
			stamper -> setStamp(frame);
			return frame;
		}
		virtual Weel *createWeel() {
			Weel *weel = new BMWWeel();
			stamper -> setStamp(weel);
			return weel;
		}
	};

	void TestSuite() {
//#define HONDAF
#define BMWF
		AutoFactory *factory = nullptr;
#ifdef HONDAF
		factory = new Honda();
#endif
#ifdef BMWF
		factory = new BMW();
#endif
		Component *cmps[2] = {
			factory -> createFrame(),
			factory -> createWeel()
		};
		std::cout << *cmps[0] << std::endl;
		std::cout << *cmps[1] << std::endl;
	}
}