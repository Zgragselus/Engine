#include "DevIL.h"

using namespace Engine;

LoaderDevIL::Image* LoaderDevIL::Load(const std::string& filename) const
{
	if (!Files::Exists(filename))
	{
		mLog->Print("LoaderDevIL", std::string("File ") + filename + std::string(" does not exist!"));
		return NULL;
	}

	mLog->Print("LoaderDevIL", std::string("Importing file: ") + filename);

	ILuint image;

	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage(filename.c_str());

	ILenum error;
	while ((error = ilGetError()) != IL_NO_ERROR) 
	{
		mLog->Print("LoaderDevIL", std::string("Error: ") + std::to_string(error) + " " + std::string(iluErrorString(error)));
	}

	ILuint width, height, channels, bpp;
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	channels = ilGetInteger(IL_IMAGE_CHANNELS);
	bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	mLog->Print("LoaderDevIL", std::string("Info: Width = ") + std::to_string(width) + std::string(" Height = ") + std::to_string(height) + 
		std::string(" Channels = ") + std::to_string(channels) + std::string(" Bytes Per Pixel = ") + std::to_string(bpp));

	ILubyte* data = ilGetData();

	unsigned int r = 0;
	unsigned int b = 2;

	if (filename.find(".tga") != std::string::npos)
	{
		r = 2;
		b = 0;
		iluFlipImage();
	}

	Image* result = new Image();
	result->mBpp = (size_t)bpp;
	result->mChannels = (size_t)channels;
	if (result->mChannels == 1)
	{
		result->mUseAlphaMipmap = false;
		result->mChannels = 4;
		result->mBpp = 4;
		result->mData = new unsigned char[width * height * 4];

		for (unsigned int j = 0; j < height; j++)
		{
			for (unsigned int i = 0; i < width; i++)
			{
				result->mData[(j * width + i) * 4 + 0] = data[(j * width + i)];
				result->mData[(j * width + i) * 4 + 1] = data[(j * width + i)];
				result->mData[(j * width + i) * 4 + 2] = data[(j * width + i)];
				result->mData[(j * width + i) * 4 + 3] = 255;
			}
		}
	}
	else if (result->mChannels == 3)
	{
		result->mUseAlphaMipmap = false;
		result->mChannels = 4;
		result->mBpp = 4;
		result->mData = new unsigned char[width * height * 4];

		for (unsigned int j = 0; j < height; j++)
		{
			for (unsigned int i = 0; i < width; i++)
			{
				result->mData[(j * width + i) * 4 + 0] = data[(j * width + i) * 3 + r];
				result->mData[(j * width + i) * 4 + 1] = data[(j * width + i) * 3 + 1];
				result->mData[(j * width + i) * 4 + 2] = data[(j * width + i) * 3 + b];
				result->mData[(j * width + i) * 4 + 3] = 255;
			}
		}
	}
	else
	{
		result->mUseAlphaMipmap = true;
		result->mData = new unsigned char[width * height * bpp];

		for (unsigned int j = 0; j < height; j++)
		{
			for (unsigned int i = 0; i < width; i++)
			{
				result->mData[(j * width + i) * 4 + 0] = data[(j * width + i) * 4 + r];
				result->mData[(j * width + i) * 4 + 1] = data[(j * width + i) * 4 + 1];
				result->mData[(j * width + i) * 4 + 2] = data[(j * width + i) * 4 + b];
				result->mData[(j * width + i) * 4 + 3] = data[(j * width + i) * 4 + 3];
			}
		}
	}
	result->mHeight = (size_t)height;
	result->mWidth = (size_t)width;
	result->mName = filename;

	ilDeleteImages(1, &image);

	return result;
}

float LoaderDevIL::GetCoverage(size_t width, size_t height, unsigned char* data, float alphaCoverageRatio, float scale) const
{
	float coverage = 0.0f;
	const unsigned int n = 8;

	for (size_t i = 0; i < height - 1; i++)
	{
		for (size_t j = 0; j < width - 1; j++)
		{
			float alpha00 = scale * (float)(data[(i * width + j) * 4 + 3]) / 255.0f;
			float alpha10 = scale * (float)(data[(i * width + j + 1) * 4 + 3]) / 255.0f;
			float alpha01 = scale * (float)(data[((i + 1) * width + j) * 4 + 3]) / 255.0f;
			float alpha11 = scale * (float)(data[((i + 1) * width + j + 1) * 4 + 3]) / 255.0f;

			for (float fy = 0.5f / n; fy < 1.0f; fy++)
			{
				for (float fx = 0.5f / n; fx < 1.0f; fx++) 
				{
					float alpha = alpha00 * (1 - fx) * (1 - fy) + alpha10 * fx * (1 - fy) + alpha01 * (1 - fx) * fy + alpha11 * fx * fy;
					if (alpha > alphaCoverageRatio)
					{
						coverage += 1.0f;
					}
				}
			}
		}
	}

	return coverage / float(width * height * n * n);
}

