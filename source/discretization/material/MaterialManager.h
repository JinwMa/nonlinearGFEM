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
        if (materials_.find(materialId) != materials_.end()) {
            throw std::runtime_error("Material with ID " + materialId + " already exists");
        }
        materials_[materialId] = material;
        materialIds_.push_back(materialId);
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
        auto it = materials_.find(materialId);
        if (it == materials_.end()) {
            throw std::runtime_error("Material not found: " + materialId);
        }
        return it->second;
    }

    // Check if material exists
    bool hasMaterial(const std::string& materialId) const {
        return materials_.find(materialId) != materials_.end();
    }

    // Get all material IDs
    const std::vector<std::string>& getAllMaterialIds() const {
        return materialIds_;
    }

    // Get number of materials
    size_t getNumMaterials() const {
        return materials_.size();
    }

    // Clear all materials (use with caution)
    void clear() {
        materials_.clear();
        materialIds_.clear();
    }

private:
    MaterialManager() = default;
    ~MaterialManager() = default;
    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;

    std::map<std::string, std::shared_ptr<BaseMaterial>> materials_;
    std::vector<std::string> materialIds_;  // Preserve insertion order
};

#endif // MATERIAL_MANAGER_H