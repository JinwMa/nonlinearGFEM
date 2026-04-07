#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <memory>
#include <string>
#include <map>
#include <vector>
#include "BaseMaterial.h"
#include "MaterialFactory.h"

class MaterialManager {
public:
    // Singleton pattern for global access
    static MaterialManager& getInstance() {
        static MaterialManager instance;
        return instance;
    }

    // Add material with specific ID
    void addMaterial(const std::string& materialId, std::shared_ptr<BaseMaterial> material) {
        if (materialId.empty()) {
            throw std::invalid_argument("Material ID cannot be empty");
        }
        if (!material) {
            throw std::invalid_argument("Material pointer is null");
        }
        if (d_materials.find(materialId) != d_materials.end()) {
            throw std::runtime_error("Material with ID " + materialId + " already exists");
        }
        d_materials[materialId] = material;
        d_materialIds.push_back(materialId);
    }

    // Create and add material from database
    std::shared_ptr<BaseMaterial> createAndAddMaterial(
        const std::string& materialId,
        std::shared_ptr<DataBase> db) {

        auto& factory = MaterialFactory::getInstance();
        auto material = factory.createMaterial(db);
        addMaterial(materialId, material);
        return material;
    }

    // Get material by ID
    std::shared_ptr<BaseMaterial> getMaterial(const std::string& materialId) const {
        auto it = d_materials.find(materialId);
        if (it == d_materials.end()) {
            throw std::runtime_error("Material not found: " + materialId);
        }
        return it->second;
    }

    // Check if material exists
    bool hasMaterial(const std::string& materialId) const {
        return d_materials.find(materialId) != d_materials.end();
    }

    // Get all material IDs
    const std::vector<std::string>& getAllMaterialIds() const {
        return d_materialIds;
    }

    // Get number of materials
    size_t getNumMaterials() const {
        return d_materials.size();
    }

    // Clear all materials (use with caution)
    void clear() {
        d_materials.clear();
        d_materialIds.clear();
    }

private:
    MaterialManager() = default;
    ~MaterialManager() = default;
    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;

    std::map<std::string, std::shared_ptr<BaseMaterial>> d_materials;
    std::vector<std::string> d_materialIds;  // Preserve insertion order
};

#endif // MATERIAL_MANAGER_H