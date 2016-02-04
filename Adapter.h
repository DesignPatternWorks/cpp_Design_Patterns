#pragma once

namespace Adapter {
	enum Type {
		None = 0,
		Male,
		Female
	};
	
	class Properties {
		int size;
		Type type;
	public:
		Properties(int aSize = 0, Type aType = None) {
			size = aSize;
			type = aType;
		}
		bool doesFit(const Properties &props) const {
			if (props.size == size)
				if ((props.type == Male && type == Female) ||
					(props.type == Female && type == Male))
					return true;
			return false;
		}
		static Properties propsToFit(const Properties &props) {
			Type t = (props.type == Female ? Male : (props.type == Male ? Female : None));
			return Properties(props.size, t);
		}
		static std::string typeDesc(Type t) {
			switch (t) {
				case None:
					return "None";
					break;
				case Female:
					return "Feamale";
					break;
				case Male:
					return "Male";
					break;
			}
			return "";
		}
		friend std::ostream &operator<<(std::ostream &os, const Properties &props);
	};
	
	std::ostream &operator<<(std::ostream &os, const Properties &props) {
		os << "[" << props.size << ", " << Properties::typeDesc(props.type) << "]";
		return os;
	}

	// adaptee
	class Ratchet {
		Properties props;
	public:
		Ratchet(Properties aProps) {
			props = aProps;
		}
		const Properties &getProps() const {
			return props;
		}
		void rotate() {
			std::cout << "Rotating Ratchet" << std::endl;
			std::cout << "Ratchet Props " << props << std::endl;
		}
	};
	
	// interface that client expects
	class BaseHummer {
	public:
		virtual void hit() = 0;
		virtual ~BaseHummer() {};
	};
	
	// adapter
	class HummerRatchet : public BaseHummer {
		Properties props;
		Ratchet *ratchet;
	public:
		HummerRatchet(Ratchet *aRatchet) {
			ratchet = aRatchet;
			if (ratchet)
				props = Properties::propsToFit(ratchet -> getProps());
		}
		virtual ~HummerRatchet() {
			if (ratchet)
				delete ratchet;
			ratchet = nullptr;
		}
		virtual void hit() {
			if (ratchet && props.doesFit(ratchet -> getProps())) {
				std::cout << "Hitting with Ratchet" << std::endl;
				std::cout << "Ratchet Props " << ratchet -> getProps() << std::endl;
				std::cout << "Adapter Props " << props << std::endl;
			}
		}
	};
	
	// client
	class Nail {
	public:
		void getHit(BaseHummer *hummer) {
			std::cout << "Requesting Nail Hit" << std::endl;
			hummer -> hit();
		}
	};
	
	void TestSuite() {
		Nail *n = new Nail();
		BaseHummer *h = new HummerRatchet(new Ratchet(Properties(14, Female)));
		n -> getHit(h);
		delete n;
		delete h;
	}
}