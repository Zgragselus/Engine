#ifndef __EDITOR__H__
#define __EDITOR__H__

#include "Main.h"
#include "Graphics/Graphics.h"
#include "Game/Game.h"
#include "ImguiSubsystem.h"
#include "RenderPassPicking.h"
#include "Core/Math/Primitive/Ray.h"

namespace SkyeCuillin
{
	/// Structure to hold all variables needed for editing
	struct Editor
	{
		// Edit modes
		enum class Mode
		{
			SELECTION = 0,
			TRANSLATE,
			ROTATE,
			SCALE
		};

		Mode mEditMode;

		// Snap mode
		//
		// When transforming object, do we want no snap at all, snap to whole units relatively or snap to grid
		enum class SnapMode
		{
			NONE,
			REL,
			GRID
		};

		SnapMode mSnapMode;
		float mSnapStepTranslation = 10.0f;
		float mSnapStepRotation = Engine::Math::DegToRad(5.0f);
		float mSnapStepScale = 0.1f;

		// Edit actions
		enum class Action
		{
			IDLE,
			SELECT
		};

		Action mAction;

		// Mouse position (stored when pushed), button and key states
		int mMousePos[2][2];
		bool mMousePressed;
		bool mShiftPressed;

		// Window size
		unsigned int mWidth;
		unsigned int mHeight;

		// Selection flags for axis/plane
		int mActiveAxis;		// Which axis is mouse hovered over (all)
		int mActivePlane;		// Which plane is mouse hovered over (only translation & scale)

		bool mTransforming;		// Are we currently transforming

		// Are we transforming in global or local space + axes
		bool mLocalSpace;

		// Drawn axis size
		Engine::float4* mAxis;
		float mAxisScale;
		float mSensitivity;

		// Mouse ray
		Engine::Ray* mMouseRay;
		Engine::Plane* mGizmoPlanes;
		Engine::float4* mTranslationPoints;

		// Scene
		Engine::Scene* mScene;

		// Camera
		Engine::Entity* mCamera;

		// Picking
		SkyeCuillin::RenderPassPicking* mPicking;

		bool mChange;			// Did change happened

		Editor()
		{
			mMouseRay = new Engine::Ray(Engine::float4(), Engine::float4());
			mGizmoPlanes = new Engine::Plane[3];
			mTranslationPoints = new Engine::float4[2];
			mEditMode = Mode::TRANSLATE;
			mAction = Action::IDLE;

			mMousePressed = false;
			mShiftPressed = false;
			mTransforming = false;
			mLocalSpace = false;
			mChange = true;

			mAxis = new Engine::float4[3];
			RefreshAxis();
			mAxisScale = 50.0f;
			mSensitivity = tanf(1.0f * 3.141592f / 180.0f);
			mActiveAxis = -1;
			mActivePlane = -1;

			/*mHoveringID = -1;
			mPrevClickedID = -1;
			mClickedID = -1;*/
		}

		~Editor()
		{
			delete[] mAxis;
			delete[] mGizmoPlanes;
			delete[] mTranslationPoints;
			delete mMouseRay;
		}

	private:
		// Drag & Drop of entities
		/*int mHoveringID;
		int mPrevClickedID;
		int mClickedID;*/

		void RefreshAxis()
		{
			if (mLocalSpace)
			{
				const Engine::mat4& m = mScene->GetGizmoMatrix();
				float* f = (float*)&m;
				mAxis[0] = Engine::normalize(Engine::float4(f[0], f[4], f[8], 0.0f));
				mAxis[1] = Engine::normalize(Engine::float4(f[1], f[5], f[9], 0.0f));
				mAxis[2] = Engine::normalize(Engine::float4(f[2], f[6], f[10], 0.0f));
			}
			else
			{
				mAxis[0] = Engine::normalize(Engine::float4(1.0f, 0.0f, 0.0f, 0.0f));
				mAxis[1] = Engine::normalize(Engine::float4(0.0f, 1.0f, 0.0f, 0.0f));
				mAxis[2] = Engine::normalize(Engine::float4(0.0f, 0.0f, 1.0f, 0.0f));
			}
		}

		void RefreshPlanes()
		{
			mGizmoPlanes[0] = Engine::Plane::CreateFromPoints(mScene->GetGizmoPosition(),
				mScene->GetGizmoPosition() + mAxis[1],
				mScene->GetGizmoPosition() + mAxis[2]);

			mGizmoPlanes[1] = Engine::Plane::CreateFromPoints(mScene->GetGizmoPosition(),
				mScene->GetGizmoPosition() + mAxis[0],
				mScene->GetGizmoPosition() + mAxis[2]);

			mGizmoPlanes[2] = Engine::Plane::CreateFromPoints(mScene->GetGizmoPosition(),
				mScene->GetGizmoPosition() + mAxis[0],
				mScene->GetGizmoPosition() + mAxis[1]);
		}

