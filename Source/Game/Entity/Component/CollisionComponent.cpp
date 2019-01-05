#include "ComponentStatic.h"
#include "Imgui/imgui.h"

namespace Engine
{
	bool CollisionComponent::Editor(std::string& prev, std::string& next)
	{
		bool change = false;
		float4 data;

		int type = (int)mShape->GetType();
		if (ImGui::Combo("Shape type", &type, "Box\0Capsule\0Compound\0Cone\0Convex Hull\0Cylinder\0Sphere\0Trimesh"))
		{
			if (type != (int)mShape->GetType())
			{
				change = true;
			}
		}

		switch ((Shape::Type)mShape->GetType())
		{
		case Shape::Type::BOX:
			data = ((Box*)mShape)->GetExtents();
			if (ImGui::InputFloat3("Extents", &(data[0]))) { change = true; }
			break;

		case Shape::Type::CAPSULE:
			data[0] = ((Capsule*)mShape)->GetRadius();
			data[1] = ((Capsule*)mShape)->GetHeight();
			if (ImGui::InputFloat("Radius", &(data[0]))) { change = true; }
			if (ImGui::InputFloat("Height", &(data[1]))) { change = true; }
			break;

		case Shape::Type::COMPOUND:
			//delete mShape;
			//mShape = new Compound(float4(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		case Shape::Type::CONE:
			data[0] = ((Cone*)mShape)->GetRadius();
			data[1] = ((Cone*)mShape)->GetHeight();
			if (ImGui::InputFloat("Radius", &(data[0]))) { change = true; }
			if (ImGui::InputFloat("Height", &(data[1]))) { change = true; }
			break;

		case Shape::Type::CONVEX_HULL:
			//delete mShape;
			//mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		case Shape::Type::CYLINDER:
			data = ((Cylinder*)mShape)->GetHalfExtents();
			if (ImGui::InputFloat3("Half extents", &(data[0]))) { change = true; }
			break;

		case Shape::Type::SPHERE:
			data[0] = ((Sphere*)mShape)->GetRadius();
			if (ImGui::InputFloat("Radius", &(data[0]))) { change = true; }
			break;

		case Shape::Type::TRIANGLE_MESH:
			break;
		}

		if (change)
		{
			prev = Serialize().c_str();

			if (type != (int)mShape->GetType())
			{
				switch ((Shape::Type)type)
				{
				case Shape::Type::BOX:
					delete mShape;
					mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
					break;

				case Shape::Type::CAPSULE:
					delete mShape;
					mShape = new Capsule(1.0f, 1.0f);
					break;

				case Shape::Type::COMPOUND:
					//delete mShape;
					//mShape = new Compound(float4(1.0f, 1.0f, 1.0f, 1.0f));
					break;

				case Shape::Type::CONE:
					delete mShape;
					mShape = new Cone(1.0f, 1.0f);
					break;

				case Shape::Type::CONVEX_HULL:
					//delete mShape;
					//mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
					break;

				case Shape::Type::CYLINDER:
					delete mShape;
					mShape = new Cylinder(float4(1.0f, 1.0f, 1.0f, 1.0f));
					break;

				case Shape::Type::SPHERE:
					delete mShape;
					mShape = new Sphere(1.0f);
					break;

				case Shape::Type::TRIANGLE_MESH:
					delete mShape;
					mShape = new TriangleMesh();
					break;

				default:
					break;
				}
			}

			switch ((Shape::Type)type)
			{
			case Shape::Type::BOX:
				((Box*)mShape)->mExtents = data;
				break;

			case Shape::Type::CAPSULE:
				((Capsule*)mShape)->mRadius = data[0];
				((Capsule*)mShape)->mHeight = data[1];
				break;

			case Shape::Type::COMPOUND:
				break;

			case Shape::Type::CONE:
				((Cone*)mShape)->mRadius = data[0];
				((Cone*)mShape)->mHeight = data[1];
				break;

			case Shape::Type::CONVEX_HULL:
				break;

			case Shape::Type::CYLINDER:
				((Cylinder*)mShape)->mHalfExtents = data;
				break;

			case Shape::Type::SPHERE:
				((Sphere*)mShape)->mRadius = data[0];
				break;

			case Shape::Type::TRIANGLE_MESH:
				break;

			default:
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

	void CollisionComponent::Deserialize(const std::string& s)
	{
		std::stringstream ss(s);

		std::string name;
		int type;
		ss >> name >> type;

		if (type != (int)(mShape->GetType()))
		{
			Shape::Type t = (Shape::Type)type;

			switch (t)
			{
			case Shape::Type::BOX:
				delete mShape;
				mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::CAPSULE:
				delete mShape;
				mShape = new Capsule(1.0f, 1.0f);
				break;

			case Shape::Type::COMPOUND:
				//delete mShape;
				//mShape = new Compound(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::CONE:
				delete mShape;
				mShape = new Cone(1.0f, 1.0f);
				break;

			case Shape::Type::CONVEX_HULL:
				//delete mShape;
				//mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::CYLINDER:
				delete mShape;
				mShape = new Cylinder(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::SPHERE:
				delete mShape;
				mShape = new Sphere(1.0f);
				break;

			case Shape::Type::TRIANGLE_MESH:
				delete mShape;
				mShape = new TriangleMesh();
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

		mShape->Deserialize(data);
	}
}