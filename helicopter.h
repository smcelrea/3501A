#ifndef HELICOPTER_H_
#define HELICOPTER_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class Helicopter : public SceneNode {

        public:
            // Create asteroid from given resources
            Helicopter(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture);
			Helicopter(const SceneNode &nodeCpy);

            // Destructor
            ~Helicopter();
            
            // Get/set attributes specific to Helicopters
            glm::quat GetAngM(void) const;
            void SetAngM(glm::quat angm);
			void SetKeysIn(std::map<std::string, bool> keysin);
			void ApplyAngForce(glm::vec3 angularvel); // vec3 ( pitch, yaw, roll )
			void ApplyForce(glm::vec3 force);
			//void Pitch(float angle);
			//void Yaw(float angle);
			//void Roll(float angle);

            // Update geometry configuration
            void Update(void);
            
        private:
            // Angular momentum of asteroid
            glm::quat angm_;
			glm::vec3 angularvel_;
			glm::vec3 velocity_;
			glm::vec3 acceleration;
			std::map<std::string, bool> keysin_;
    }; // class Helicopter

} // namespace game

#endif // HELICOPTER_H_
