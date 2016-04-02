#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace Visitor {
	// NOTE! Because of the cyclic dependencies, the declaration of the element and visitor classes will need to be interleaved.
	class Rim;
	class Tire;
	class Weel;
	class Door;
	class Frame;
	class Car;

	class CarOperation {
	public:
		virtual void visit(Rim *rim) = 0;
		virtual void visit(Tire *tire) = 0;
		virtual void visit(Weel *weel) = 0;
		virtual void visit(Door *door) = 0;
		virtual void visit(Frame *frame) = 0;
		virtual void visit(Car *car) = 0;
	};

	// element
	class Part {
		std::string color;
	public:
		virtual void accept(CarOperation *operation) = 0;
		virtual void setColor(std::string aColor) {
			std::cout << "Setting color to [" << aColor << "]." << std::endl;
			color = aColor;
		}
	};

	class Rim : public Part {
	public:
		virtual void accept(CarOperation *operation) {
			operation -> visit(this);
		}
		virtual void setColor(std::string aColor) {
			std::cout << "[Rim] ";
			Part::setColor(aColor);
		}
	};
	class Tire : public Part {
	public:
		virtual void accept(CarOperation *operation) {
			operation -> visit(this);
		}
		virtual void setColor(std::string aColor) {
			std::cout << "[Tire] Cannot set color." << std::endl;
		}
	};
	class Weel : public Part {
	public:
		Rim *rim; 
		Tire *tire;
		Weel(Rim *aRim = nullptr, Tire *aTire = nullptr) : rim(aRim), tire(aTire) {
			if (!rim) rim = new Rim();
			if (!tire) tire = new Tire();
		}
		~Weel() {
			delete rim;
			delete tire;
		}
		virtual void accept(CarOperation *operation) {
			rim -> accept(operation);
			tire -> accept(operation);
			operation -> visit(this);
		}
		virtual void setColor(std::string aColor) {
			std::cout << "[Weel] ";
			Part::setColor(aColor);
		}
	};

	class Door : public Part {
	public:
		virtual void accept(CarOperation *operation) {
			operation -> visit(this);
		}
		virtual void setColor(std::string aColor) {
			std::cout << "[Door] ";
			Part::setColor(aColor);
		}
	};
	class Frame : public Part {
	public:
		std::vector<Door *> doors;
		Frame(std::vector<Door *> theDoors = {}) : doors(theDoors) {
			if (doors.size() < 4) {
				doors = std::vector<Door *>(4, nullptr);
				for (Door *&d : doors)
					d = new Door();
			}
		}
		~Frame() {
			for (Door *&d : doors)
				delete d;
		}
		virtual void accept(CarOperation *operation) {
			for (Door *d : doors)
				d -> accept(operation);
			operation -> visit(this);
		}
		virtual void setColor(std::string aColor) {
			std::cout << "[Frame] ";
			Part::setColor(aColor);
		}
	};

	class Car : public Part {
	public:
		Frame *frame;	
		std::vector<Weel *> weels;
		Car(Frame *aFrame = nullptr, std::vector<Weel *> theWeels = {}) : frame(aFrame), weels(theWeels) {
			if (!frame) frame = new Frame();
			if (weels.size() < 4) {
				weels = std::vector<Weel *>(4, nullptr);
				for (Weel *&w : weels)
					w = new Weel();
			}
		}
		~Car() {
			delete frame;
			for (Weel *&w : weels)
				delete w;
		}
		virtual void accept(CarOperation *operation) {
			for (Weel *w : weels)
				w -> accept(operation);
			frame -> accept(operation);
			operation -> visit(this);
		}
		virtual void setColor(std::string aColor) {
			std::cout << "[Car] ";
			Part::setColor(aColor);
		}
	};

	// visitor
	class Paint : public CarOperation {
	public:
		std::string color;
		Paint(std::string aColor) : color(aColor) {
		}
		virtual void visit(Rim *rim) {
			rim -> setColor(color);
		}
		virtual void visit(Tire *tire) {
			tire -> setColor(color);
		}
		virtual void visit(Weel *weel) {
			weel -> setColor(color);
		}
		virtual void visit(Door *door) {
			door -> setColor(color);
		}
		virtual void visit(Frame *frame) {
			frame -> setColor(color);
		}
		virtual void visit(Car *car) {
			car -> setColor(color);
		}
	};

	void TestSuite() {
		Car *car = new Car();
		Paint *paint = new Paint("Red");
		car -> accept(paint);
		delete car;
		delete paint;
	}
}