		void Select()
		{
			// Find bounds of selection
			int xmin = mMousePos[0][0] < mMousePos[1][0] ? mMousePos[0][0] : mMousePos[1][0];
			int ymin = mMousePos[0][1] < mMousePos[1][1] ? mMousePos[0][1] : mMousePos[1][1];
			int xmax = mMousePos[0][0] >= mMousePos[1][0] ? mMousePos[0][0] : mMousePos[1][0];
			int ymax = mMousePos[0][1] >= mMousePos[1][1] ? mMousePos[0][1] : mMousePos[1][1];

			// Find all unique entity IDs that are in selection
			std::set<unsigned int> selected;
			for (int j = ymin; j <= ymax; j++)
			{
				for (int i = xmin; i <= xmax; i++)
				{
					selected.insert(mPicking->GetValue(i, j));
				}
			}

			// If shift key is not pressed, always deselect current selection
			if (!mShiftPressed)
			{
				if (mScene->GetState()->GetSelection().size() > 0)
				{
					Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
					for (int i : mScene->GetState()->GetSelection())
					{
						cmd->AddArg<int>(i);
					}
					mScene->Apply(cmd);
				}

				// Select everything in selection set (except 0xFFFFFFFF - which denotes invalid selection
				Engine::Command* cmd = new Engine::Command(Engine::Command::Select);
				for (unsigned int i : selected)
				{
					if (i != (unsigned int)(-1))
					{
						cmd->AddArg<int>(i);
					}
				}

				if (cmd->GetArgsCount() > 0)
				{
					mScene->Apply(cmd);
				}
				else
				{
					delete cmd;
				}
			}
			else
			{
				// If there is just 1 item (+ shift press), and it is already in selection - deselect it
				if (selected.size() == 1 &&
					(*selected.begin() != (unsigned int)(-1)))
				{
					if (mScene->GetState()->GetSelection().find(*selected.begin()) != mScene->GetState()->GetSelection().end())
					{
						Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
						cmd->AddArg<int>(*selected.begin());
						mScene->Apply(cmd);
					}
					else
					{
						Engine::Command* cmd = new Engine::Command(Engine::Command::Select);
						cmd->AddArg<int>(*selected.begin());
						mScene->Apply(cmd);
					}
				}
				// Otherwise add all marked items into the selection set
				else
				{
					Engine::Command* cmd = new Engine::Command(Engine::Command::Select);
					for (unsigned int i : selected)
					{
						if (i != (unsigned int)(-1))
						{
							cmd->AddArg<int>(i);
						}
					}

					if (cmd->GetArgsCount() > 0)
					{
						mScene->Apply(cmd);
					}
					else
					{
						delete cmd;
					}
				}
			}

			RefreshAxis();
			RefreshPlanes();
		}

		void ScenegraphReassing(int newParentId)
		{
			// Drag & Drop changes parent of selected entities, we need:
			// - pair of selected entityID and their parentID before re-assignment
			// - separator (-1)
			// - pair of selected entityID and their new parentID after re-assignment
			Engine::Command* cmd = new Engine::Command(Engine::Command::Reassign);

			// Before pairs
			for (int id : mScene->GetState()->GetSelection())
			{
				Engine::Entity* e = mScene->GetEntity(id);
				int parent = e->GetParent()->mSceneID;

				cmd->AddArg<int>(id);
				cmd->AddArg<int>(parent);
			}

			// Separator
			cmd->AddArg<int>(-1);

			// After pairs
			for (int id : mScene->GetState()->GetSelection())
			{
				Engine::Entity* e = mScene->GetEntity(id);
				int parent = e->GetParent()->mSceneID;

				cmd->AddArg<int>(id);
				cmd->AddArg<int>(newParentId);
			}
			mScene->Apply(cmd);
			mChange = true;
		}

	public:
		void SetLocalSpace(bool localSpace)
		{
			if (mLocalSpace != localSpace)
			{
				mLocalSpace = localSpace;
				RefreshAxis();
				RefreshPlanes();
			}
		}

		void SetPickingPass(SkyeCuillin::RenderPassPicking* picking)
		{
			mPicking = picking;
		}

		void SetScene(Engine::Scene* scene)
		{
			mScene = scene;
		}

		void SetCamera(Engine::Entity* camera)
		{
			mCamera = camera;
		}

		void Resize(unsigned int w, unsigned int h)
		{
			mWidth = w;
			mHeight = h;
		}

		void Process()
		{
			if (mAction == Action::SELECT)
			{
				Select();
				mAction = Action::IDLE;
			}
		}

		void MousePress(int button, int x, int y)
		{
			switch (mEditMode)
			{
			case Mode::SELECTION:
				if (button == 0)
				{
					mMousePos[0][0] = x;
					mMousePos[0][1] = y;
				}
				break;

			case Mode::TRANSLATE:
			case Mode::SCALE:
				if (mActiveAxis < 0 && mActivePlane < 0)
				{
					mMousePos[0][0] = x;
					mMousePos[0][1] = y;
				}
				else
				{
					RefreshPlanes();

					if (mActiveAxis != -1)
					{
						float minDistance = 1000000.0f;
						for (unsigned int i = 1; i < 3; i++)
						{
							float distance;
							if (mMouseRay->TestPlane(mGizmoPlanes[(mActiveAxis + i) % 3], distance))
							{
								if (distance < minDistance)
								{
									minDistance = distance;
								}
							}
						}

						mTranslationPoints[0] = mMouseRay->origin + mMouseRay->direction * minDistance;
					}
					else
					{
						float minDistance = 1000000.0f;
						if (mMouseRay->TestPlane(mGizmoPlanes[mActivePlane], minDistance))
						{
							mTranslationPoints[0] = mMouseRay->origin + mMouseRay->direction * minDistance;
						}
					}

					mTransforming = true;
				}
				break;

			case Mode::ROTATE:
				if (mActiveAxis < 0)
				{
					mMousePos[0][0] = x;
					mMousePos[0][1] = y;
				}
				else
				{
					RefreshPlanes();

					float minDistance;

					mMouseRay->TestPlane(mGizmoPlanes[mActiveAxis], minDistance);

					mTranslationPoints[0] = mMouseRay->origin + mMouseRay->direction * minDistance;

					mTransforming = true;
				}
				break;
			}
		}

