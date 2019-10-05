#include "BitMap.h"

void BitMap::read(char* fname) {

	std::ifstream inp{ fname, std::ios_base::binary };

	if (inp) {

		inp.read((char*)&file_header, sizeof(file_header));

		if (file_header.file_type != 0x4D42) {

			throw std::runtime_error("Error! Unrecognized file format.");

		}

		inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));



		// The BMPColorHeader is used only for transparent images

		if (bmp_info_header.bit_count == 32) {

			// Check if the file has bit mask color information

			if (bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {

				inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));

				// Check if the pixel data is stored as BGRA and if the color space type is sRGB

				check_color_header(bmp_color_header);

			}
			else {

				std::cerr << "Error! The file \"" << fname << "\" does not seem to contain bit mask information\n";

				throw std::runtime_error("Error! Unrecognized file format.");

			}

		}



		// Jump to the pixel data location

		inp.seekg(file_header.offset_data, inp.beg);



		// Adjust the header fields for output.

		// Some editors will put extra info in the image file, we only save the headers and the data.

		if (bmp_info_header.bit_count == 32) {

			bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

			file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

		}
		else {

			bmp_info_header.size = sizeof(BMPInfoHeader);

			file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

		}

		file_header.file_size = file_header.offset_data;



		if (bmp_info_header.height < 0) {

			throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");

		}



		data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);



		// Here we check if we need to take into account row padding

		if (bmp_info_header.width % 4 == 0) {

			inp.read((char*)data.data(), data.size());

			file_header.file_size += data.size();

		}

		else {

			row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;

			uint32_t new_stride = make_stride_aligned(4);

			std::vector<uint8_t> padding_row(new_stride - row_stride);



			for (int y = 0; y < bmp_info_header.height; ++y) {

				inp.read((char*)(data.data() + row_stride * y), row_stride);

				inp.read((char*)padding_row.data(), padding_row.size());

			}

			file_header.file_size += data.size() + bmp_info_header.height * padding_row.size();

		}

	}

	else {

		throw std::runtime_error("Unable to open the input image file.");

	}

}



BitMap::BitMap(int32_t width, int32_t height, bool has_alpha) {

	if (width <= 0 || height <= 0) {

		throw std::runtime_error("The image width and height must be positive numbers.");

	}



	bmp_info_header.width = width;

	bmp_info_header.height = height;

	if (has_alpha) {

		bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

		file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);



		bmp_info_header.bit_count = 32;

		bmp_info_header.compression = 3;

		row_stride = width * 4;

		data.resize(row_stride * height);

		file_header.file_size = file_header.offset_data + data.size();

	}

	else {

		bmp_info_header.size = sizeof(BMPInfoHeader);

		file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);



		bmp_info_header.bit_count = 24;

		bmp_info_header.compression = 0;

		row_stride = width * 3;

		data.resize(row_stride * height);



		uint32_t new_stride = make_stride_aligned(4);

		file_header.file_size = file_header.offset_data + data.size() + bmp_info_header.height * (new_stride - row_stride);

	}

}



void BitMap::write(char* fname) {

	std::ofstream of{ fname, std::ios_base::binary };

	if (of) {

		if (bmp_info_header.bit_count == 32) {

			write_headers_and_data(of);

		}

		else if (bmp_info_header.bit_count == 24) {

			if (bmp_info_header.width % 4 == 0) {

				write_headers_and_data(of);

			}

			else {

				uint32_t new_stride = make_stride_aligned(4);

				std::vector<uint8_t> padding_row(new_stride - row_stride);



				write_headers(of);



				for (int y = 0; y < bmp_info_header.height; ++y) {

					of.write((const char*)(data.data() + row_stride * y), row_stride);

					of.write((const char*)padding_row.data(), padding_row.size());

				}

			}

		}

		else {

			throw std::runtime_error("The program can treat only 24 or 32 bits per pixel BMP files");

		}

	}

	else {

		throw std::runtime_error("Unable to open the output image file.");

	}

}

void BitMap::write_headers(std::ofstream& of) {

	of.write((const char*)&file_header, sizeof(file_header));

	of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));

	if (bmp_info_header.bit_count == 32) {

		of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));

	}

}



void BitMap::write_headers_and_data(std::ofstream& of) {

	write_headers(of);

	of.write((const char*)data.data(), data.size());

}

void BitMap::getPixel(int x, int y, Pixel& pixel) {
	if (x > (uint32_t)bmp_info_header.width || y > (uint32_t)bmp_info_header.height || x < 0 || y < 0) {
		throw std::runtime_error("Pixel coordinates out of range!");
	}
	uint32_t channels = bmp_info_header.bit_count / 8;
	pixel.b = data[channels * (y * bmp_info_header.width + x) + 0];
	pixel.g = data[channels * (y * bmp_info_header.width + x) + 1];
	pixel.r = data[channels * (y * bmp_info_header.width + x) + 2];
	if (channels == 4) {
		pixel.a = data[channels * (y * bmp_info_header.width + x) + 3];
	} else {
		pixel.a = 1;
	}
}

void BitMap::setPixel(int x, int y, Pixel& pixel) {
	if (x > (uint32_t)bmp_info_header.width || y > (uint32_t)bmp_info_header.height || x < 0 || y < 0) {
		throw std::runtime_error("Pixel coordinates out of range!");
	}
	uint32_t channels = bmp_info_header.bit_count / 8;
	data[channels * (y * bmp_info_header.width + x) + 0] = pixel.b;
	data[channels * (y * bmp_info_header.width + x) + 1] = pixel.g;
	data[channels * (y * bmp_info_header.width + x) + 2] = pixel.r;
	if (channels == 4) {
		data[channels * (y * bmp_info_header.width + x) + 3] = pixel.a;
	}
}

// Add 1 to the row_stride until it is divisible with align_stride

uint32_t BitMap::make_stride_aligned(uint32_t align_stride) {

	uint32_t new_stride = row_stride;

	while (new_stride % align_stride != 0) {

		new_stride++;

	}

	return new_stride;

}



// Check if the pixel data is stored as BGRA and if the color space type is sRGB

void BitMap::check_color_header(BMPColorHeader& bmp_color_header) {

	BMPColorHeader expected_color_header;

	if (expected_color_header.red_mask != bmp_color_header.red_mask ||

		expected_color_header.blue_mask != bmp_color_header.blue_mask ||

		expected_color_header.green_mask != bmp_color_header.green_mask ||

		expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {

		throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");

	}

	if (expected_color_header.color_space_type != bmp_color_header.color_space_type) {

		throw std::runtime_error("Unexpected color space type! The program expects sRGB values");

	}

}