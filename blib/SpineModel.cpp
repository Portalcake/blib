#include "SpineModel.h"

#include <spine/spine.h>
#include <spine/extension.h>
#include <glm/gtc/matrix_transform.hpp>

#include <blib/util/Log.h>
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/util/FileSystem.h>
#include <blib/gl/Vertex.h>
#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>
#include <blib/linq.h>

using blib::util::Log;

#define SPINE_MESH_VERTEX_COUNT_MAX 1000

void _spAtlasPage_createTexture(spAtlasPage* self, const char* path) {
	blib::Texture* texture = blib::ResourceManager::getInstance().getResource<blib::Texture>(path);
	self->rendererObject = texture;
	self->width = texture->originalWidth;
	self->height = texture->originalHeight;
}

void _spAtlasPage_disposeTexture(spAtlasPage* self) {
	blib::ResourceManager::getInstance().dispose((blib::Texture*)self->rendererObject);
}

char* _spUtil_readFile(const char* path, int* length) {
	char *data;
	*length = blib::util::FileSystem::getData(path, data);
	return data;
}


namespace blib
{


	SpineModel::SpineModel(const std::string &atlasFile, const std::string &skeletonFile)
	{
		atlas = spAtlas_createFromFile(atlasFile.c_str(), 0);
		json = spSkeletonJson_create(atlas);
		json->scale = 0.5f;

		skeletonData = spSkeletonJson_readSkeletonDataFile(json, skeletonFile.c_str());
		if (!skeletonData) {
			Log::out << json->error << Log::newline;
			return;
		}
		stateData = spAnimationStateData_create(skeletonData);
	}

	SpineModel::~SpineModel()
	{
		blib::linq::deleteall(instances);
	}


	SpineModelInstance* SpineModel::getInstance()
	{
		SpineModelInstance* instance = new SpineModelInstance();
		instances.push_back(instance);

		instance->skeleton = spSkeleton_create(skeletonData);
		instance->skeleton->flipY = true;
		spSkeleton_setToSetupPose(instance->skeleton);
		instance->state = spAnimationState_create(stateData);
		return instance;
	}

	void SpineModel::disposeInstance(SpineModelInstance* instance)
	{
		delete instance;
		for (size_t i = 0; i < instances.size(); i++)
			if (instances[i] == instance)
				instances.erase(instances.begin() + i);
	}



	void SpineModelInstance::playAnimation(const std::string &name, bool loop)
	{
		spAnimationState_setAnimationByName(state, 0, name.c_str(), loop);
	}

	void SpineModelInstance::playAnimation(const std::string &name, const std::function<void()> &callback)
	{
		spAnimationState_setAnimationByName(state, 0, name.c_str(), false);
		this->callback = callback;
	}

	void SpineModelInstance::stopAnimation(const std::string &name)
	{
		spAnimationState_clearTrack(state, 0);
	}

	bool SpineModelInstance::isPlaying()
	{
		return spAnimationState_getCurrent(state, 0) != NULL;
	}

	void SpineModelInstance::update(double elapsedTime)
	{
		bool wasPlaying = isPlaying();
		spSkeleton_update(skeleton, (float)elapsedTime);
		spAnimationState_update(state, (float)elapsedTime * 1);
		spAnimationState_apply(state, skeleton);
		spSkeleton_updateWorldTransform(skeleton);
		if (!isPlaying() && wasPlaying && callback)
			callback();


	}



	void SpineModelInstance::draw(const glm::mat4& transform, SpriteBatch& spriteBatch)
	{
		static float worldVertices[SPINE_MESH_VERTEX_COUNT_MAX];
		std::pair<glm::vec2, glm::vec2> vertices[4];

		for (int i = 0; i < skeleton->slotsCount; ++i)
		{
			spSlot* slot = skeleton->drawOrder[i];
			spAttachment* attachment = slot->attachment;
			if (!attachment) continue;


			/*			switch (slot->data->blendMode) {
			case BLEND_MODE_ADDITIVE:
			blend = BlendAdd;
			break;
			case BLEND_MODE_MULTIPLY:
			blend = BlendMultiply;
			break;
			case BLEND_MODE_SCREEN: // Unsupported, fall through.
			default:
			blend = BlendAlpha;
			}
			if (states.blendMode != blend) {
			target.draw(*vertexArray, states);
			vertexArray->clear();
			states.blendMode = blend;
			}*/

			Texture* texture = 0;



			std::vector<std::pair<glm::vec2, glm::vec2>> verts;
			glm::vec4 color;

			if (attachment->type == SP_ATTACHMENT_REGION) {
				spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;
				texture = (Texture*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
				spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVertices);
				color = glm::vec4(skeleton->r * slot->r, skeleton->g * slot->g, skeleton->b * slot->b, skeleton->a * slot->a);

				static const int indices[] = {
					SP_VERTEX_X1, SP_VERTEX_Y1,
					SP_VERTEX_X2, SP_VERTEX_Y2,
					SP_VERTEX_X3, SP_VERTEX_Y3,
					SP_VERTEX_X4, SP_VERTEX_Y4
				};

				static const int vertIndices[] = { 0, 1, 2, 0, 2, 3 };

				for (int ii = 0; ii < 6; ii++)
				{
					verts.push_back(std::pair<glm::vec2, glm::vec2>(
						glm::vec2(worldVertices[indices[2 * vertIndices[ii] + 0]], worldVertices[indices[2 * vertIndices[ii] + 1]]),
						glm::vec2(regionAttachment->uvs[indices[2 * vertIndices[ii] + 0]], regionAttachment->uvs[indices[2 * vertIndices[ii] + 1]])));
				}
			}
			else if (attachment->type == SP_ATTACHMENT_MESH) {
				spMeshAttachment* mesh = (spMeshAttachment*)attachment;
				if (mesh->verticesCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
				texture = (Texture*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;
				spMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

				color = glm::vec4(skeleton->r * slot->r, skeleton->g * slot->g, skeleton->b * slot->b, skeleton->a * slot->a);
				for (int i = 0; i < mesh->trianglesCount; ++i) {
					int index = mesh->triangles[i] << 1;
					verts.push_back(std::pair<glm::vec2, glm::vec2>(glm::vec2(worldVertices[index], worldVertices[index + 1]), glm::vec2(mesh->uvs[index], mesh->uvs[index + 1])));
				}

			}
			else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH) {
				spSkinnedMeshAttachment* mesh = (spSkinnedMeshAttachment*)attachment;
				if (mesh->uvsCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
				texture = (Texture*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;
				spSkinnedMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

				color = glm::vec4(skeleton->r * slot->r, skeleton->g * slot->g, skeleton->b * slot->b, skeleton->a * slot->a);

				for (int i = 0; i < mesh->trianglesCount; ++i) {
					int index = mesh->triangles[i] << 1;
					verts.push_back(std::pair<glm::vec2, glm::vec2>(glm::vec2(worldVertices[index], worldVertices[index + 1]), glm::vec2(mesh->uvs[index], mesh->uvs[index + 1])));
				}
			}

			spriteBatch.draw(texture, transform, verts, color);
		}
	}
}