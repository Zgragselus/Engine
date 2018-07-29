#ifndef __RENDER_GIZMO__H__
#define __RENDER_GIZMO__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderGizmo
	{
	public:
		enum class Gizmo
		{
			LIGHT = 0
		};

	protected:
		Engine::RootSignature* mGizmoRS;
		Engine::PipelineState* mGizmoPS;
		Engine::D3DShader* mGizmo;

		Engine::GpuBuffer* mLinesVBO;
		float* mLinesVBOMem;
		size_t mLinesVBOSize;
		size_t mLinesVBOUsage;

		Engine::PipelineState* mGizmoFlatPS;

		Engine::GpuBuffer* mTrianglesVBO;
		float* mTrianglesVBOMem;
		size_t mTrianglesVBOSize;
		size_t mTrianglesVBOUsage;

		Engine::RootSignature* mGizmoIconsRS;
		Engine::PipelineState* mGizmoIconsPS;
		Engine::D3DShader* mGizmoIcons;

		Engine::PipelineState* mGizmoPickingPS;
		Engine::D3DShader* mGizmoPicking;

		unsigned int mIconsAtlasSize;				// Size of single icon in texture atlas
		unsigned int mIconsAtlasCount[2];			// Number of icons in row/column in texture atlas
		unsigned int mIconsAtlasResolution[2];		// Total resolution of texture atlas
		float mIconsAtlasStep[2];					// Step for one icon in atlas
		Engine::Texture* mIconsTexture;
		Engine::GpuBuffer* mIconsVBO;
		float* mIconsVBOMem;
		size_t mIconsVBOSize;
		size_t mIconsVBOUsage;

		Engine::GpuMappedBuffer* mCameraBuffer;
		void* mCameraBufferPtr;

		Engine::GraphicsContext* mContext;

		inline void GetIconOffsets(Gizmo icon, float& u0, float& v0, float& u1, float& v1)
		{
			int row = (int)icon / mIconsAtlasCount[0];
			int col = (int)icon % mIconsAtlasCount[0];
			u0 = (float)col * mIconsAtlasStep[0];
			u1 = (float)(col + 1) * mIconsAtlasStep[0];
			v0 = (float)row * mIconsAtlasStep[1];
			v1 = (float)(row + 1) * mIconsAtlasStep[1];
		}

		inline float Pack(const Engine::float4& value)
		{
			unsigned int result = (unsigned int)((unsigned char)(value.w * 255.0f)) << 24;
			result += (unsigned int)((unsigned char)(value.z * 255.0f)) << 16;
			result += (unsigned int)((unsigned char)(value.y * 255.0f)) << 8;
			result += (unsigned int)((unsigned char)(value.x * 255.0f));
			return *(float*)&result;
		}

		void ProcessRenderNode(Engine::RenderNode& node);

	public:
		inline void AddLine(const Engine::float4& v0, const Engine::float4& c0, const Engine::float4& v1, const Engine::float4& c1)
		{
			if (mLinesVBOUsage + 10 >= mLinesVBOSize)
			{
				printf("Error: mLinesVBOUsage is too small!\n");
				return;
			}

			mLinesVBOMem[mLinesVBOUsage] = v0.x;
			mLinesVBOMem[mLinesVBOUsage + 1] = v0.y;
			mLinesVBOMem[mLinesVBOUsage + 2] = v0.z;
			mLinesVBOMem[mLinesVBOUsage + 3] = 1.0f;
			mLinesVBOMem[mLinesVBOUsage + 4] = Pack(c0);

			mLinesVBOUsage += 5;

			mLinesVBOMem[mLinesVBOUsage] = v1.x;
			mLinesVBOMem[mLinesVBOUsage + 1] = v1.y;
			mLinesVBOMem[mLinesVBOUsage + 2] = v1.z;
			mLinesVBOMem[mLinesVBOUsage + 3] = 1.0f;
			mLinesVBOMem[mLinesVBOUsage + 4] = Pack(c1);

			mLinesVBOUsage += 5;
		}

		inline void AddWidget(const Engine::float4& v, unsigned int id, Gizmo gizmo)
		{
			if (mIconsVBOUsage + 9 >= mIconsVBOSize)
			{
				printf("Error: mIconsVBOUsage is too small!\n");
				return;
			}

			float coord[2][2];
			GetIconOffsets(gizmo, coord[0][0], coord[1][0], coord[0][1], coord[1][1]);

			mIconsVBOMem[mIconsVBOUsage] = v.x;
			mIconsVBOMem[mIconsVBOUsage + 1] = v.y;
			mIconsVBOMem[mIconsVBOUsage + 2] = v.z;
			mIconsVBOMem[mIconsVBOUsage + 3] = 1.0f;
			mIconsVBOMem[mIconsVBOUsage + 4] = coord[0][0];
			mIconsVBOMem[mIconsVBOUsage + 5] = coord[1][0];
			mIconsVBOMem[mIconsVBOUsage + 6] = coord[0][1];
			mIconsVBOMem[mIconsVBOUsage + 7] = coord[1][1];
			mIconsVBOMem[mIconsVBOUsage + 8] = *(float*)&id;

			mIconsVBOUsage += 9;
		}

		inline void AddTriangle(const Engine::float4& v0, const Engine::float4& c0,
			const Engine::float4& v1, const Engine::float4& c1,
			const Engine::float4& v2, const Engine::float4& c2)
		{
			if (mTrianglesVBOUsage + 15 >= mTrianglesVBOSize)
			{
				printf("Error: mTrianglesVBOUsage is too small!\n");
				return;
			}

			mTrianglesVBOMem[mTrianglesVBOUsage] = v0.x;
			mTrianglesVBOMem[mTrianglesVBOUsage + 1] = v0.y;
			mTrianglesVBOMem[mTrianglesVBOUsage + 2] = v0.z;
			mTrianglesVBOMem[mTrianglesVBOUsage + 3] = 1.0f;
			mTrianglesVBOMem[mTrianglesVBOUsage + 4] = Pack(c0);

			mTrianglesVBOUsage += 5;

			mTrianglesVBOMem[mTrianglesVBOUsage] = v1.x;
			mTrianglesVBOMem[mTrianglesVBOUsage + 1] = v1.y;
			mTrianglesVBOMem[mTrianglesVBOUsage + 2] = v1.z;
			mTrianglesVBOMem[mTrianglesVBOUsage + 3] = 1.0f;
			mTrianglesVBOMem[mTrianglesVBOUsage + 4] = Pack(c1);

			mTrianglesVBOUsage += 5;

			mTrianglesVBOMem[mTrianglesVBOUsage] = v2.x;
			mTrianglesVBOMem[mTrianglesVBOUsage + 1] = v2.y;
			mTrianglesVBOMem[mTrianglesVBOUsage + 2] = v2.z;
			mTrianglesVBOMem[mTrianglesVBOUsage + 3] = 1.0f;
			mTrianglesVBOMem[mTrianglesVBOUsage + 4] = Pack(c2);

			mTrianglesVBOUsage += 5;
		}

		inline void AddAABB(const Engine::AABB& box, const Engine::float4& color)
		{
			Engine::float4 d = box.mMax - box.mMin;
			Engine::float4 v[8];
			v[0] = box.mMin;
			v[1] = box.mMin + Engine::float4(d.x, 0.0f, 0.0f, 0.0f);
			v[2] = box.mMin + Engine::float4(0.0f, d.y, 0.0f, 0.0f);
			v[3] = box.mMin + Engine::float4(0.0f, 0.0f, d.z, 0.0f);
			v[4] = box.mMin + Engine::float4(d.x, d.y, 0.0f, 0.0f);
			v[5] = box.mMin + Engine::float4(d.x, 0.0f, d.z, 0.0f);
			v[6] = box.mMin + Engine::float4(0.0f, d.y, d.z, 0.0f);
			v[7] = box.mMin + Engine::float4(d.x, d.y, d.z, 0.0f);

			AddLine(v[0], color, v[1], color);
			AddLine(v[1], color, v[4], color);
			AddLine(v[4], color, v[2], color);
			AddLine(v[2], color, v[0], color);
			AddLine(v[3], color, v[5], color);
			AddLine(v[5], color, v[7], color);
			AddLine(v[7], color, v[6], color);
			AddLine(v[6], color, v[3], color);
			AddLine(v[0], color, v[3], color);
			AddLine(v[1], color, v[5], color);
			AddLine(v[4], color, v[7], color);
			AddLine(v[2], color, v[6], color);
		}

		inline void AddAxis(const Engine::float4& position, float scale)
		{
			AddLine(position, Engine::float4(1.0f, 0.0f, 0.0f, 1.0f), position + Engine::float4(scale, 0.0f, 0.0f, 0.0f), Engine::float4(1.0f, 0.0f, 0.0f, 1.0f));
			AddLine(position, Engine::float4(0.0f, 1.0f, 0.0f, 1.0f), position + Engine::float4(0.0f, scale, 0.0f, 0.0f), Engine::float4(0.0f, 1.0f, 0.0f, 1.0f));
			AddLine(position, Engine::float4(0.0f, 0.0f, 1.0f, 1.0f), position + Engine::float4(0.0f, 0.0f, scale, 0.0f), Engine::float4(0.0f, 0.0f, 1.0f, 1.0f));
		}

		inline void AddPoint(const Engine::float4& position, float scale)
		{
			AddLine(position - Engine::float4(scale, 0.0f, 0.0f, 0.0f), Engine::float4(1.0f, 1.0f, 0.0f, 1.0f), position + Engine::float4(scale, 0.0f, 0.0f, 0.0f), Engine::float4(1.0f, 1.0f, 0.0f, 1.0f));
			AddLine(position - Engine::float4(0.0f, scale, 0.0f, 0.0f), Engine::float4(1.0f, 1.0f, 0.0f, 1.0f), position + Engine::float4(0.0f, scale, 0.0f, 0.0f), Engine::float4(1.0f, 1.0f, 0.0f, 1.0f));
			AddLine(position - Engine::float4(0.0f, 0.0f, scale, 0.0f), Engine::float4(1.0f, 1.0f, 0.0f, 1.0f), position + Engine::float4(0.0f, 0.0f, scale, 0.0f), Engine::float4(1.0f, 1.0f, 0.0f, 1.0f));
		}

		inline void AddRay(const Engine::float4& origin, const Engine::float4 direction)
		{
			AddPoint(origin, 5.0f);
			AddLine(origin, Engine::float4(1.0f, 1.0f, 0.0f, 1.0f), origin + direction * 10000.0f, Engine::float4(1.0f, 1.0f, 0.0f, 1.0f));
		}

		inline void AddTranslationAxis(const Engine::float4& position, const Engine::float4* basis, float scale, int axis, int plane)
		{
			Engine::float4 color[3];
			color[0] = (axis == 0 ? Engine::float4(1.0f, 1.0f, 0.0f, 1.0f) : Engine::float4(1.0f, 0.0f, 0.0f, 1.0f));
			color[1] = (axis == 1 ? Engine::float4(1.0f, 1.0f, 0.0f, 1.0f) : Engine::float4(0.0f, 1.0f, 0.0f, 1.0f));
			color[2] = (axis == 2 ? Engine::float4(1.0f, 1.0f, 0.0f, 1.0f) : Engine::float4(0.0f, 0.0f, 1.0f, 1.0f));

			Engine::float4 planeColor[3];
			planeColor[0] = (plane == 0 ? Engine::float4(1.0f, 1.0f, 0.0f, 0.5f) : Engine::float4(1.0f, 0.0f, 0.0f, 0.1f));
			planeColor[1] = (plane == 1 ? Engine::float4(1.0f, 1.0f, 0.0f, 0.5f) : Engine::float4(0.0f, 1.0f, 0.0f, 0.1f));
			planeColor[2] = (plane == 2 ? Engine::float4(1.0f, 1.0f, 0.0f, 0.5f) : Engine::float4(0.0f, 0.0f, 1.0f, 0.1f));

			AddLine(position, color[0], position + basis[0] * scale, color[0]);
			AddLine(position, color[1], position + basis[1] * scale, color[1]);
			AddLine(position, color[2], position + basis[2] * scale, color[2]);

			Engine::mat4 basisMat = Engine::mat4(basis[0], basis[1], basis[2], Engine::float4(0.0f, 0.0f, 0.0f, 1.0f));

			AddTriangle(position, planeColor[0],
				position + basisMat * Engine::float4(0.0f, scale, 0.0f, 0.0f) * 0.35f, planeColor[0],
				position + basisMat * Engine::float4(0.0f, 0.0f, scale, 0.0f) * 0.35f, planeColor[0]);
			AddTriangle(position + basisMat * Engine::float4(0.0f, scale, 0.0f, 0.0f) * 0.35f, planeColor[0],
				position + basisMat * Engine::float4(0.0f, scale, scale, 0.0f) * 0.35f, planeColor[0],
				position + basisMat * Engine::float4(0.0f, 0.0f, scale, 0.0f) * 0.35f, planeColor[0]);

			AddTriangle(position, planeColor[1],
				position + basisMat * Engine::float4(scale, 0.0f, 0.0f, 0.0f) * 0.35f, planeColor[1],
				position + basisMat * Engine::float4(0.0f, 0.0f, scale, 0.0f) * 0.35f, planeColor[1]);
			AddTriangle(position + basisMat * Engine::float4(scale, 0.0f, 0.0f, 0.0f) * 0.35f, planeColor[1],
				position + basisMat * Engine::float4(scale, 0.0f, scale, 0.0f) * 0.35f, planeColor[1],
				position + basisMat * Engine::float4(0.0f, 0.0f, scale, 0.0f) * 0.35f, planeColor[1]);

			AddTriangle(position, planeColor[2],
				position + basisMat * Engine::float4(0.0f, scale, 0.0f, 0.0f) * 0.35f, planeColor[2],
				position + basisMat * Engine::float4(scale, 0.0f, 0.0f, 0.0f) * 0.35f, planeColor[2]);
			AddTriangle(position + basisMat * Engine::float4(0.0f, scale, 0.0f, 0.0f) * 0.35f, planeColor[2],
				position + basisMat * Engine::float4(scale, scale, 0.0f, 0.0f) * 0.35f, planeColor[2],
				position + basisMat * Engine::float4(scale, 0.0f, 0.0f, 0.0f) * 0.35f, planeColor[2]);
		}

		inline void AddRotationAxis(const Engine::float4& position, const Engine::float4* basis, float scale, int axis, int segments = 32)
		{
			float total = Engine::Math::PI * 2.0f;
			float step = total / (float)segments;

			Engine::float4 color[3];
			color[0] = (axis == 0 ? Engine::float4(1.0f, 1.0f, 0.0f, 1.0f) : Engine::float4(1.0f, 0.0f, 0.0f, 1.0f));
			color[1] = (axis == 1 ? Engine::float4(1.0f, 1.0f, 0.0f, 1.0f) : Engine::float4(0.0f, 1.0f, 0.0f, 1.0f));
			color[2] = (axis == 2 ? Engine::float4(1.0f, 1.0f, 0.0f, 1.0f) : Engine::float4(0.0f, 0.0f, 1.0f, 1.0f));

			Engine::mat4 basisMat = Engine::mat4(basis[0], basis[1], basis[2], Engine::float4(0.0f, 0.0f, 0.0f, 1.0f));

			for (float angle = 0.0f; angle < total; angle += step)
			{
				AddLine(position + basisMat * Engine::float4(0.0f, cosf(angle), sinf(angle), 0.0f) * scale, color[0],
					position + basisMat * Engine::float4(0.0f, cosf(angle + step), sinf(angle + step), 0.0f) * scale, color[0]);

				AddLine(position + basisMat * Engine::float4(cosf(angle), 0.0f, sinf(angle), 0.0f) * scale, color[1],
					position + basisMat * Engine::float4(cosf(angle + step), 0.0f, sinf(angle + step), 0.0f) * scale, color[1]);

				AddLine(position + basisMat * Engine::float4(cosf(angle), sinf(angle), 0.0f, 0.0f) * scale, color[2],
					position + basisMat * Engine::float4(cosf(angle + step), sinf(angle + step), 0.0f, 0.0f) * scale, color[2]);
			}
		}

		inline void AddPlane(const Engine::Plane& plane, float scale)
		{
			Engine::float4 basis[3];
			basis[0] = Engine::normalize(Engine::float4(plane.eq.x, plane.eq.y, plane.eq.z, 0.0f));
			if (fabsf(plane.eq.x) < fabsf(plane.eq.y))
			{
				if (fabsf(plane.eq.x) < fabsf(plane.eq.z))
					basis[1] = Engine::float4(plane.eq.x, plane.eq.z, -plane.eq.y, 0.0f);
				else
					basis[1] = Engine::float4(plane.eq.y, -plane.eq.x, plane.eq.z, 0.0f);
			}
			else
			{
				if (fabsf(plane.eq.y) < fabsf(plane.eq.z))
					basis[1] = Engine::float4(plane.eq.z, plane.eq.y, -plane.eq.x, 0.0f);
				else
					basis[1] = Engine::float4(plane.eq.y, -plane.eq.x, plane.eq.z, 0.0f);
			}
			basis[1] = Engine::normalize(basis[1]);
			basis[2] = Engine::normalize(Engine::cross(basis[0], basis[1]));

			basis[0] = basis[0] * -plane.eq.w;
			
			AddLine(basis[0] - basis[1] * scale - basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), basis[0] + basis[1] * scale - basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));
			AddLine(basis[0] + basis[1] * scale - basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), basis[0] + basis[1] * scale + basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));
			AddLine(basis[0] + basis[1] * scale + basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), basis[0] - basis[1] * scale + basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));
			AddLine(basis[0] - basis[1] * scale + basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), basis[0] - basis[1] * scale - basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));
			AddLine(basis[0] - basis[1] * scale - basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), basis[0] + basis[1] * scale + basis[2] * scale, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		inline void AddSphere(const Engine::float4& center, float radius, const Engine::float4& color, int segments = 32)
		{
			float total = Engine::Math::PI * 2.0f;
			float step = total / (float)segments;

			for (float angle = 0.0f; angle < total; angle += step)
			{
				AddLine(center + Engine::float4(0.0f, cosf(angle), sinf(angle), 0.0f) * radius, color,
					center + Engine::float4(0.0f, cosf(angle + step), sinf(angle + step), 0.0f) * radius, color);

				AddLine(center + Engine::float4(cosf(angle), 0.0f, sinf(angle), 0.0f) * radius, color,
					center + Engine::float4(cosf(angle + step), 0.0f, sinf(angle + step), 0.0f) * radius, color);

				AddLine(center + Engine::float4(cosf(angle), sinf(angle), 0.0f, 0.0f) * radius, color,
					center + Engine::float4(cosf(angle + step), sinf(angle + step), 0.0f, 0.0f) * radius, color);
			}
		}

		inline void AddCollider(Engine::Entity* ent, Engine::CollisionComponent* col, Engine::RigidBodyComponent* rb)
		{
			Engine::Shape* shp = col->Get();
			Engine::mat4 mat;
			if (rb->Get()->GetBody() == nullptr)
			{
				mat = ent->GetWorldMatrix();
			}
			else
			{
				mat = rb->Get()->GetTransform();
			}

			switch (shp->GetType())
			{
			case Engine::Shape::Type::BOX:
				{
					Engine::Box* box = (Engine::Box*)shp;
					Engine::float4 maxP = box->GetExtents();
					maxP.w = 1.0f;
					Engine::float4 minP = -maxP;
					minP.w = 1.0f;

					Engine::float4 d = maxP - minP;
					Engine::float4 color = Engine::float4(0.0f, 1.0f, 0.0f, 1.0f);
					Engine::float4 v[8];
					v[0] = minP * mat;
					v[1] = (minP + Engine::float4(d.x, 0.0f, 0.0f, 0.0f)) * mat;
					v[2] = (minP + Engine::float4(0.0f, d.y, 0.0f, 0.0f)) * mat;
					v[3] = (minP + Engine::float4(0.0f, 0.0f, d.z, 0.0f)) * mat;
					v[4] = (minP + Engine::float4(d.x, d.y, 0.0f, 0.0f)) * mat;
					v[5] = (minP + Engine::float4(d.x, 0.0f, d.z, 0.0f)) * mat;
					v[6] = (minP + Engine::float4(0.0f, d.y, d.z, 0.0f)) * mat;
					v[7] = (minP + Engine::float4(d.x, d.y, d.z, 0.0f)) * mat;

					AddLine(v[0], color, v[1], color);
					AddLine(v[1], color, v[4], color);
					AddLine(v[4], color, v[2], color);
					AddLine(v[2], color, v[0], color);
					AddLine(v[3], color, v[5], color);
					AddLine(v[5], color, v[7], color);
					AddLine(v[7], color, v[6], color);
					AddLine(v[6], color, v[3], color);
					AddLine(v[0], color, v[3], color);
					AddLine(v[1], color, v[5], color);
					AddLine(v[4], color, v[7], color);
					AddLine(v[2], color, v[6], color);
				}
				break;

			case Engine::Shape::Type::SPHERE:
				{
					Engine::Sphere* sphere = (Engine::Sphere*)shp;
					float radius = sphere->GetRadius();

					const unsigned int segments = 8;
					float total = Engine::Math::PI * 2.0f;
					float step = total / (float)segments;

					Engine::float4 v[3][segments];

					Engine::float4 color = Engine::float4(0.0f, 1.0f, 0.0f, 1.0f);

					float angle = 0.0f;
					unsigned int i = 0;
					for (unsigned int i = 0; i < segments; i++)
					{
						v[0][i] = Engine::float4(0.0f, cosf(angle) * radius, sinf(angle) * radius, 1.0f) * mat;
						v[1][i] = Engine::float4(cosf(angle) * radius, 0.0f, sinf(angle) * radius, 1.0f) * mat;
						v[2][i] = Engine::float4(cosf(angle) * radius, sinf(angle) * radius, 0.0f, 1.0f) * mat;

						angle += step;
					}

					for (unsigned int i = 0; i < segments; i++)
					{
						AddLine(v[0][i], color, v[0][(i + 1) % segments], color);
						AddLine(v[1][i], color, v[1][(i + 1) % segments], color);
						AddLine(v[2][i], color, v[2][(i + 1) % segments], color);
					}
				}
				break;

			case Engine::Shape::Type::CYLINDER:
				{
					Engine::Cylinder* cylinder = (Engine::Cylinder*)shp;
					Engine::float4 extents = cylinder->GetHalfExtents();

					const unsigned int segments = 8;
					float total = Engine::Math::PI * 2.0f;
					float step = total / (float)segments;

					Engine::float4 v[2][segments];

					Engine::float4 color = Engine::float4(0.0f, 1.0f, 0.0f, 1.0f);

					float angle = 0.0f;
					unsigned int i = 0;
					for (unsigned int i = 0; i < segments; i++)
					{
						v[0][i] = Engine::float4(cosf(angle) * extents.x, -extents.y, sinf(angle) * extents.z, 1.0f) * mat;
						v[1][i] = Engine::float4(cosf(angle) * extents.x, extents.y, sinf(angle) * extents.z, 1.0f) * mat;

						angle += step;
					}

					for (unsigned int i = 0; i < segments; i++)
					{
						AddLine(v[0][i], color, v[0][(i + 1) % segments], color);
						AddLine(v[1][i], color, v[1][(i + 1) % segments], color);
						AddLine(v[0][i], color, v[1][i], color);
					}
				}
				break;

			case Engine::Shape::Type::CONE:
				{
					Engine::Cone* cone = (Engine::Cone*)shp;
					float radius = cone->GetRadius();
					float height = cone->GetHeight();

					const unsigned int segments = 8;
					float total = Engine::Math::PI * 2.0f;
					float step = total / (float)segments;

					Engine::float4 v[segments + 1];

					Engine::float4 color = Engine::float4(0.0f, 1.0f, 0.0f, 1.0f);

					float angle = 0.0f;
					unsigned int i = 0;
					for (unsigned int i = 0; i < segments; i++)
					{
						v[i] = Engine::float4(cosf(angle) * radius, -height * 0.5f, sinf(angle) * radius, 1.0f) * mat;
						angle += step;
					}
					v[segments] = Engine::float4(0.0f, height * 0.5f, 0.0f, 1.0f) * mat;

					for (unsigned int i = 0; i < segments; i++)
					{
						AddLine(v[i], color, v[(i + 1) % segments], color);
						AddLine(v[i], color, v[segments], color);
					}
				}
				break;

			case Engine::Shape::Type::CAPSULE:
				{
					Engine::Capsule* capsule = (Engine::Capsule*)shp;
					float radius = capsule->GetRadius();
					float height = capsule->GetHeight();

					const unsigned int segments = 8;
					float total = Engine::Math::PI * 2.0f;
					float step = total / (float)segments;

					Engine::float4 v[2][segments];
					Engine::float4 v1[4][segments / 2 + 1];

					Engine::float4 color = Engine::float4(0.0f, 1.0f, 0.0f, 1.0f);

					float angle = 0.0f;
					for (unsigned int i = 0; i < segments; i++)
					{
						v[0][i] = Engine::float4(cosf(angle) * radius, -height * 0.5f, sinf(angle) * radius, 1.0f) * mat;
						v[1][i] = Engine::float4(cosf(angle) * radius, height * 0.5f, sinf(angle) * radius, 1.0f) * mat;

						angle += step;
					}

					angle = 0.0f;
					for (unsigned int i = 0; i < segments / 2 + 1; i++)
					{
						v1[0][i] = Engine::float4(0.0f, -height * 0.5f - sinf(angle) * radius, cosf(angle) * radius, 1.0f) * mat;
						v1[1][i] = Engine::float4(0.0f, height * 0.5f + sinf(angle) * radius, cosf(angle) * radius, 1.0f) * mat;
						v1[2][i] = Engine::float4(cosf(angle) * radius, -height * 0.5f - sinf(angle) * radius, 0.0f, 1.0f) * mat;
						v1[3][i] = Engine::float4(cosf(angle) * radius, height * 0.5f + sinf(angle) * radius, 0.0f, 1.0f) * mat;
						angle += step;
					}

					for (unsigned int i = 0; i < segments; i++)
					{
						AddLine(v[0][i], color, v[0][(i + 1) % segments], color);
						AddLine(v[1][i], color, v[1][(i + 1) % segments], color);
					}

					for (unsigned int i = 0; i < segments / 2; i++)
					{
						AddLine(v1[0][i], color, v1[0][i + 1], color);
						AddLine(v1[1][i], color, v1[1][i + 1], color);
						AddLine(v1[2][i], color, v1[2][i + 1], color);
						AddLine(v1[3][i], color, v1[3][i + 1], color);
					}

					AddLine(v[0][0], color, v[1][0], color);
					AddLine(v[0][segments / 4], color, v[1][segments / 4], color);
					AddLine(v[0][segments / 2], color, v[1][segments / 2], color);
					AddLine(v[0][segments * 3 / 4], color, v[1][segments * 3 / 4], color);
				}
				break;

			case Engine::Shape::Type::TRIANGLE_MESH:
				if (ent->GameObject().Has<Engine::MeshComponent>())
				{
					Engine::Mesh* mesh = ent->GameObject().Get<Engine::MeshComponent>()->GetMesh();
					char* vbo = mesh->GetVertexBuffer()->Data();
					size_t vertSize = mesh->GetVertexBuffer()->ElementSize();
					unsigned int* ibo = (unsigned int*)mesh->GetIndexBuffer()->Data();

					Engine::float4 color = Engine::float4(0.0f, 1.0f, 0.0f, 1.0f);
					
					for (unsigned int i = 0; i < mesh->GetIndexBuffer()->NumElements(); i += 3)
					{
						size_t offset[3] = { ibo[i] * vertSize, ibo[i + 1] * vertSize, ibo[i + 2] * vertSize };
						float* v0 = (float*)(vbo + offset[0]);
						float* v1 = (float*)(vbo + offset[1]);
						float* v2 = (float*)(vbo + offset[2]);

						Engine::float4 vert0 = Engine::float4(*v0, *(v0 + 1), *(v0 + 2), 1.0f) * mat;
						Engine::float4 vert1 = Engine::float4(*v1, *(v1 + 1), *(v1 + 2), 1.0f) * mat;
						Engine::float4 vert2 = Engine::float4(*v2, *(v2 + 1), *(v2 + 2), 1.0f) * mat;

						AddLine(vert0, color, vert1, color);
						AddLine(vert1, color, vert2, color);
						AddLine(vert2, color, vert0, color);
					}
				}
				break;

			default:
				break;
			}
		}

		RenderGizmo(Engine::Log* log, Engine::D3DRenderer* renderer, unsigned int gizmoImageSize);
		~RenderGizmo();
		void Clear()
		{
			mLinesVBOUsage = 0;
			mTrianglesVBOUsage = 0;
			mIconsVBOUsage = 0;
		}
		//void Process(std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);
		void Process(std::vector<Engine::RenderNode>& nodes, Engine::Scene* scene, Engine::GpuMappedBuffer* matrices, unsigned int id);
		void Process(std::vector<Engine::RenderNode>& nodes, Engine::Scene* scene, Engine::GpuMappedBuffer* matrices, std::set<int>& ids);
		void ProcessIcons(std::vector<Engine::RenderNode>& nodes);
		void Render(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context);
		void RenderPicking(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context);
	};
}

#endif