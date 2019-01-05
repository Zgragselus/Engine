#include "ComponentStatic.h"
#include "Imgui/imgui.h"

namespace Engine
{
	int LightComponent::mResolutionTableSize = 6;
	int LightComponent::mResolutionTable[6] = { 64, 128, 256, 512, 1024, 2048 };

	bool LightComponent::Editor(std::string& prev, std::string& next)
	{
		bool change = false;

		float4 color = mLight->mColor;

		if (ImGui::ColorEdit4("Light color", (float*)&color)) { change = true; }

		int type = (int)mLight->mType;
		if (ImGui::Combo("Light type", &type, "Point\0Spot\0Directional"))
		{
			if (type != (int)mLight->mType)
			{
				change = true;
			}
		}

		float temperature = mLight->mTemperature;
		if (ImGui::SliderFloat("Temperature (K)", &temperature, 1500.0f, 15000.0f)) { change = true; }

		float power = mLight->mPower;
		if (ImGui::InputFloat("Power (lm)", &power)) { change = true; }

		float nearPlane = mLight->mNear;
		if (ImGui::InputFloat("Shadow Near", (float*)&nearPlane)) { change = true; }

		float farPlane = mLight->mFar;
		if (ImGui::InputFloat("Shadow Far", (float*)&farPlane)) { change = true; }

		bool shadowsEnabled = mLight->mShadowEnabled == 1 ? true : false;
		if (ImGui::Checkbox("Shadows Enabled", &shadowsEnabled)) { change = true; }

		int shadowsFilter = (int)mLight->mFilter;
		if (ImGui::Combo("Shadows Filter", &shadowsFilter, "Nearest\0Bilinear\0Bicubic\0PCF\0PCSS\0PCMLSM\0BACKPROJECTION\0")) { change = true; }

		float filterSize = mLight->mFilterSize;
		if (ImGui::DragFloat("Shadows Filter Size", &filterSize, 0.0005f, 0.0f, 1.0f, "%.4f")) { change = true; }

		int resolution = 0;
		for (int i = 0; i < mResolutionTableSize; i++)
		{
			if (mResolutionTable[i] == (int)mLight->GetResolution())
			{
				resolution = i;
			}
		}
		if (ImGui::Combo("Shadow Map Resolution", &resolution, "   64 \0  128 \0  256 \0  512 \0 1024 \0 2048 ")) { change = true; }

		float lightSize = mLight->mLightSize;
		switch (mLight->mFilter)
		{
		case Light::ShadowFilter::NEAREST:
			break;

		case Light::ShadowFilter::BILINEAR:
			break;

		case Light::ShadowFilter::BICUBIC:
			break;

		case Light::ShadowFilter::PCF:
			if (ImGui::DragFloat("Light Size", &lightSize, 1.0f, 0.0f, 5.0f, "%.4f")) { change = true; }
			break;

		case Light::ShadowFilter::PCSS:
			if (ImGui::DragFloat("Light Size", &lightSize, 0.05f, 0.0f, 10.0f, "%.4f")) { change = true; }
			break;

		case Light::ShadowFilter::PCMLSM:
			if (ImGui::DragFloat("Light Size", &lightSize, 0.05f, 0.0f, 20.0f, "%.4f")) { change = true; }
			break;

		case Light::ShadowFilter::BACKPROJECTION:
			if (ImGui::DragFloat("Light Size", &lightSize, 0.05f, 0.0f, 50.0f, "%.4f")) { change = true; }
			break;
		}

		float linearAtt;
		float quadraticAtt;
		float spotAngle;
		float4 direction;

		switch (mLight->mType)
		{
		case Light::Type::POINT:
			linearAtt = ((PointLight*)mLight)->mLinearAtt;
			quadraticAtt = ((PointLight*)mLight)->mQuadraticAtt;
			if (ImGui::InputFloat("Linear Attenuation", &linearAtt)) { change = true; }
			if (ImGui::InputFloat("Quadratic Attenuation", &quadraticAtt)) { change = true; }
			break;

		case Light::Type::SPOT:
			linearAtt = ((SpotLight*)mLight)->mLinearAtt;
			quadraticAtt = ((SpotLight*)mLight)->mQuadraticAtt;
			spotAngle = ((SpotLight*)mLight)->mSpotAngle;
			direction = ((SpotLight*)mLight)->mDirection;
			if (ImGui::InputFloat("Linear Attenuation", &linearAtt)) { change = true; }
			if (ImGui::InputFloat("Quadratic Attenuation", &quadraticAtt)) { change = true; }
			if (ImGui::InputFloat("Spot Angle", &spotAngle)) { change = true; }
			if (ImGui::InputFloat4("Direction", (float*)&direction)) { change = true; }
			break;

		case Light::Type::DIRECTIONAL:
			direction = ((DirectionalLight*)mLight)->mDirection;
			if (ImGui::InputFloat4("Direction", (float*)&direction)) { change = true; }
			break;
		}

		if (change)
		{
			prev = Serialize().c_str();

			mLight->mColor = color;

			if (type != (int)mLight->mType)
			{
				switch (type)
				{
				case 0:
					delete mLight;
					mLight = new PointLight(color, 0.001f, 0.001f);
					break;

				case 1:
					delete mLight;
					mLight = new SpotLight(color, float4(1.0f, 0.0f, 0.0f, 0.0f), Math::PI / 4.0f, 0.001f, 0.001f);
					break;

				case 2:
					delete mLight;
					mLight = new DirectionalLight(color, float4(0.0f, -1.0f, 0.0f, 0.0f));
					break;

				default:
					break;
				}
			}

			if (mLight->mFilter != (Light::ShadowFilter)shadowsFilter)
			{
				mLight->mLightSize = 1.0f;
			}

			mLight->mNear = nearPlane;
			mLight->mFar = farPlane;
			mLight->mShadowEnabled = shadowsEnabled ? 1 : 0;
			mLight->mFilter = (Light::ShadowFilter)shadowsFilter;
			mLight->mFilterSize = filterSize;
			mLight->mResolution = mResolutionTable[resolution];
			mLight->mLightSize = lightSize;
			mLight->mTemperature = temperature;
			mLight->mPower = power;

			switch (mLight->mType)
			{
			case Light::Type::POINT:
				((PointLight*)mLight)->mLinearAtt = linearAtt;
				((PointLight*)mLight)->mQuadraticAtt = quadraticAtt;
				break;

			case Light::Type::SPOT:
				((PointLight*)mLight)->mLinearAtt = linearAtt;
				((SpotLight*)mLight)->mQuadraticAtt = quadraticAtt;
				((SpotLight*)mLight)->mSpotAngle = spotAngle;
				((SpotLight*)mLight)->mDirection = normalize(direction);
				break;

			case Light::Type::DIRECTIONAL:
				((DirectionalLight*)mLight)->mDirection = normalize(direction);
				break;
			}

			next = Serialize().c_str();

			if (next == prev)
			{
				change = false;
			}
		}

		return change;
	}

	void LightComponent::Deserialize(const std::string& s)
	{
		std::stringstream ss(s);

		std::string name;
		float tmp;
		int type;
		ss >> name >> tmp >> tmp >> tmp >> tmp >> type;

		if (type != (int)(mLight->mType))
		{
			switch (type)
			{
			case 0:
				delete mLight;
				mLight = new PointLight(float4(0.0f, 0.0f, 0.0f, 0.0f), 0.001f, 0.001f);
				break;

			case 1:
				delete mLight;
				mLight = new SpotLight(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 0.0f, 0.0f, 0.0f), Math::PI / 4.0f, 0.001f, 0.001f);
				break;

			case 2:
				delete mLight;
				mLight = new DirectionalLight(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, -1.0f, 0.0f, 0.0f));
				break;

			default:
				break;
			}
		}

		std::vector<std::string> lines = String::Split(s, '\n');

		std::vector<std::string> dataVec;
		dataVec.push_back(lines[1]);
		dataVec.push_back(lines[2]);
		std::string data = String::Join(dataVec, '\n');

		mLight->Deserialize(data);
	}
}