#ifndef MATERIAL_FACTORY_H
#define MATERIAL_FACTORY_H

#include <memory>
#include <string>
#include <map>
#include <functional>
#include "BaseMaterial.h"
#include "LinearElastic.h"

// Forward declaration
class DataBase;

class MaterialFactory {
public:
    // Singleton pattern for global access
    static MaterialFactory& getInstance() {
        static MaterialFactory instance;
        return instance;
    }

    // Register a material creator function
    using MaterialCreator = std::function<std::shared_ptr<BaseMaterial>(
        const std::string& name,
        std::shared_ptr<DataBase> db)>;

    // Register a material type
    void registerMaterialType(const std::string& typeName, MaterialCreator creator) {
        d_creators[typeName] = creator;
    }

    // Create material from database
    // Database should contain: material_type, name, and material parameters
    std::shared_ptr<BaseMaterial> createMaterial(std::shared_ptr<DataBase> db) {
        if (!db) {
            throw std::invalid_argument("Database pointer is null");
        }

        // Get material type
        std::string materialType;
        if (db->ifExist("material_type")) {
            materialType = db->getString("material_type");
        } else if (db->ifExist("type")) {
            materialType = db->getString("type");
        } else {
            throw std::runtime_error("Material type not specified in database");
        }

        // Get material name
        std::string materialName;
        if (db->ifExist("name")) {
            materialName = db->getString("name");
        } else {
            materialName = "UnnamedMaterial";
        }

        // Get density (optional, default 0.0)
        double density = 0.0;
        if (db->ifExist("density")) {
            density = db->getDouble("density");
        }

        // Check if material type is registered
        auto it = d_creators.find(materialType);
        if (it == d_creators.end()) {
            throw std::runtime_error("Unknown material type: " + materialType);
        }

        // Create material using registered creator
        auto material = it->second(materialName, db);

        // Initialize material from database
        material->takeDB(db);

        return material;
    }

    // Create material with explicit type and parameters
    std::shared_ptr<BaseMaterial> createMaterial(
        const std::string& typeName,
        const std::string& name,
        std::shared_ptr<DataBase> db = nullptr) {

        // Check if material type is registered
        auto it = d_creators.find(typeName);
        if (it == d_creators.end()) {
            throw std::runtime_error("Unknown material type: " + typeName);
        }

        // Create material using registered creator
        auto material = it->second(name, db);

        // Initialize from database if provided
        if (db) {
            material->takeDB(db);
        }

        return material;
    }

private:
    MaterialFactory() {
        // Register built-in material types
        registerMaterialType("linear_elastic",
            [](const std::string& name, std::shared_ptr<DataBase> db) {
                double density = 0.0;
                if (db && db->ifExist("density")) {
                    density = db->getDouble("density");
                }
                return std::make_shared<LinearElastic>(name, density);
            });

        // Add more material types here as they are implemented
        // registerMaterialType("elastic_plastic", ...);
        // registerMaterialType("hyperelastic", ...);
    }

    ~MaterialFactory() = default;
    MaterialFactory(const MaterialFactory&) = delete;
    MaterialFactory& operator=(const MaterialFactory&) = delete;

    std::map<std::string, MaterialCreator> d_creators;
};

#endif // MATERIAL_FACTORY_H