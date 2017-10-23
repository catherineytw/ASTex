#include <iostream>
#include <ASTex/image_rgb.h>

using namespace ASTex;

/**
 * usage of iterators for const traversals (no modif of image)
 * Define where to iterate:
 *  -  it = image.beginConstIterator()
 *  -  it = image.beginConstIterator(x,y,w,h)
 *  -  it = image.beginConstIterator(reg)
 * Test for ending:
 *  -  !it.IsAtEnd()
 * Iter: ++it
 */
void test_const_iterator(const ImageRGBu8& image)
{
	uint32_t nb = 0;
	RGBd average(0.0);

	// The three following usages of ConstIterator vary only with parameter of beginConstIterator
	// no param, 4 int, or Region

	// 1 - IMAGE
	// traverse whole image with iterator
	// we have to use ConstIterator because image is const &
	for (ImageRGBu8::ConstIterator it = image.beginConstIterator(); !it.IsAtEnd(); ++it)
	{
		// construct a RGBd from itValue for using +=
		average += RGBd(it.Value());
		nb ++;
	}
	average /= nb;
	std::cout << "Average; "<< average << std::endl;

	uint32_t W = image.width();
	uint32_t H = image.height();
	nb = 0;
	average = RGBd(0.0);

	// 2 - ZONE x,y,w,h
	// traverse a part of image (a quad of size half of image centered)
	// Region is given by  begin pos x, begin pos y, width, height.
	for (ImageRGBu8::ConstIterator it = image.beginConstIterator(W/4, H/4, W/2, H/2); !it.IsAtEnd(); ++it)
	{
		average += RGBd(it.Value());
		nb ++;
	}
	average /= nb;
	std::cout << "Average; "<< average << std::endl;


	Region reg= gen_region(W/4, H/4, W/2, H/2);
	nb = 0;
	average = RGBd(0.0);

	// 3 - Region
	// traverse a region
	for (ImageRGBu8::ConstIterator it = image.beginConstIterator(reg); !it.IsAtEnd(); ++it)
	{
		average += RGBd(it.Value());
		nb ++;
	}
	average /= nb;
	std::cout << "Average; "<< average << std::endl;

}

/**
 * usage of iterator for modifications
 */
void test_iterator(ImageRGBu8& image)
{
	// traverse image with modification: useIterator
	for (ImageRGBu8::Iterator it = image.beginIterator(); !it.IsAtEnd(); ++it)
	{
		// it.Value -> itk::RGBPixel
		// convert ref into RGB for /=
		RGBu8::convert(it.Value()) /= 2;
	}
	// Same possible parameter than with ConstIterators (x,y,w,h) (Region)
}


/**
 * usage of several iterators concurrently
 */
void test_many_iterators(const ImageRGBu8& input1, const ImageRGBu8& input2, ImageRGBd& output)
{
	// check size compatibility
	if (!(output.is_initialized_as(input1))&&(output.is_initialized_as(input2)))
		return;

	// traverse the three images
	auto it_in1 = input1.beginConstIterator();
	auto it_in2 = input2.beginConstIterator();
	auto it_out = output.beginIterator();

	while (!it_out.IsAtEnd())
	{
		it_out.Value() = RGBd(it_in1.Value()) + RGBd(it_in2.Value());
		RGBd::convert(it_out.Value()) /= 512.0;
		 ++it_out,++it_in1,++it_in2;
	}

	// or, for those who do not like to pollute global scope, use for & tuple:
	// 0 : output, 1 input1, 2 input2
	for (auto its = std::make_tuple(output.beginIterator(),input1.beginConstIterator(),input2.beginConstIterator());
		 !std::get<0>(its).IsAtEnd(); ++std::get<0>(its), ++std::get<1>(its), ++std::get<2>(its))
	{
		std::get<0>(its).Value() = RGBd(std::get<1>(its).Value()) + RGBd(std::get<2>(its).Value());
		RGBd::convert(std::get<0>(its).Value()) /= 512.0;
	}

}


int main()
{
	ImageRGBu8 image;

	bool ok = image.load(TEMPO_PATH+"simpleRGB.png");
	if (!ok)
		return 1;

	test_const_iterator(image);
	test_iterator(image);
	test_const_iterator(image);

	ImageRGBu8 image2;
	image2.share(image); // same data

	ImageRGBd image3;
	test_many_iterators(image,image2, image3);

  return EXIT_SUCCESS;
}



