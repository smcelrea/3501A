#include "asteroid.h"

namespace game {

Asteroid::Asteroid(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture) : SceneNode(name, geometry, material, texture) {
}


Asteroid::~Asteroid(){
}


glm::quat Asteroid::GetAngM(void) const {

    return angm_;
}


void Asteroid::SetAngM(glm::quat angm){

    angm_ = angm;
}


void Asteroid::Update(void){

    Rotate(angm_);
}
            
} // namespace game