void LoaderDevIL::ScaleCoverage(size_t width, size_t height, unsigned char* data, float alphaCoverageRatio, const float desiredCoverage) const
{
	float minScale = 0.0f;
	float maxScale = 4.0f;
	float scale = 1.0f;

	const int iterations = 10;
	for (int scalingIndex = 0; scalingIndex < iterations; scalingIndex++)
	{
		float curCoverage = GetCoverage(width, height, data, alphaCoverageRatio, scale);

		if (curCoverage < desiredCoverage)
		{
			minScale = scale;
		}
		else if (curCoverage > desiredCoverage)
		{
			maxScale = scale;
		}
		else
		{
			break;
		}

		scale = (minScale + maxScale) * 0.5f;
	}

	if (scale == 1.0f)
	{
		return;
	}

	float minVarience = fabsf(desiredCoverage - GetCoverage(width, height, data, alphaCoverageRatio, minScale));
	float curVarience = fabsf(desiredCoverage - GetCoverage(width, height, data, alphaCoverageRatio, scale));
	float maxVarience = fabsf(desiredCoverage - GetCoverage(width, height, data, alphaCoverageRatio, maxScale));
	
	mLog->Print("LoaderDevIL", std::string("Variance: ") + std::to_string(minVarience) + std::string(", ") + std::to_string(curVarience) + std::string(", ") + std::to_string(maxVarience));

	if (minVarience < curVarience && minVarience < maxVarience)
	{
		scale = minScale;
	}
	else if (maxVarience < minVarience && maxVarience < curVarience)
	{
		scale = maxScale;
	}

	mLog->Print("LoaderDevIL", std::string("Alpha mipmap generated with coverage scale: ") + std::to_string(scale));

	for (size_t j = 0; j < height; j++)
	{
		for (size_t k = 0; k < width; k++)
		{
			size_t current = (k + j * width) * 4;
			unsigned int alpha = (unsigned int)((float)(data[current + 3]) * scale);
			data[current + 3] = alpha > 255 ? 255 : alpha;
		}
	}
}

void LoaderDevIL::GenerateMipmaps(Image* image) const
{
	// Calculate number of mipmaps
	size_t dimLog2[2] = { 0 };
	size_t width = image->mWidth;
	size_t height = image->mHeight;

	while (width > 1)
	{
		width /= 2;
		dimLog2[0]++;
	}

	while (height > 1)
	{
		height /= 2;
		dimLog2[1]++;
	}

	image->mMipLevels = dimLog2[0] > dimLog2[1] ? dimLog2[1] : dimLog2[0];

	// Generate output buffer and fill in first layer (lowest mip level)
	image->mMipmaps = new unsigned char*[image->mMipLevels];
	image->mMipmaps[0] = image->mData;

	float baseCoverage = 1.0f;
	if (image->mUseAlphaMipmap)
	{
		baseCoverage = GetCoverage(image->mWidth, image->mHeight, image->mData, 15.0f / 16.0f, 1.0f);
		mLog->Print("LoaderDevIL", std::string("Building alpha mipmaps, target coverage is: ") + std::to_string(baseCoverage));
	}
	else
	{
		mLog->Print("LoaderDevIL", "Building standard mipmaps...");
	}

	// Build mip maps
	width = image->mWidth;
	height = image->mHeight;
	for (size_t i = 1; i < image->mMipLevels; i++)
	{
		size_t newWidth = width / 2;
		newWidth = newWidth == 0 ? 1 : newWidth;
		size_t newHeight = height / 2;
		newHeight = newHeight == 0 ? 1 : newHeight;

		image->mMipmaps[i] = new unsigned char[newWidth * newHeight * 4];

		for (size_t j = 0; j < newHeight; j++)
		{
			for (size_t k = 0; k < newWidth; k++)
			{
				size_t current = (k + j * newWidth) * image->mChannels;

				size_t sample[4];
				sample[0] = (k * 2 + j * 2 * width) * image->mChannels;
				sample[1] = (k * 2 + 1 + j * 2 * width) * image->mChannels;
				sample[2] = (k * 2 + (j * 2 + 1) * width) * image->mChannels;
				sample[3] = (k * 2 + 1 + (j * 2 + 1) * width) * image->mChannels;

				unsigned int sum[4];
				sum[0] = (unsigned int)(image->mMipmaps[i - 1][sample[0] + 0]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[1] + 0]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[2] + 0]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[3] + 0]);
				sum[1] = (unsigned int)(image->mMipmaps[i - 1][sample[0] + 1]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[1] + 1]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[2] + 1]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[3] + 1]);
				sum[2] = (unsigned int)(image->mMipmaps[i - 1][sample[0] + 2]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[1] + 2]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[2] + 2]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[3] + 2]);
				sum[3] = (unsigned int)(image->mMipmaps[i - 1][sample[0] + 3]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[1] + 3]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[2] + 3]) +
					(unsigned int)(image->mMipmaps[i - 1][sample[3] + 3]);
				sum[0] /= 4;
				sum[1] /= 4;
				sum[2] /= 4;
				sum[3] /= 4;

				image->mMipmaps[i][current + 0] = sum[0];
				image->mMipmaps[i][current + 1] = sum[1];
				image->mMipmaps[i][current + 2] = sum[2];
				image->mMipmaps[i][current + 3] = sum[3];
			}
		}

		if (image->mUseAlphaMipmap)
		{
			ScaleCoverage(newWidth, newHeight, image->mMipmaps[i], 15.0f / 16.0f, baseCoverage);
		}

		width = newWidth;
		height = newHeight;
	}
}

