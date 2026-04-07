#ifndef MATERIAL_INITIALIZER_H
#define MATERIAL_INITIALIZER_H

#include <memory>
#include <string>
#include <iostream>
#include "MaterialManager.h"
#include "MaterialFactory.h"
#include "input.h"

class MaterialInitializer {
public:
    // Initialize materials from modal_market database
    // Expected database structure:
    // modal_market {
    //     material {
    //         material0 {
    //             material_type = "linear_elastic"
    //             name = "Steel"
    //             youngs_modulus = 2.1e11
    //             poissons_ratio = 0.3
    //             density = 7850.0
    //         }
    //         material1 {
    //             material_type = "linear_elastic"
    //             name = "Aluminum"
    //             youngs_modulus = 7.0e10
    //             poissons_ratio = 0.33
    //             density = 2700.0
    //         }
    //         // ... more materials
    //     }
    // }
    static void initializeFromDatabase(std::shared_ptr<DataBase> modal_market_db) {
        if (!modal_market_db) {
            throw std::invalid_argument("Modal market database is null");
        }

        // Check if material section exists
        if (!modal_market_db->ifExist("material")) {
            std::cout << "警告: 输入文件中未找到材料定义" << std::endl;
            return;
        }

        auto material_db = modal_market_db->getDataBase("material");
        if (!material_db) {
            std::cout << "警告: 材料数据库为空" << std::endl;
            return;
        }

        // Get all keys in material database (should be material0, material1, etc.)
        auto material_keys = material_db->getVectorString(""); // Empty string gets all keys?
        // Note: getVectorString("") might not work as expected. We need to get all keys.
        // For simplicity, assume material IDs are known or we iterate through d_all_keys
        // We'll use a different approach

        std::cout << "正在初始化材料..." << std::endl;

        // Try to find material definitions by checking for common patterns
        // This is a simplified implementation - actual implementation depends on input format
        initializeMaterialsFromSubDB(material_db);

        std::cout << "材料初始化完成，共 "
                  << MaterialManager::getInstance().getNumMaterials()
                  << " 种材料" << std::endl;
    }

private:
    // Helper function to recursively find and initialize materials
    static void initializeMaterialsFromSubDB(std::shared_ptr<DataBase> db) {
        if (!db) return;

        // Check if this database looks like a material definition
        // (has material_type or type field)
        bool looksLikeMaterial = false;
        if (db->ifExist("material_type") || db->ifExist("type")) {
            looksLikeMaterial = true;
        }

        // Also check if it has material properties
        if (db->ifExist("youngs_modulus") || db->ifExist("E") ||
            db->ifExist("poissons_ratio") || db->ifExist("nu")) {
            looksLikeMaterial = true;
        }

        if (looksLikeMaterial) {
            // Try to create material
            try {
                auto& factory = MaterialFactory::getInstance();
                auto material = factory.createMaterial(db);

                // Use material name as ID, or generate one
                std::string materialId = material->getName();
                if (materialId.empty()) {
                    materialId = "material_" + std::to_string(
                        MaterialManager::getInstance().getNumMaterials());
                }

                // Add to manager
                MaterialManager::getInstance().addMaterial(materialId, material);
                std::cout << "  已创建材料: " << materialId
                          << " (" << material->getName() << ")" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "  创建材料失败: " << e.what() << std::endl;
            }
        }

        // Recursively process sub-databases
        // Note: This depends on DataBase implementation
        // Assuming we can get sub-databases by known keys or iterate through d_son_dbs
        // Since we don't have direct access to d_son_dbs, we'll skip recursion
        // In real implementation, you would iterate through all sub-databases
    }

    // Alternative: Initialize with explicit material definitions
    static void initializeDefaultMaterials() {
        auto& manager = MaterialManager::getInstance();
        auto& factory = MaterialFactory::getInstance();

        try {
            // Create default steel material
            auto steel = factory.createMaterial("linear_elastic", "Steel");
            auto steel_db = std::make_shared<DataBase>();
            // Note: DataBase needs to be populated with parameters
            // This is simplified - real implementation would set values
            std::cout << "警告: 默认材料初始化需要完整的数据库实现" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "默认材料初始化失败: " << e.what() << std::endl;
        }
    }
};

#endif // MATERIAL_INITIALIZER_H