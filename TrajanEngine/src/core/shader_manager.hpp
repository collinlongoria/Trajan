/*
* File: shader_manager.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/7/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP
#include <fstream>
#include <optional>

#include "i_asset_manager.hpp"
#include "i_renderer.hpp"
#include "shader.hpp"

class ShaderManager : public IAssetManagerT<Shader> {
    struct Entry {
        std::unique_ptr<Shader> shader;
        int refs = 0;
    };

public:
    explicit ShaderManager(IRenderer& r) : renderer(r) {};

    // TODO: consider implementing dedupe built in to manager
    Handle getOrCreate(const std::string& key, const ShaderDescriptor& desc) {
        auto it = byKey.find(key);
        if(it != byKey.end()) {
            return loadFromGUID(it->second);
        }

        // create new shader resource
        UUID id = UUID::generate();
        auto& ent = cache[id];
        ent.shader = std::make_unique<Shader>();
        ent.shader->name = key.empty() ? "Shader" : key;
        ent.shader->rendererHandle = renderer.CreateShader(desc);
        ent.refs = 1;

        byKey[key] = id;
        return Handle{ id, ent.shader.get(), this, false };
    }

    Handle loadFromSources(std::string vertSrc, std::string fragSrc, const std::string& key) {
        ShaderDescriptor desc {
            .vertexSource = std::move(vertSrc),
            .fragmentSource = std::move(fragSrc)
        };

        return getOrCreate(key, desc);
    }

    // Convention: given "filename" -> reads filename.vert.glsl & triangle.frag.glsl
    Handle loadFromFile(const std::string &basePath) override {
        auto vs = readSource(basePath + ".vert.glsl");
        auto fs = readSource(basePath + ".frag.glsl");
        if(!vs || !fs) return {};
        return loadFromSources(*vs, *fs, basePath);
    }

    Handle loadFromGUID(UUID id) override {
        auto it = cache.find(id);
        if(it == cache.end() || !it->second.shader) return {};
        ++it->second.refs;
        return Handle{ id, it->second.shader.get(), this, false };
    }

    void addRef(UUID id) override {
        auto it = cache.find(id);
        if(it != cache.end()) ++it->second.refs;
    }

    void release(UUID id) override {
        auto it = cache.find(id);
        if(it != cache.end()) --it->second.refs;
    }

    void CollectGarbage() override {
        for(auto it = cache.begin(); it != cache.end();) {
            if(it->second.refs <= 0) {
                if(it->second.shader && it->second.shader->rendererHandle) {
                    renderer.DestroyShader(it->second.shader->rendererHandle);
                }
                // Remove any name keys pointing to this UUID
                for(auto k = byKey.begin(); k != byKey.end();) {
                    if(k->second == it->first) k = byKey.erase(k);
                    else ++k;
                }
                it = cache.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void UnloadAll() override {
        for(auto& [id, ent] : cache) {
            if(ent.shader && ent.shader->rendererHandle) {
                renderer.DestroyShader(ent.shader->rendererHandle);
            }
        }
        cache.clear();
        byKey.clear();
    }

private:
    static std::optional<std::string> readSource(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if(!file) return std::nullopt;
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

private:
    std::unordered_map<UUID, Entry, UUID::Hasher> cache;
    std::unordered_map<std::string, UUID> byKey; // caller-provided key -> UUID
    IRenderer& renderer;
};

#endif //SHADER_MANAGER_HPP
