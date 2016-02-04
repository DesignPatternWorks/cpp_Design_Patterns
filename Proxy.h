#pragma once

#include <unistd.h>

namespace Proxy {
	class Image {
		std::string name;
	public:
		Image(std::string aName) {
			name = aName;
		}
		const std::string &getName() {
			return name;
		}
	};
	
	// subject
	class View {
	public:
		View(){}
		virtual ~View() {}
		virtual void draw() {
			std::cout << "Drawing View" << std::endl;
		}
	};
	
	// real subject
	class ImageView : public View {
		Image *image;
		ImageView() {}
	public:
		ImageView(Image *anImage) {
			image = anImage;
		}
		virtual ~ImageView() {
			delete image;
			image = nullptr;
		}
		virtual void draw() {
			std::cout << "Drawing Image View [" << image -> getName() << "]" << std::endl;
		}
		void setImage(Image *anImage) {
			if (!anImage) throw "Error!";
			delete image;
			image = anImage;
			draw();
		}
	};
	
	// proxy
	class LazyImageView : public View {
		std::string url;
		ImageView *imageView;
		bool loaded;
		
		LazyImageView() {}
		Image *loadImage() {
			if (url.empty()) return nullptr;
			std::cout << "Loading Image [" << url << "]" << std::endl;
			usleep(3000000);
			return new Image(url);
		}
	public:
		LazyImageView(std::string aUrl) : View() {
			url = aUrl;
			imageView = new ImageView(new Image("~/assets/default.png"));
			loaded = false;
		}
		virtual ~LazyImageView() {
			if (imageView)
				delete imageView;
			imageView = nullptr;
		}
		virtual void draw() {
			imageView -> draw();
			if (!loaded) {
				Image *image = loadImage();
				if (image) {
					imageView -> setImage(image);
					loaded = true;
				}
			}
		}
	};
	
	void TestSuite() {
		View *iv = new LazyImageView("~/assets/image.png");
		iv -> draw();
		delete iv;
	}
}