		void MouseRelease(int button, int x, int y)
		{
			switch (mEditMode)
			{
			case Mode::SELECTION:
				if (button == 0)
				{
					mMousePos[1][0] = x;
					mMousePos[1][1] = y;
					mAction = Action::SELECT;
				}
				else if (button == 1)
				{
					Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
					for (int i : mScene->GetState()->GetSelection())
					{
						cmd->AddArg<int>(i);
					}
					mScene->Apply(cmd);
					mAction = Action::IDLE;
				}
				break;

			case Mode::TRANSLATE:
			case Mode::SCALE:
				if (mActiveAxis < 0 && mActivePlane < 0)
				{
					if (button == 0)
					{
						mMousePos[1][0] = x;
						mMousePos[1][1] = y;
						mAction = Action::SELECT;
					}
					else if (button == 1)
					{
						Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
						for (int i : mScene->GetState()->GetSelection())
						{
							cmd->AddArg<int>(i);
						}
						mScene->Apply(cmd);
						mChange = true;
						mAction = Action::IDLE;
					}
				}
				else
				{
					if (mTransforming)
					{
						if (mActiveAxis != -1)
						{
							float minDistance = 1000000.0f;
							for (unsigned int i = 1; i < 3; i++)
							{
								float distance;
								if (mMouseRay->TestPlane(mGizmoPlanes[(mActiveAxis + i) % 3], distance))
								{
									if (distance < minDistance)
									{
										minDistance = distance;
									}
								}
							}
							mTranslationPoints[1] = mMouseRay->origin + mMouseRay->direction * minDistance;
						}
						else if (mActivePlane != -1)
						{
							float minDistance = 1000000.0f;
							if (mMouseRay->TestPlane(mGizmoPlanes[mActivePlane], minDistance))
							{
								mTranslationPoints[1] = mMouseRay->origin + mMouseRay->direction * minDistance;
							}
						}

						Engine::float4 delta = mTranslationPoints[1] - mTranslationPoints[0];

						if (mEditMode == Mode::TRANSLATE)
						{
							if (mActiveAxis == 0)
							{
								delta = Engine::dot(delta, mAxis[0]) * mAxis[0];
							}
							else if (mActiveAxis == 1)
							{
								delta = Engine::dot(delta, mAxis[1]) * mAxis[1];
							}
							else if (mActiveAxis == 2)
							{
								delta = Engine::dot(delta, mAxis[2]) * mAxis[2];
							}
							else if (mActivePlane != -1 && mLocalSpace)
							{
								delta = Engine::float4(Engine::dot(delta, mAxis[0]),
									Engine::dot(delta, mAxis[1]),
									Engine::dot(delta, mAxis[2]), 0.0f);

								Engine::mat4 axis = Engine::mat4(mAxis[0].x, mAxis[0].y, mAxis[0].z, 0.0f,
									mAxis[1].x, mAxis[1].y, mAxis[1].z, 0.0f,
									mAxis[2].x, mAxis[2].y, mAxis[2].z, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f);

								delta = axis * delta;
							}
							
							if (mSnapMode == SnapMode::REL)
							{
								delta.x = floorf(delta.x / mSnapStepTranslation) * mSnapStepTranslation;
								delta.y = floorf(delta.y / mSnapStepTranslation) * mSnapStepTranslation;
								delta.z = floorf(delta.z / mSnapStepTranslation) * mSnapStepTranslation;
							}
							else if (mSnapMode == SnapMode::GRID)
							{
								Engine::float4 baseOffset = mScene->GetGizmoPosition() - f4floor(mScene->GetGizmoPosition() / mSnapStepTranslation) * mSnapStepTranslation;

								baseOffset.x = mActiveAxis == 0 ? baseOffset.x : 0.0f;
								baseOffset.y = mActiveAxis == 1 ? baseOffset.y : 0.0f;
								baseOffset.z = mActiveAxis == 2 ? baseOffset.z : 0.0f;

								delta.x = floorf(delta.x / mSnapStepTranslation) * mSnapStepTranslation - baseOffset.x;
								delta.y = floorf(delta.y / mSnapStepTranslation) * mSnapStepTranslation - baseOffset.y;
								delta.z = floorf(delta.z / mSnapStepTranslation) * mSnapStepTranslation - baseOffset.z;
							}

							Engine::Command* cmd = new Engine::Command(Engine::Command::Translate);
							cmd->AddArg<float>(delta.x);
							cmd->AddArg<float>(delta.y);
							cmd->AddArg<float>(delta.z);
							mScene->Apply(cmd);
							mChange = true;
							RefreshAxis();
							RefreshPlanes();
						}
						else
						{
							if (mActiveAxis == 0)
							{
								delta = Engine::float4(Engine::dot(delta, mAxis[0]), 0.0f, 0.0f, 0.0f);
							}
							else if (mActiveAxis == 1)
							{
								delta = Engine::float4(0.0f, Engine::dot(delta, mAxis[1]), 0.0f, 0.0f);
							}
							else if (mActiveAxis == 2)
							{
								delta = Engine::float4(0.0f, 0.0f, Engine::dot(delta, mAxis[2]), 0.0f);
							}
							else if (mActivePlane != -1 && mLocalSpace)
							{
								delta = Engine::float4(Engine::dot(delta, mAxis[0]),
									Engine::dot(delta, mAxis[1]),
									Engine::dot(delta, mAxis[2]), 0.0f);
							}

							Engine::float4 baseDelta = mTranslationPoints[0] - mScene->GetGizmoPosition();
							if (mActiveAxis == 0)
							{
								baseDelta = Engine::float4(Engine::dot(baseDelta, mAxis[0]), 0.0f, 0.0f, 0.0f);
							}
							else if (mActiveAxis == 1)
							{
								baseDelta = Engine::float4(0.0f, Engine::dot(baseDelta, mAxis[1]), 0.0f, 0.0f);
							}
							else if (mActiveAxis == 2)
							{
								baseDelta = Engine::float4(0.0f, 0.0f, Engine::dot(baseDelta, mAxis[2]), 0.0f);
							}
							else if (mActivePlane != -1 && mLocalSpace)
							{
								baseDelta = Engine::float4(Engine::dot(baseDelta, mAxis[0]),
									Engine::dot(baseDelta, mAxis[1]),
									Engine::dot(baseDelta, mAxis[2]), 0.0f);
							}

							Engine::float4 scale;
							if (mSnapMode == SnapMode::REL || mSnapMode == SnapMode::GRID)
							{
								scale.x = 1.0f + floorf((fabsf(delta.x) < 0.00001f ? 0.0f : delta.x / baseDelta.x) / mSnapStepScale) * mSnapStepScale;
								scale.y = 1.0f + floorf((fabsf(delta.y) < 0.00001f ? 0.0f : delta.y / baseDelta.y) / mSnapStepScale) * mSnapStepScale;
								scale.z = 1.0f + floorf((fabsf(delta.z) < 0.00001f ? 0.0f : delta.z / baseDelta.z) / mSnapStepScale) * mSnapStepScale;
							}
							else
							{
								scale.x = 1.0f + (fabsf(delta.x) < 0.00001f ? 0.0f : delta.x / baseDelta.x);
								scale.y = 1.0f + (fabsf(delta.y) < 0.00001f ? 0.0f : delta.y / baseDelta.y);
								scale.z = 1.0f + (fabsf(delta.z) < 0.00001f ? 0.0f : delta.z / baseDelta.z);
							}
							scale.x = (fabsf(scale.x) < 0.00001f ? 0.00001f : scale.x);
							scale.y = (fabsf(scale.y) < 0.00001f ? 0.00001f : scale.y);
							scale.z = (fabsf(scale.z) < 0.00001f ? 0.00001f : scale.z);
							scale.w = 1.0f;

							Engine::Command* cmd = new Engine::Command(Engine::Command::Scale);
							cmd->AddArg<float>(scale.x);
							cmd->AddArg<float>(scale.y);
							cmd->AddArg<float>(scale.z);
							cmd->AddArg<float>(mScene->GetGizmoPosition().x);
							cmd->AddArg<float>(mScene->GetGizmoPosition().y);
							cmd->AddArg<float>(mScene->GetGizmoPosition().z);
							cmd->AddArg<float>(mAxis[0].x);
							cmd->AddArg<float>(mAxis[0].y);
							cmd->AddArg<float>(mAxis[0].z);
							cmd->AddArg<float>(mAxis[1].x);
							cmd->AddArg<float>(mAxis[1].y);
							cmd->AddArg<float>(mAxis[1].z);
							cmd->AddArg<float>(mAxis[2].x);
							cmd->AddArg<float>(mAxis[2].y);
							cmd->AddArg<float>(mAxis[2].z);
							mScene->Apply(cmd);
							mChange = true;
							RefreshAxis();
							RefreshPlanes();
						}

						mTransforming = false;

						mScene->SetSubstituteMatrix(Engine::mat4());
					}
				}
				break;

			case Mode::ROTATE:
				if (mActiveAxis < 0)
				{
					if (button == 0)
					{
						mMousePos[1][0] = x;
						mMousePos[1][1] = y;
						mAction = Action::SELECT;
					}
					else if (button == 1)
					{
						Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
						for (int i : mScene->GetState()->GetSelection())
						{
							cmd->AddArg<int>(i);
						}
						mScene->Apply(cmd);
						mChange = true;
						mAction = Action::IDLE;
					}
				}
				else
				{
					if (mTransforming)
					{
						float minDistance;

						mMouseRay->TestPlane(mGizmoPlanes[mActiveAxis], minDistance);

						mTranslationPoints[1] = mMouseRay->origin + mMouseRay->direction * minDistance;

						Engine::float4 center = mScene->GetGizmoPosition();

						Engine::float4 vbegin = mTranslationPoints[0] - center;
						vbegin = vbegin - dot(vbegin, mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f)) * mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f);
						vbegin.w = 0.0f;

						Engine::float4 vend = mTranslationPoints[1] - center;
						vend = vend - dot(vend, mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f)) * mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f);
						vend.w = 0.0f;

