/*
* File: mesh_manager.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/1/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef MESH_MANAGER_HPP
#define MESH_MANAGER_HPP
#include "i_asset_manager.hpp"
#include "i_renderer.hpp"
#include "mesh.hpp"

class MeshManager : public IAssetManagerT<Mesh> {
    struct Entry {
        std::unique_ptr<Mesh> cpu_mesh;
        int refs = 0;
    };

public:
    explicit MeshManager(IRenderer& r) : renderer(r) {};

    Handle loadQuad() {
        static const float quadVerts[] = { -0.5f,-0.5f,0,   0,0,
                                            0.5f,-0.5f,0,   1,0,
                                            0.5f, 0.5f,0,   1,1,
                                           -0.5f, 0.5f,0,   0,1 };
        static const uint32_t quadInds[] = { 0,1,2,  0,2,3 };

        // Generate UUID for the asset
        static UUID quadID = UUID::generate();
        auto& ent = cache[quadID];

        if(!ent.cpu_mesh) {
            ent.cpu_mesh = std::make_unique<Mesh>();
            ent.cpu_mesh->name = "Quad";
            ent.cpu_mesh->vertexCount = 4;
            ent.cpu_mesh->indexCount = 6;

            MeshDescriptor desc;
            desc.vertexData = quadVerts;
            desc.vertexSize = sizeof(quadVerts);
            desc.indexData = quadInds;
            desc.indexSize = sizeof(quadInds);
            desc.layout.attribs = {
                { VertexSemantic::Position, VertexDataType::Float32, 3, false, 0 },
                { VertexSemantic::TexCoord0, VertexDataType::Float32, 2, false, 12 }
            };
            desc.layout.stride = 5*sizeof(float);

            ent.cpu_mesh->rendererHandle = renderer.CreateMesh(desc);
        }

        ++ent.refs;
        return { quadID, ent.cpu_mesh.get() };
    }

    // Asset Manager Overrides
    Handle loadFromGUID(UUID id) override { return { id, cache[id].cpu_mesh.get() }; }
    Handle loadFromFile(const std::string &virtualPath) override { return {}; } // TODO

    void release(UUID id) override {
        if(auto it = cache.find( id ); it != cache.end()) {
            --it->second.refs;
        }
    }

    void CollectGarbage() override {
        for(auto it = cache.begin(); it != cache.end(); ) {
            if(it->second.refs == 0) {
                renderer.DestroyMesh(it->second.cpu_mesh->rendererHandle);
                it = cache.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void UnloadAll() override {
        for(auto& [id, ent] : cache) {
            if(ent.cpu_mesh && ent.cpu_mesh->rendererHandle) {
                renderer.DestroyMesh(ent.cpu_mesh->rendererHandle);
            }
        }
        cache.clear();
    }

private:
    std::unordered_map<UUID, Entry, UUID::Hasher> cache;
    IRenderer& renderer;
};

#endif //MESH_MANAGER_HPP