void LoaderDevIL::GenerateMipmapsNormal(Image* image) const
{
	// Calculate number of mipmaps
	size_t dimLog2[2] = { 0 };
	size_t width = image->mWidth;
	size_t height = image->mHeight;

	while (width > 1)
	{
		width /= 2;
		dimLog2[0]++;
	}

	while (height > 1)
	{
		height /= 2;
		dimLog2[1]++;
	}

	image->mMipLevels = dimLog2[0] > dimLog2[1] ? dimLog2[1] : dimLog2[0];

	// Generate output buffer and fill in first layer (lowest mip level)
	image->mMipmaps = new unsigned char*[image->mMipLevels];
	image->mMipmaps[0] = image->mData;
	
	// Build mip maps
	width = image->mWidth;
	height = image->mHeight;
	for (size_t i = 1; i < image->mMipLevels; i++)
	{
		size_t newWidth = width / 2;
		newWidth = newWidth == 0 ? 1 : newWidth;
		size_t newHeight = height / 2;
		newHeight = newHeight == 0 ? 1 : newHeight;

		image->mMipmaps[i] = new unsigned char[newWidth * newHeight * 4];
		
		float nIn[4][3] = { 0.0f };
		float nOut[3] = { 0.0f };

		for (size_t j = 0; j < newHeight; j++)
		{
			for (size_t k = 0; k < newWidth; k++)
			{
				size_t current = (k + j * newWidth) * image->mChannels;

				size_t sample[4];
				sample[0] = (k * 2 + j * 2 * width) * image->mChannels;
				sample[1] = (k * 2 + 1 + j * 2 * width) * image->mChannels;
				sample[2] = (k * 2 + (j * 2 + 1) * width) * image->mChannels;
				sample[3] = (k * 2 + 1 + (j * 2 + 1) * width) * image->mChannels;

				ReadNormal(sample[0], image->mMipmaps[i - 1], &(nIn[0][0]), &(nIn[0][1]), &(nIn[0][2]));
				ReadNormal(sample[1], image->mMipmaps[i - 1], &(nIn[1][0]), &(nIn[1][1]), &(nIn[1][2]));
				ReadNormal(sample[2], image->mMipmaps[i - 1], &(nIn[2][0]), &(nIn[2][1]), &(nIn[2][2]));
				ReadNormal(sample[3], image->mMipmaps[i - 1], &(nIn[3][0]), &(nIn[3][1]), &(nIn[3][2]));

				nOut[0] = nIn[0][0] + nIn[1][0] + nIn[2][0] + nIn[3][0];
				nOut[1] = nIn[0][1] + nIn[1][1] + nIn[2][1] + nIn[3][1];
				nOut[2] = nIn[0][2] + nIn[1][2] + nIn[2][2] + nIn[3][2];
				
				WriteNormal(current, image->mMipmaps[i], nOut[0], nOut[1], nOut[2]);
				image->mMipmaps[i][current + 3] = 255;
			}
		}

		width = newWidth;
		height = newHeight;
	}
}