						Engine::quat rotation;

						if (mSnapMode == SnapMode::REL || mSnapMode == SnapMode::GRID)
						{
							float angle = floor(acos(dot(normalize(vbegin), normalize(vend))) / mSnapStepRotation) * mSnapStepRotation * 0.5f;
							Engine::float4 axis = normalize(cross(vbegin, vend));

							float cosAngle = cosf(angle);
							float sinAngle = sinf(angle);

							rotation = Engine::quat(axis.x * sinAngle, axis.y * sinAngle, axis.z * sinAngle, cosAngle);
						}
						else
						{
							rotation = Engine::quat(vbegin, vend);
						}

						Engine::Command* cmd = new Engine::Command(Engine::Command::Rotate);
						cmd->AddArg<float>(rotation.x);
						cmd->AddArg<float>(rotation.y);
						cmd->AddArg<float>(rotation.z);
						cmd->AddArg<float>(rotation.w);
						cmd->AddArg<float>(center.x);
						cmd->AddArg<float>(center.y);
						cmd->AddArg<float>(center.z);
						mScene->Apply(cmd);
						mChange = true;
						RefreshAxis();
						RefreshPlanes();

						mTransforming = false;

						mScene->SetSubstituteMatrix(Engine::mat4());
					}
				}
				break;
			}
		}

		void MouseMove(int x, int y)
		{
			switch (mEditMode)
			{
			case Mode::SELECTION:
				break;

			case Mode::TRANSLATE:
			case Mode::SCALE:
				mScene->GetRay(mCamera, mMouseRay->origin, mMouseRay->direction, (2.0f * x) / (float)mWidth - 1.0f, 1.0f - (2.0f * y) / (float)mHeight);

				if (!mTransforming)
				{
					bool hit[3] = { false, false, false };			// Did the ray hit one of the gizmo axis?
					float gizmoDistance[3];							// Closest distance to each gizmo axis
					float limit = tanf(0.5f * 3.141592f / 180.0f);	// Angle limit to determine axis picked
					float coord[2];

					// Test vs 3 gizmos
					hit[0] = mMouseRay->TestLine(mScene->GetGizmoPosition(), mScene->GetGizmoPosition() + mAxis[0] * mAxisScale, limit, gizmoDistance[0], coord[0], coord[1]);
					hit[1] = mMouseRay->TestLine(mScene->GetGizmoPosition(), mScene->GetGizmoPosition() + mAxis[1] * mAxisScale, limit, gizmoDistance[1], coord[0], coord[1]);
					hit[2] = mMouseRay->TestLine(mScene->GetGizmoPosition(), mScene->GetGizmoPosition() + mAxis[2] * mAxisScale, limit, gizmoDistance[2], coord[0], coord[1]);

					// Check which gizmo axis has closest distance to ray
					int minAxis = 0;
					if (gizmoDistance[0] < gizmoDistance[1])
					{
						if (gizmoDistance[0] < gizmoDistance[2])
						{
							minAxis = 0;
						}
						else
						{
							minAxis = 2;
						}
					}
					else
					{
						if (gizmoDistance[1] < gizmoDistance[2])
						{
							minAxis = 1;
						}
						else
						{
							minAxis = 2;
						}
					}

					// If there is any axis satisfying intersection criteria - we're mousing over it, otherwise no (standard selection)
					if (hit[minAxis])
					{
						mActiveAxis = minAxis;
					}
					else
					{
						mActiveAxis = -1;
					}

					mActivePlane = -1;

					// If we don't have any axis selected - attempt to select a plane
					if (mActiveAxis == -1)
					{
						for (int i = 0; i < 3; i++)
						{
							hit[i] = mMouseRay->TestPlane(mGizmoPlanes[i], gizmoDistance[i]);

							if (hit[i])
							{
								Engine::float4 hitPoint = mMouseRay->origin + mMouseRay->direction * gizmoDistance[i];
								Engine::float4 hitDelta = hitPoint - mScene->GetGizmoPosition();

								float coord[2];
								coord[0] = Engine::dot(hitDelta, mAxis[(i + 1) % 3]);
								coord[1] = Engine::dot(hitDelta, mAxis[(i + 2) % 3]);

								if ((coord[0] > 0.0f && coord[0] < 0.35f * mAxisScale) &&
									(coord[1] > 0.0f && coord[1] < 0.35f * mAxisScale))
								{
									mActivePlane = i;
								}
							}
						}
					}
				}
				else
				{
					if (mActiveAxis != -1)
					{
						float minDistance = 1000000.0f;
						for (unsigned int i = 1; i < 3; i++)
						{
							float distance;
							if (mMouseRay->TestPlane(mGizmoPlanes[(mActiveAxis + i) % 3], distance))
							{
								if (distance < minDistance)
								{
									minDistance = distance;
								}
							}
						}
						mTranslationPoints[1] = mMouseRay->origin + mMouseRay->direction * minDistance;
					}
					else if (mActivePlane != -1)
					{
						float minDistance = 1000000.0f;
						if (mMouseRay->TestPlane(mGizmoPlanes[mActivePlane], minDistance))
						{
							mTranslationPoints[1] = mMouseRay->origin + mMouseRay->direction * minDistance;
						}
					}

					Engine::float4 delta = mTranslationPoints[1] - mTranslationPoints[0];

					if (mEditMode == Mode::TRANSLATE)
					{
						if (mActiveAxis == 0)
						{
							delta = Engine::dot(delta, mAxis[0]) * mAxis[0];
						}
						else if (mActiveAxis == 1)
						{
							delta = Engine::dot(delta, mAxis[1]) * mAxis[1];
						}
						else if (mActiveAxis == 2)
						{
							delta = Engine::dot(delta, mAxis[2]) * mAxis[2];
						}
						else if (mActivePlane != -1 && mLocalSpace)
						{
							delta = Engine::float4(Engine::dot(delta, mAxis[0]),
								Engine::dot(delta, mAxis[1]),
								Engine::dot(delta, mAxis[2]), 0.0f);

							Engine::mat4 axis = Engine::mat4(mAxis[0].x, mAxis[0].y, mAxis[0].z, 0.0f,
								mAxis[1].x, mAxis[1].y, mAxis[1].z, 0.0f,
								mAxis[2].x, mAxis[2].y, mAxis[2].z, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f);

							delta = axis * delta;
						}

						if (mSnapMode == SnapMode::REL)
						{
							delta.x = floorf(delta.x / mSnapStepTranslation) * mSnapStepTranslation;
							delta.y = floorf(delta.y / mSnapStepTranslation) * mSnapStepTranslation;
							delta.z = floorf(delta.z / mSnapStepTranslation) * mSnapStepTranslation;
						}
						else if (mSnapMode == SnapMode::GRID)
						{
							Engine::float4 baseOffset = mScene->GetGizmoPosition() - f4floor(mScene->GetGizmoPosition() / mSnapStepTranslation) * mSnapStepTranslation;

							baseOffset.x = mActiveAxis == 0 ? baseOffset.x : 0.0f;
							baseOffset.y = mActiveAxis == 1 ? baseOffset.y : 0.0f;
							baseOffset.z = mActiveAxis == 2 ? baseOffset.z : 0.0f;

							delta.x = floorf(delta.x / mSnapStepTranslation) * mSnapStepTranslation - baseOffset.x;
							delta.y = floorf(delta.y / mSnapStepTranslation) * mSnapStepTranslation - baseOffset.y;
							delta.z = floorf(delta.z / mSnapStepTranslation) * mSnapStepTranslation - baseOffset.z;
						}

						mChange = true;
						mScene->SetSubstituteMatrix(Engine::mat4(1.0f, 0.0f, 0.0f, delta.x,
							0.0f, 1.0f, 0.0f, delta.y,
							0.0f, 0.0f, 1.0f, delta.z,
							0.0f, 0.0f, 0.0f, 1.0f));
					}
					else
					{
						if (mActiveAxis == 0)
						{
							delta = Engine::float4(Engine::dot(delta, mAxis[0]), 0.0f, 0.0f, 0.0f);
						}
						else if (mActiveAxis == 1)
						{
							delta = Engine::float4(0.0f, Engine::dot(delta, mAxis[1]), 0.0f, 0.0f);
						}
						else if (mActiveAxis == 2)
						{
							delta = Engine::float4(0.0f, 0.0f, Engine::dot(delta, mAxis[2]), 0.0f);
						}
						else if (mActivePlane != -1 && mLocalSpace)
						{
							delta = Engine::float4(Engine::dot(delta, mAxis[0]),
								Engine::dot(delta, mAxis[1]),
								Engine::dot(delta, mAxis[2]), 0.0f);
						}

						Engine::float4 baseDelta = mTranslationPoints[0] - mScene->GetGizmoPosition();
						if (mActiveAxis == 0)
						{
							baseDelta = Engine::float4(Engine::dot(baseDelta, mAxis[0]), 0.0f, 0.0f, 0.0f);
						}
						else if (mActiveAxis == 1)
						{
							baseDelta = Engine::float4(0.0f, Engine::dot(baseDelta, mAxis[1]), 0.0f, 0.0f);
						}
						else if (mActiveAxis == 2)
						{
							baseDelta = Engine::float4(0.0f, 0.0f, Engine::dot(baseDelta, mAxis[2]), 0.0f);
						}
						else if (mActivePlane != -1 && mLocalSpace)
						{
							baseDelta = Engine::float4(Engine::dot(baseDelta, mAxis[0]),
								Engine::dot(baseDelta, mAxis[1]),
								Engine::dot(baseDelta, mAxis[2]), 0.0f);
						}

						Engine::float4 scale;
						if (mSnapMode == SnapMode::REL || mSnapMode == SnapMode::GRID)
						{
							scale.x = 1.0f + floorf((fabsf(delta.x) < 0.00001f ? 0.0f : delta.x / baseDelta.x) / mSnapStepScale) * mSnapStepScale;
							scale.y = 1.0f + floorf((fabsf(delta.y) < 0.00001f ? 0.0f : delta.y / baseDelta.y) / mSnapStepScale) * mSnapStepScale;
							scale.z = 1.0f + floorf((fabsf(delta.z) < 0.00001f ? 0.0f : delta.z / baseDelta.z) / mSnapStepScale) * mSnapStepScale;
						}
						else
						{
							scale.x = 1.0f + (fabsf(delta.x) < 0.00001f ? 0.0f : delta.x / baseDelta.x);
							scale.y = 1.0f + (fabsf(delta.y) < 0.00001f ? 0.0f : delta.y / baseDelta.y);
							scale.z = 1.0f + (fabsf(delta.z) < 0.00001f ? 0.0f : delta.z / baseDelta.z);
						}
						scale.x = (fabsf(scale.x) < 0.00001f ? 0.00001f : scale.x);
						scale.y = (fabsf(scale.y) < 0.00001f ? 0.00001f : scale.y);
						scale.z = (fabsf(scale.z) < 0.00001f ? 0.00001f : scale.z);
						scale.w = 1.0f;

						Engine::mat4 axis = Engine::mat4(mAxis[0].x, mAxis[1].x, mAxis[2].x, 0.0f,
							mAxis[0].y, mAxis[1].y, mAxis[2].y, 0.0f,
							mAxis[0].z, mAxis[1].z, mAxis[2].z, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

						Engine::mat4 axisInverse = Engine::mat4(mAxis[0].x, mAxis[0].y, mAxis[0].z, 0.0f,
							mAxis[1].x, mAxis[1].y, mAxis[1].z, 0.0f,
							mAxis[2].x, mAxis[2].y, mAxis[2].z, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

						Engine::mat4 scaleMat = axis * Engine::mat4(scale.x, 0.0f, 0.0f, 0.0f,
							0.0f, scale.y, 0.0f, 0.0f,
							0.0f, 0.0f, scale.z, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f) * axisInverse;

						mChange = true;
						mScene->SetSubstituteMatrix(Engine::mat4(mScene->GetGizmoPosition()) * scaleMat * Engine::mat4(-mScene->GetGizmoPosition()));
					}
				}
				break;

			case Mode::ROTATE:
				// Obtain camera ray
				mScene->GetRay(mCamera, mMouseRay->origin, mMouseRay->direction, (2.0f * x) / (float)mWidth - 1.0f, 1.0f - (2.0f * y) / (float)mHeight);

				if (!mTransforming)
				{
					// Calculate gizmo planes
					RefreshPlanes();

					// Test ray vs 3 planes and find intersection positions
					bool hit[3] = { false, false, false };			// Did the ray hit one of the gizmo planes
					float gizmoDistance[3];							// Distance to intersection with plane
					float limit = tanf(0.5f * 3.141592f / 180.0f);	// Angle limit to determine axis picked

					hit[0] = mMouseRay->TestPlane(mGizmoPlanes[0], gizmoDistance[0]);
					hit[1] = mMouseRay->TestPlane(mGizmoPlanes[1], gizmoDistance[1]);
					hit[2] = mMouseRay->TestPlane(mGizmoPlanes[2], gizmoDistance[2]);

					// Find closest match
					int closestAxis = -1;
					float closestDistance = 1000000.0f;

					for (int i = 0; i < 3; i++)
					{
						if (hit[i])
						{
							Engine::float4 pt = mMouseRay->origin + mMouseRay->direction * gizmoDistance[i];
							float d = fabsf(50.0f - Engine::length(pt - mScene->GetGizmoPosition()));

							if (d < closestDistance)
							{
								closestAxis = i;
								closestDistance = d;
							}
						}
					}

					if (closestAxis != -1 && (closestDistance / gizmoDistance[closestAxis] < limit))
					{
						mActiveAxis = closestAxis;
					}
					else
					{
						mActiveAxis = -1;
					}
				}
				else
				{
					float minDistance;

					mMouseRay->TestPlane(mGizmoPlanes[mActiveAxis], minDistance);

					mTranslationPoints[1] = mMouseRay->origin + mMouseRay->direction * minDistance;

					Engine::float4 center = mScene->GetGizmoPosition();

					Engine::float4 vbegin = mTranslationPoints[0] - center;
					vbegin = vbegin - dot(vbegin, mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f)) * mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f);
					vbegin.w = 0.0f;

					Engine::float4 vend = mTranslationPoints[1] - center;
					vend = vend - dot(vend, mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f)) * mGizmoPlanes[mActiveAxis].eq * Engine::float4(1.0f, 1.0f, 1.0f, 0.0f);
					vend.w = 0.0f;

					if (mSnapMode == SnapMode::REL || mSnapMode == SnapMode::GRID)
					{
						float angle = floor(acos(dot(normalize(vbegin), normalize(vend))) / mSnapStepRotation) * mSnapStepRotation * 0.5f;
						Engine::float4 axis = normalize(cross(vbegin, vend));

						float cosAngle = cosf(angle);
						float sinAngle = sinf(angle);

						Engine::quat rotation = Engine::quat(axis.x * sinAngle, axis.y * sinAngle, axis.z * sinAngle, cosAngle);

						mChange = true;
						mScene->SetSubstituteMatrix(Engine::mat4(center) * Engine::mat4(rotation) * Engine::mat4(-center));
					}
					else
					{
						Engine::quat rotation = Engine::quat(vbegin, vend);

						mChange = true;
						mScene->SetSubstituteMatrix(Engine::mat4(center) * Engine::mat4(rotation) * Engine::mat4(-center));
					}
				}
				break;
			}
		}

		void KeyPress(Engine::Keyboard::KeyChar c)
		{
			if (c.mKey == Engine::Keyboard::KEY_SHIFT_L ||
				c.mKey == 16)
			{
				mShiftPressed = true;
			}
		}

		void KeyRelease(Engine::Keyboard::KeyChar c)
		{
			if (c.mKey == Engine::Keyboard::KEY_SHIFT_L ||
				c.mKey == 16)
			{
				mShiftPressed = false;
			}
		}

		/*void TreeViewPress(int button)
		{

		}

		void TreeViewRelease(int button)
		{
			if (button == 0)
			{
				mClickedID = -1;
			}
		}*/

		void ImguiSelectionEditor()
		{
			if (mScene->GetState()->GetSelection().size() == 1)
			{
				int id = *mScene->GetState()->GetSelection().begin();

				char buffer[256] = { 0 };
				sprintf_s(buffer, "%s", mScene->GetName(id).c_str());
				if (ImGui::InputText("Name", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (mScene->GetEntity(buffer) == nullptr)
					{
						Engine::Command* cmd = new Engine::Command(Engine::Command::Rename);
						cmd->AddArg<std::string>(mScene->GetName(id));
						cmd->AddArg<std::string>(std::string(buffer));
						mScene->Apply(cmd);
					}
					else
					{
						printf("ERROR: Can't rename!");
					}
				}

				Engine::Entity* ent = mScene->GetEntity(id);
				Engine::Transformation& t = ent->Transformation();

				if (ImGui::TreeNode("Transformation"))
				{
					Engine::float4 position = t.GetTranslation();
					if (ImGui::InputFloat3("Position", (float*)&position, -1, 0/*ImGuiInputTextFlags_EnterReturnsTrue*/))
					{
						Engine::float4 delta = position - t.GetTranslation();

						if (length(delta) > 0.0f)
						{
							Engine::Command* cmd = new Engine::Command(Engine::Command::Translate);
							cmd->AddArg<float>(delta.x);
							cmd->AddArg<float>(delta.y);
							cmd->AddArg<float>(delta.z);
							mScene->Apply(cmd);
							mChange = true;

							RefreshAxis();
							RefreshPlanes();

							t.Invalidate();
						}
					}

					Engine::quat rotation = t.GetRotation();
					float angles[3];
					Engine::to_euler(rotation, angles[0], angles[1], angles[2]);
					if (ImGui::InputFloat3("Rotation", (float*)&(angles[0]), -1, 0/*ImGuiInputTextFlags_EnterReturnsTrue*/))
					{
						Engine::quat delta = rotation.inverse() * Engine::from_euler(angles[0], angles[1], angles[2]);

						Engine::Command* cmd = new Engine::Command(Engine::Command::Rotate);
						cmd->AddArg<float>(delta.x);
						cmd->AddArg<float>(delta.y);
						cmd->AddArg<float>(delta.z);
						cmd->AddArg<float>(delta.w);
						cmd->AddArg<float>(mScene->GetGizmoPosition().x);
						cmd->AddArg<float>(mScene->GetGizmoPosition().y);
						cmd->AddArg<float>(mScene->GetGizmoPosition().z);
						mScene->Apply(cmd);
						mChange = true;

						RefreshAxis();
						RefreshPlanes();

						t.Invalidate();
					}

					Engine::float4 scale = t.GetScale();
					if (ImGui::InputFloat3("Scale", (float*)&scale, -1, 0/*ImGuiInputTextFlags_EnterReturnsTrue*/))
					{
						Engine::float4 delta = scale / t.GetScale();

						if ((delta.x > 1.000001f || delta.x < 0.999999f) ||
							(delta.y > 1.000001f || delta.y < 0.999999f) ||
							(delta.z > 1.000001f || delta.z < 0.999999f))
						{
							Engine::float4 scaleFactor;
							scaleFactor.x = fabsf(delta.x) < 0.00001f ? 1.0f : delta.x;
							scaleFactor.y = fabsf(delta.y) < 0.00001f ? 1.0f : delta.y;
							scaleFactor.z = fabsf(delta.z) < 0.00001f ? 1.0f : delta.z;
							scaleFactor.x = (fabsf(scaleFactor.x) < 0.00001f ? 0.00001f : scaleFactor.x);
							scaleFactor.y = (fabsf(scaleFactor.y) < 0.00001f ? 0.00001f : scaleFactor.y);
							scaleFactor.z = (fabsf(scaleFactor.z) < 0.00001f ? 0.00001f : scaleFactor.z);
							scaleFactor.w = 1.0f;

							Engine::Command* cmd = new Engine::Command(Engine::Command::Scale);
							cmd->AddArg<float>(scaleFactor.x);
							cmd->AddArg<float>(scaleFactor.y);
							cmd->AddArg<float>(scaleFactor.z);
							cmd->AddArg<float>(mScene->GetGizmoPosition().x);
							cmd->AddArg<float>(mScene->GetGizmoPosition().y);
							cmd->AddArg<float>(mScene->GetGizmoPosition().z);
							cmd->AddArg<float>(1.0f);
							cmd->AddArg<float>(0.0f);
							cmd->AddArg<float>(0.0f);
							cmd->AddArg<float>(0.0f);
							cmd->AddArg<float>(1.0f);
							cmd->AddArg<float>(0.0f);
							cmd->AddArg<float>(0.0f);
							cmd->AddArg<float>(0.0f);
							cmd->AddArg<float>(1.0f);
							mScene->Apply(cmd);
							mChange = true;

							RefreshAxis();
							RefreshPlanes();

							t.Invalidate();
						}
					}
					ImGui::TreePop();
				}

				for (auto it = ent->GameObject().ComponentsItBegin(); it != ent->GameObject().ComponentsItEnd(); it++)
				{
					//if (ImGui::TreeNode((it->second)->GetName().c_str()))
					if (ImGui::TreeNode(Engine::ComponentTypeId::GetName(it->first).c_str()))
					{
						std::string prev, next;
						if (it->second->Editor(prev, next))
						{
							Engine::Command* cmd = new Engine::Command(Engine::Command::Serialize);
							cmd->AddArg<std::string>(next);
							cmd->AddArg<std::string>(prev);
							mScene->ApplySerialize(cmd);
							mChange = true;
						}

						ImGui::TreePop();
					}

					if (ImGui::Button((std::string("X##(") + Engine::ComponentTypeId::GetName(it->first).c_str() + ")").c_str()))
					{
						//ent->GameObject().RemoveComponent(it->first);

						Engine::Command* cmd = new Engine::Command(Engine::Command::RemoveComponent);
						cmd->AddArg<int>(it->first);
						cmd->AddArg<std::string>("");
						mScene->Apply(cmd);
						mChange = true;
						break;
					}
				}

				if (ImGui::Button("Add Component"))
				{
					ImGui::OpenPopup("AddComponent");
				}

				if (ImGui::BeginPopup("AddComponent"))
				{
					for (unsigned int i = 0; i < Engine::ComponentTypeId::Count(); i++)
					{
						if (!ent->GameObject().HasComponent(i))
						{
							if (ImGui::Button(Engine::ComponentTypeId::GetName(i).c_str()))
							{
								/*Engine::Component* c = Engine::ComponentFactory::CreateComponent(i);
								ent->GameObject().AddComponent(i, c);*/
								
								Engine::Command* cmd = new Engine::Command(Engine::Command::AddComponent);
								cmd->AddArg<int>(i);
								cmd->AddArg<std::string>("");
								mScene->Apply(cmd);
								mChange = true;

								ImGui::CloseCurrentPopup();
							}
						}
					}

					ImGui::EndPopup();
				}
			}
		}

		void _ImguiScenegraphEditor(std::pair<int, Engine::Entity*>& node)
		{
			for (auto& child : node.second->Children())
			{
				std::pair<int, Engine::Entity*> node = std::pair<int, Engine::Entity*>(child->mSceneID, child);

				bool nodeOpen = ImGui::TreeNodeEx(mScene->GetName(node.first).c_str(),
					(mScene->GetState()->GetSelection().find(node.first) == mScene->GetState()->GetSelection().end() ? 0 : ImGuiTreeNodeFlags_Selected) |
					(child->Children().size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0) |
					ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen);
				
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("SCENEGRAPH_NODE", &node, sizeof(std::pair<int, Engine::Entity*>));
					ImGui::Text(mScene->GetName(node.first).c_str());
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENEGRAPH_NODE"))
					{
						ScenegraphReassing(node.first);
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGui::IsItemClicked())
				{
					if (!mShiftPressed)
					{
						if (mScene->GetState()->GetSelection().size() > 0)
						{
							Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
							for (int i : mScene->GetState()->GetSelection())
							{
								cmd->AddArg<int>(i);
							}
							mScene->Apply(cmd);
						}

						Engine::Command* cmd = new Engine::Command(Engine::Command::Select);
						cmd->AddArg<int>(node.first);
						mScene->Apply(cmd);
					}
					else
					{
						if (mScene->GetState()->GetSelection().find(node.first) == mScene->GetState()->GetSelection().end())
						{
							Engine::Command* cmd = new Engine::Command(Engine::Command::Select);
							cmd->AddArg<int>(node.first);
							mScene->Apply(cmd);
						}
						else
						{
							Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
							cmd->AddArg<int>(node.first);
							mScene->Apply(cmd);
						}
					}
				}

				if (nodeOpen)
				{
					_ImguiScenegraphEditor(node);
					ImGui::TreePop();
				}
			}
		}

		void ImguiScenegraphEditor()
		{
			ImGuiIO& io = ImGui::GetIO();

			std::pair<int, Engine::Entity*> node = std::pair<int, Engine::Entity*>(mScene->GetScenegraph()->mSceneID, mScene->GetScenegraph());

			ImGui::SetNextTreeNodeOpen(true);
			if (ImGui::TreeNodeEx(mScene->GetName(node.first).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENEGRAPH_NODE"))
					{
						ScenegraphReassing(node.first);
					}
					ImGui::EndDragDropTarget();
				}

				_ImguiScenegraphEditor(node);
				ImGui::TreePop();
			}
		}

		template <typename T>
		void ImguiManagerList(const std::string& name, Engine::Manager<T>* manager)
		{
			static char filter[256] = { 0 };
			static std::string selected;
			
			if (ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
			{
				ImGui::InputText("Filter", filter, 256, ImGuiInputTextFlags_EnterReturnsTrue);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				if (ImGui::BeginChild("RecordsBorder", ImVec2(0, -32), true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
				{
					//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
					if (ImGui::BeginChild("Records", ImVec2(0, -32), false))
					{
						if (ImGui::TreeNodeEx("Select:", ImGuiTreeNodeFlags_DefaultOpen))
						{
							std::vector<std::string> records;
							manager->FilterKeys(filter, records);

							for (const std::string& r : records)
							{
								if (typeid(T) == typeid(Engine::Texture))
								{
									ImGui::Image((ImTextureID)(manager->GetNode(r)->Get()->GetSRV().mGpuHandle.ptr), ImVec2(32, 32));

									if (ImGui::IsItemClicked())
									{
										selected = r;
									}

									ImGui::SameLine();
								}

								if (ImGui::TreeNodeEx(r.c_str(), ImGuiTreeNodeFlags_Leaf | (r == selected ? ImGuiTreeNodeFlags_Selected : 0)))
								{
									if (ImGui::IsItemClicked())
									{
										selected = r;
									}

									ImGui::TreePop();
								}
							}

							ImGui::TreePop();
						}

						ImGui::EndChild();
					}
					//ImGui::PopStyleVar();

					ImGui::EndChild();
				}
				ImGui::PopStyleVar();

				if (ImGui::Button("Cancel"))
				{
					Engine::ComponentStatic::mEditedComponent = nullptr;
				}

				ImGui::SameLine();
				if (ImGui::Button("OK"))
				{
					if (selected.length() > 0)
					{
						switch (Engine::ComponentStatic::mEditedTexture)
						{
						case 0:
							Engine::ComponentStatic::mEditedDiffuse = selected;
							break;

						case 1:
							Engine::ComponentStatic::mEditedNormals = selected;
							break;

						case 2:
							Engine::ComponentStatic::mEditedHeight = selected;
							break;

						case 3:
							Engine::ComponentStatic::mEditedRoughness = selected;
							break;

						case 4:
							Engine::ComponentStatic::mEditedMetallic = selected;
							break;

						default:
							break;
						}
					}

					Engine::ComponentStatic::mEditedComponent = nullptr;
				}
			}
			ImGui::End();
		}
	};
}

#endif 