#include "helicopter.h"

namespace game {

Helicopter::Helicopter(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture) : SceneNode(name, geometry, material, texture) {
}


Helicopter::~Helicopter(){
}


glm::quat Helicopter::GetAngM(void) const {

    return angm_;
}


void Helicopter::SetAngM(glm::quat angm){

    angm_ = angm;
}

void Helicopter::ApplyAngForce(glm::vec3 angularvel) {
	angularvel_ += angularvel;
}

void Helicopter::ApplyForce(glm::vec3 force) {
	velocity_ += force;
}

void Helicopter::SetKeysIn(std::map<std::string, bool> keysin) {
	keysin_ = keysin;
}

void Helicopter::Update(void){
	// apply angular momentum
	
	
	glm::vec3 tempAngles = glm::eulerAngles(orientation_);
	std::cout << tempAngles.x << ", " << tempAngles.y << ", " << tempAngles.z << std::endl;
	
	// keep upright code
	// courteousy of
	// https://answers.unity.com/questions/821033/make-my-objects-rotation-upright.html
	// https://stackoverflow.com/questions/1171849/finding-quaternion-representing-the-rotation-from-one-vector-to-another
	// https://glm.g-truc.net/0.9.4/api/a00153.html
	glm::vec3 upPosition = glm::vec3(0, -1, 0);
	glm::vec3 curUp = GetForward();
	glm::vec3 difference = glm::cross(curUp, upPosition);

	glm::quat upright;
	upright.x = difference.x; upright.y = difference.y; upright.z = difference.z;
	upright.w = sqrt(std::pow(glm::length(curUp),2) * std::pow(glm::length(upPosition),2)) + glm::dot(curUp, upPosition);
	upright *= orientation_;
	upright = glm::normalize(upright);

	orientation_ = glm::normalize(glm::slerp(orientation_, upright, 0.5f));
	// end keep upright

	// linear movement
	velocity_ += glm::vec3(0, -0.01, 0); // gravity
	velocity_ -= GetForward()*0.01f; // helicopter idle rotor thrust
	velocity_ *= glm::vec3(0.90, 0.99, 0.99);
	Translate(velocity_);
	// angular movement
	// rotate based on angular velocity
	Pitch(angularvel_.x);
	Yaw(angularvel_.y);
	Roll(angularvel_.z);

	// add friction to angular momentum
	angularvel_ = angularvel_ * 0.95f;
	
}
            
} // namespace game
