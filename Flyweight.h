#pragma once

#include <unordered_map>
#include <vector>

namespace Flyweight {
	// extrinsic (non-shareable) state
	class Frame {
		int x, y, w, h;
	public:
		Frame(int xx = 0, int yy = 0, int ww = 0, int hh = 0) {
			x = xx; y = yy; w = ww; h = hh;
		}
		friend std::ostream &operator<<(std::ostream &os, const Frame &frame);
	};
	
	std::ostream &operator<<(std::ostream &os, const Frame &frame) {
		os << "Origin " << "{x : " << frame.x << ", y : " << frame.y << "} ";
		os << "Size " << "{w : " << frame.w << ", h : " << frame.h << "}";
		return os;
	}

	// flyweight
	class Image {
		// intrinsic (shareable) state
		std::string name;
	public:
		Image(std::string aName) {
			name = aName;
		}
		const std::string &getName() {
			return name;
		}
		void draw(Frame frame) const {
			std::cout << "Drawing Image [" << name << "] ";
			std::cout << "in frame " << frame;
			std::cout << std::endl;
		}
	};
	
	// abstaract factory
	class BaseImageLoader {
	public:
		BaseImageLoader() {}
		virtual ~BaseImageLoader() {};
		virtual Image *loadImage(std::string &imageName) = 0;
		virtual const Image *getImage(std::string &imageName) = 0;
	};
	
	class ImageLoader : public BaseImageLoader {
	public:
		virtual ~ImageLoader() {}
		virtual Image *loadImage(std::string &imageName) {
			return new Image(imageName);
		}
		virtual const Image *getImage(std::string &imageName) {
			return ImageLoader::loadImage(imageName);
		}
	};
	
	// decorator
	class ImageLoaderAddon : public BaseImageLoader {
		BaseImageLoader *imageLoader;
	public:
		ImageLoaderAddon(BaseImageLoader *anImageLoader = nullptr) : BaseImageLoader() {
			imageLoader = anImageLoader;
			if (!imageLoader) throw "Error!";
		}
		virtual ~ImageLoaderAddon() {
			delete imageLoader;
			imageLoader = nullptr;
		}
		virtual Image *loadImage(std::string &imageName) {
			return imageLoader -> loadImage(imageName);
		}
		virtual const Image *getImage(std::string &imageName) {
			return imageLoader -> getImage(imageName);
		}
	};
	
//	class Security : public ImageLoaderAddon {
//	};
	
	class Cache : public ImageLoaderAddon {
		std::unordered_map<std::string, Image *> cache;
	public:
		Cache(BaseImageLoader *anImageLoader) : ImageLoaderAddon(anImageLoader) {}
		virtual ~Cache() {
			for (auto &entry : cache) {
				delete entry.second;
				entry.second = nullptr;
			}
		}
		virtual const Image *getImage(std::string &imageName) {
			if (!cache.count(imageName)) {
				std::cout << "Loading Image [" << imageName << "] " << std::endl;
				cache[imageName] = ImageLoaderAddon::loadImage(imageName);
			} else
				std::cout << "Reusing Image [" << imageName << "] " << std::endl;
				
			return cache[imageName];
		}
	};
	
	static std::unordered_map<std::string, std::vector<std::string>> www = {
		{"page1.html", {"img1.png", "img2.png", "img3.png"}},
		{"page2.html", {"img2.png", "img4.png"}},
		{"page3.html", {"img1.png", "img4.png", "img5.png", "img6.png"}}
	};
	
	class Browser {
		BaseImageLoader *imageLoader;
	public:
		Browser() {
			imageLoader = new Cache(new ImageLoader());
		}
		~Browser() {
			delete imageLoader;
			imageLoader = nullptr;
		}
		void drawPage(std::string &pageName) {
			std::vector<std::string> &imageNames = www.at(pageName);
			std::cout << "Drawing Page [" << pageName << "] " << std::endl;
			for (auto &imageName : imageNames) {
				const Image *image = imageLoader -> getImage(imageName);
				image -> draw(Frame());
			}
		}
	};
	
	void TestSuite() {
		Browser *b = new Browser();
		std::vector<std::string> pageNames = {
			"page1.html",
			"page2.html",
			"page3.html"
		};
		for (auto &pageName : pageNames)
			b -> drawPage(pageName);
		delete b;
	}
}