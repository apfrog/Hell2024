#include "AnimatedGameObject.h"
#include "Player.h"
#include "Audio.hpp"
#include "Floorplan.h"
#include "../Util.hpp"
#include "Input.h"
#include "AssetManager.h"

void AnimatedGameObject::Update(float deltaTime) {


   
    /*if (Input::KeyPressed(HELL_KEY_L)) {
    std::cout << "\nANIMATED TRANSFORM NAMES\n";
    for (int i = 0; i < _animatedTransforms.names.size(); i++) {
        std::string boneName = _animatedTransforms.names[i];
        std::cout << " " << boneName << "\n";
    }
    Ragdoll& ragdoll = _ragdoll;
    std::cout << "\nRIGID NAMES\n";
    for (int i = 0; i < ragdoll._rigidComponents.size(); i++) {
        RigidComponent& rigid = ragdoll._rigidComponents[i];
        std::string name = rigid.name;
        std::cout << " " << name << "\n";
    }
    std::cout << "\nRIGID CORRESPONDING JOINT NAMES\n";
    for (int i = 0; i < ragdoll._rigidComponents.size(); i++) {
        RigidComponent& rigid = ragdoll._rigidComponents[i];
        std::string name = rigid.correspondingJointName;
        std::cout << " " << name << "\n";
    }
    std::cout << "\nJOINT NAMES\n";
    for (int i = 0; i < ragdoll._jointComponents.size(); i++) {
        JointComponent& joint = ragdoll._jointComponents[i];
        std::string name = joint.name;
        std::cout << " " << name << "\n";
    }
    std::cout << "\n\n";
    }*/

 

    if (Input::KeyPressed(HELL_KEY_L) && _hasRagdoll) {
    //    _animationMode = RAGDOLL;
    }

    if (_animationMode == ANIMATION) {
        if (_currentAnimation) {
            UpdateAnimation(deltaTime);
            CalculateBoneTransforms();
            SetRagdollToCurrentAniamtionState();
        }
    }
    else if (_animationMode == BINDPOSE && _skinnedModel) {
        _skinnedModel->UpdateBoneTransformsFromBindPose(_animatedTransforms);
        SetRagdollToCurrentAniamtionState();
    }
    else if (_animationMode == RAGDOLL && _skinnedModel) {
        UpdateBoneTransformsFromRagdoll();
    }
}

void AnimatedGameObject::ToggleAnimationPause() {
    _animationPaused = !_animationPaused;
}

void AnimatedGameObject::PlayAndLoopAnimation(std::string animationName, float speed) {
       
    if (!_skinnedModel) {
        //std::cout << "could not play animation cause skinned model was nullptr\n";
        return;
    }

    // Find the matching animation name if it exists
    for (int i = 0; i < _skinnedModel->m_animations.size(); i++) {
        if (_skinnedModel->m_animations[i]->_filename == animationName) {

            // If the animation isn't already playing, set the time to 0
            Animation* animation = _skinnedModel->m_animations[i];
            if (_currentAnimation != animation) {
                _currentAnimationTime = 0;
                _animationPaused = false;
            }
            // Update the current animation with this one
            _currentAnimation = animation;

            // Reset flags
            _loopAnimation = true;
            _animationIsComplete = false;
            _animationMode = ANIMATION;

            // Update the speed
            _animationSpeed = speed;
            return;
        }
    }
    // Not found
    std::cout << animationName << " not found!\n";
}

void AnimatedGameObject::PauseAnimation() {
    _animationPaused = true;
}

void AnimatedGameObject::SetMeshMaterial(std::string meshName, std::string materialName) {
    if (!_skinnedModel) {
        return;
    }
    for (int i = 0; i < _skinnedModel->m_meshEntries.size(); i++) {
        auto& mesh = _skinnedModel->m_meshEntries[i];
        if (mesh.Name == meshName) {
            _materialIndices[i] = AssetManager::GetMaterialIndex(materialName);
            //return;
        }
    }
}

void AnimatedGameObject::SetMeshMaterialByIndex(int meshIndex, std::string materialName) {
    if (!_skinnedModel) {
        return;
    }
    if (meshIndex >= 0 && meshIndex < _materialIndices.size()) {
        _materialIndices[meshIndex] = AssetManager::GetMaterialIndex(materialName);
    }
}

void AnimatedGameObject::SetMaterial(std::string materialName) {
    if (!_skinnedModel) {
        return;
    }
    for (int i = 0; i < _skinnedModel->m_meshEntries.size(); i++) {
        //auto& mesh = _skinnedModel->m_meshEntries[i];
        _materialIndices[i] = AssetManager::GetMaterialIndex(materialName);
    }
}

glm::mat4 AnimatedGameObject::GetBoneWorldMatrixFromBoneName(std::string name) {
	for (int i = 0; i < _animatedTransforms.names.size(); i++) {
        if (_animatedTransforms.names[i] == name) {
            return _animatedTransforms.worldspace[i];
        }
    }
    std::cout << "GetBoneWorldMatrixFromBoneName() failed to find name " << name << "\n";
    return glm::mat4();
}

void AnimatedGameObject::SetAnimationModeToBindPose() {
    _animationMode = BINDPOSE;
}

void AnimatedGameObject::SetAnimatedModeToRagdoll() {
    _animationMode = RAGDOLL;
}

void AnimatedGameObject::PlayAnimation(std::string animationName, float speed) {    
    // Find the matching animation name if it exists
    for (int i = 0; i < _skinnedModel->m_animations.size(); i++) {
        if (_skinnedModel->m_animations[i]->_filename == animationName) {       
            _currentAnimationTime = 0;           
            _currentAnimation = _skinnedModel->m_animations[i];
            _loopAnimation = false;
            _animationSpeed = speed;
            _animationPaused = false;
            _animationIsComplete = false; 
            _animationMode = ANIMATION;
            return;
        }
    }
    // Not found
    std::cout << animationName << " not found!\n";
}

void AnimatedGameObject::UpdateAnimation(float deltaTime) {

    float duration = _currentAnimation->m_duration / _currentAnimation->m_ticksPerSecond;

    // Increase the animation time
    if (!_animationPaused) {
        _currentAnimationTime += deltaTime * _animationSpeed;
    }
    // Animation is complete?
    if (_currentAnimationTime > duration) {
        if (!_loopAnimation) {
            _currentAnimationTime = duration;
            _animationPaused = true;
            _animationIsComplete = true;
        }
        else {
            _currentAnimationTime = 0;
        }
    }
}

//void SkinnedModel::UpdateBoneTransformsFromAnimation(float animTime, Animation* animation, AnimatedTransforms& animatedTransforms, glm::mat4& outCameraMatrix)

float GetAnimationTime(SkinnedModel* skinnedModel, float animTime, Animation* animation) {
    float AnimationTime = 0;
    if (skinnedModel->m_animations.size() > 0) {
        float TicksPerSecond = animation->m_ticksPerSecond != 0 ? animation->m_ticksPerSecond : 25.0f;
        float TimeInTicks = animTime * TicksPerSecond;
        AnimationTime = fmod(TimeInTicks, animation->m_duration);
        AnimationTime = std::min(TimeInTicks, animation->m_duration);
    }
    return AnimationTime;
}

void AnimatedGameObject::CalculateBoneTransforms() {
    //_skinnedModel->UpdateBoneTransformsFromAnimation(_currentAnimationTime, _currentAnimation, _animatedTransforms, _cameraMatrix);

    // Get the animation time
    float AnimationTime = GetAnimationTime(_skinnedModel, _currentAnimationTime, _currentAnimation);
    auto m_animations = _skinnedModel->m_animations;
    auto m_BoneMapping = _skinnedModel->m_BoneMapping;
    auto m_BoneInfo = _skinnedModel->m_BoneInfo;

  // _debugBones.clear();

    struct JointWorldMatrix {
        std::string name;
        glm::mat4 worldMatrix;
    };

    std::vector<JointWorldMatrix> jointWorldMatrices;
    jointWorldMatrices.resize(_skinnedModel->m_joints.size());

    // Traverse the tree 
    for (int i = 0; i < _skinnedModel->m_joints.size(); i++) {

        // Get the node and its um bind pose transform?
        const char* NodeName = _skinnedModel->m_joints[i].m_name;
        glm::mat4 NodeTransformation = _skinnedModel->m_joints[i].m_inverseBindTransform;
                       
        // Calculate any animation
        if (m_animations.size() > 0) {

            const AnimatedNode* animatedNode = _skinnedModel->FindAnimatedNode(_currentAnimation, NodeName);

            if (animatedNode) {
                glm::vec3 Scaling;
                _skinnedModel->CalcInterpolatedScaling(Scaling, AnimationTime, animatedNode);
                glm::mat4 ScalingM;
                ScalingM = Util::Mat4InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
                glm::quat RotationQ;
                _skinnedModel->CalcInterpolatedRotation(RotationQ, AnimationTime, animatedNode);
                glm::mat4 RotationM(RotationQ);
                glm::vec3 Translation;
                _skinnedModel->CalcInterpolatedPosition(Translation, AnimationTime, animatedNode);
                glm::mat4 TranslationM;
                ScalingM = glm::mat4(1);
                TranslationM = Util::Mat4InitTranslationTransform(Translation.x, Translation.y, Translation.z);
                NodeTransformation = TranslationM * RotationM * ScalingM;
            }
        }
        unsigned int parentIndex = _skinnedModel->m_joints[i].m_parentIndex;

        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : _skinnedModel->m_joints[parentIndex].m_currentFinalTransform;
        glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

        jointWorldMatrices[i].worldMatrix = GlobalTransformation;
        jointWorldMatrices[i].name = NodeName;

       // glm::vec3 point = GetModelMatrix() * GlobalTransformation * glm::vec4(0, 0, 0, 1.0);
       // _debugBones.push_back(point);

        // Store the current transformation, so child nodes can access it
        _skinnedModel->m_joints[i].m_currentFinalTransform = GlobalTransformation;

        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
            m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;
        }
    }


    for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {


        bool found = false;
        RigidComponent& rigid = _ragdoll._rigidComponents[j];

        /*
        if (Input::KeyPressed(HELL_KEY_J)) {
            std::cout << "\n";
            //std::cout << "_skinnedModel->m_BoneInfo.size(): " << _skinnedModel->m_BoneInfo.size() << "\n";
            //std::cout << "_skinnedModel->m_joints.size():   " << _skinnedModel->m_joints.size() << "\n";
            std::cout << "jointWorldMatrices.size():   " << jointWorldMatrices.size() << "\n";
        }*/

            
        for (int i = 0; i < jointWorldMatrices.size(); i++) {

            std::string& NodeName = jointWorldMatrices[i].name;
            /*
            if (Input::KeyPressed(HELL_KEY_J)) {
                std::cout << i << ": " << NodeName << "\n";
            }
            */
            if (rigid.correspondingJointName == NodeName) {

                glm::mat4 m = GetModelMatrix() * jointWorldMatrices[i].worldMatrix;
                PxMat44 mat = Util::GlmMat4ToPxMat44(m);
                PxTransform pose(mat);
                rigid.pxRigidBody->setGlobalPose(pose);
                rigid.pxRigidBody->putToSleep();
                found = true;
                break;
            }               
        }

        if (!found) {

            if (Input::KeyPressed(HELL_KEY_SPACE)) {
                std::cout << j << ": " << rigid.correspondingJointName << " was not found\n";
            }

            //std::cout << NodeName << " was not found"
        }
        
    }

    _animatedTransforms.Resize(_skinnedModel->m_NumBones);

    for (unsigned int i = 0; i < _skinnedModel->m_NumBones; i++) {
        _animatedTransforms.local[i] = m_BoneInfo[i].FinalTransformation;
        _animatedTransforms.worldspace[i] = m_BoneInfo[i].ModelSpace_AnimatedTransform;
        _animatedTransforms.names[i] = m_BoneInfo[i].BoneName;
        //animatedTransforms.inverseBindTransform[i] = m_BoneInfo[i].BoneOffset;
    }



}

glm::mat4 AnimatedGameObject::GetModelMatrix() {

    Transform correction;
    
    if (_skinnedModel->_filename == "AKS74U" 
        || _skinnedModel->_filename == "Glock"
       // || _skinnedModel->_filename == "Shotgun"
        ) {
        correction.rotation.y = HELL_PI;
    }

    // THIS IS A HAAAAAAACK TO FIX THE MODELS BEING BACKWARDS 180 degrees. 
    // Make it toggleable so not all animated models are flipped
    return _transform.to_mat4() * correction.to_mat4();
}

bool AnimatedGameObject::IsAnimationComplete() {
    return _animationIsComplete;
}

std::string AnimatedGameObject::GetName() {
    return _name;
}

void AnimatedGameObject::SetName(std::string name) {
    _name = name;
}

void AnimatedGameObject::SetSkinnedModel(std::string name) {
    SkinnedModel* skinnedModel = AssetManager::GetSkinnedModel(name);
    if (skinnedModel) {
        _skinnedModel = skinnedModel;
        _materialIndices.resize(skinnedModel->m_meshEntries.size());

        /*std::cout << "SetSkinnedModel() " << name << "\n";
        for (int i = 0; i < skinnedModel->m_meshEntries.size(); i++) {
            std::cout << "-" << skinnedModel->m_meshEntries[i].Name << "\n";
        }*/
    }
    else {
        std::cout << "Could not SetSkinnedModel(name) with name: \"" << name << "\", it does not exist\n";
    }
}

glm::vec3 AnimatedGameObject::GetScale() {
    return  _transform.scale;
}

void AnimatedGameObject::SetScale(float scale) {
    _transform.scale = glm::vec3(scale);
}
void AnimatedGameObject::SetPosition(glm::vec3 position) {
    _transform.position = position;
}

void AnimatedGameObject::SetRotationX(float rotation) {
    _transform.rotation.x = rotation;
}

void AnimatedGameObject::SetRotationY(float rotation) {
    _transform.rotation.y = rotation;
}

void AnimatedGameObject::SetRotationZ(float rotation) {
    _transform.rotation.z = rotation;
}

bool AnimatedGameObject::AnimationIsPastPercentage(float percent) {
    if (_currentAnimationTime * _currentAnimation->GetTicksPerSecond() > _currentAnimation->m_duration * (percent / 100.0))
        return true;
    else
        return false;
}





glm::vec3 AnimatedGameObject::GetAK74USCasingSpawnPostion() {
    if (_name == "AKS74U") {
        int boneIndex = _skinnedModel->m_BoneMapping["Weapon"];
        if (_animatedTransforms.worldspace.size()) {
            glm::mat4 boneMatrix = _animatedTransforms.worldspace[boneIndex];
            Transform offset;
            offset.position = glm::vec3(-2.0f, 2.0f, -2.0f); // real

            static float x2 = -44.0f;
            static float y2 = -28.0f;
            static float z2 = 175.0f;
            float amount = 0.5f;
            if (Input::KeyDown(HELL_KEY_LEFT)) {
                x2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT)) {
                x2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_LEFT_BRACKET)) {
                y2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT_BRACKET)) {
                y2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_UP)) {
                z2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_DOWN)) {
                z2 += amount;
            }
             //std::cout << x2 << ", " << y2 << ", " << z2 << "\n";

             //offset.position = glm::vec3(x2, y2, z2); // hack to look good
            offset.position = glm::vec3(-95, -54, 236.5); // hack to look good

            
            glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
            float x = m[3][0];
            float y = m[3][1];
            float z = m[3][2];
            return glm::vec3(x, y, z);
        }
    }
    return glm::vec3(0);
}

glm::vec3 AnimatedGameObject::GetGlockCasingSpawnPostion() {
    if (_name == "Glock") {
        int boneIndex = _skinnedModel->m_BoneMapping["Barrel"];
        if (_animatedTransforms.worldspace.size()) {
            glm::mat4 boneMatrix = _animatedTransforms.worldspace[boneIndex];
            Transform offset;
            offset.position = glm::vec3(-2.0f, 2.0f, -2.0f); // real

            static float x2 = -44.0f;
            static float y2 = -28.0f;
            static float z2 = 175.0f;
            float amount = 0.5f;
            if (Input::KeyDown(HELL_KEY_LEFT)) {
                x2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT)) {
                x2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_LEFT_BRACKET)) {
                y2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT_BRACKET)) {
                y2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_UP)) {
                z2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_DOWN)) {
                z2 += amount;
            }
          // std::cout << x2 << ", " << y2 << ", " << z2 << "\n";

           // offset.position = glm::vec3(x2, y2, z2); // hack to look good
            offset.position = glm::vec3(-90.5, -29.5, 267.5); // hack to look good
            
            glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
            float x = m[3][0];
            float y = m[3][1];
            float z = m[3][2];
            return glm::vec3(x, y, z);
        }
    }
    return glm::vec3(0);
}



glm::vec3 AnimatedGameObject::GetAKS74UBarrelPostion() {
    if (_name == "AKS74U") {
        int boneIndex = _skinnedModel->m_BoneMapping["Weapon"];
        glm::mat4 boneMatrix = _animatedTransforms.worldspace[boneIndex];
        Transform offset;
        offset.position = glm::vec3(0, 0 + 1, 36);
        glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
        float x = m[3][0];
        float y = m[3][1];
        float z = m[3][2];
        return glm::vec3(x, y, z);
    }
    else {
        return glm::vec3(0);
    }
}

glm::vec3 AnimatedGameObject::GetShotgunBarrelPosition() {
   // if (_name == "ShotgunTest") {
        int boneIndex = _skinnedModel->m_BoneMapping["Weapon"];
        glm::mat4 boneMatrix = _animatedTransforms.worldspace[boneIndex];
        Transform offset;
        offset.position = glm::vec3(82, 2, -10);
        glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
        float x = m[3][0];
        float y = m[3][1];
        float z = m[3][2];
     //  std::cout << "heDDDDDDDDDDDDDDDDDDDDDllon\n";
        return glm::vec3(x, y, z);
//    }
 //   else {
        return glm::vec3(0);
 //   }
}

void AnimatedGameObject::UpdateBoneTransformsFromBindPose() {

    // Traverse the tree 
    auto& joints = _skinnedModel->m_joints;

    for (int i = 0; i < joints.size(); i++) {

        // Get the node and its um bind pose transform?
        const char* NodeName = joints[i].m_name;
        glm::mat4 NodeTransformation = joints[i].m_inverseBindTransform;

        unsigned int parentIndex = joints[i].m_parentIndex;

        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : joints[parentIndex].m_currentFinalTransform;
        glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

        joints[i].m_currentFinalTransform = GlobalTransformation;

        if (_skinnedModel->m_BoneMapping.find(NodeName) != _skinnedModel->m_BoneMapping.end()) {
            unsigned int BoneIndex = _skinnedModel->m_BoneMapping[NodeName];
            _skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * _skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
            _skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;
        }
    }

    _debugTransformsA.resize(joints.size());
    _debugTransformsB.resize(joints.size());

    for (unsigned int i = 0; i < _skinnedModel->m_NumBones; i++) {
        _debugTransformsA[i] = _skinnedModel->m_BoneInfo[i].FinalTransformation;
        _debugTransformsB[i] = _skinnedModel->m_BoneInfo[i].ModelSpace_AnimatedTransform;
        _animatedTransforms.names[i] = _skinnedModel->m_BoneInfo[i].BoneName;
    }
}

void AnimatedGameObject::LoadRagdoll(std::string filename, PxU32 ragdollCollisionFlags) {
    _ragdoll.LoadFromJSON(filename, ragdollCollisionFlags);
    _hasRagdoll = true;
}

void AnimatedGameObject::SetRagdollToCurrentAniamtionState() {

    

    
    return;

    if (_hasRagdoll) {

        if (Input::KeyPressed(HELL_KEY_L)) {

            std::cout << "\nWORLDSPACE TRANSFORM NAMES: \n";
            for (auto name : _animatedTransforms.names) {
                std::cout << " " << name << "\n";
            }

            std::cout << "\nRIGID NAMES: \n";
            for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {
                RigidComponent& rigid = _ragdoll._rigidComponents[j];
                std::cout << " " << j << ": " << rigid.name << "\n";
            }
            std::cout << "\nRIGID CORRESPONDING BONE NAMES: \n";
            for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {
                RigidComponent& rigid = _ragdoll._rigidComponents[j];
                std::cout << " " << j << ": " << rigid.correspondingJointName << "\n";
            }
        }
        
        for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {

            RigidComponent& rigid = _ragdoll._rigidComponents[j];
            bool found = false;

           /* if (rigid.name == "rMarker_CC_Base_L_Calf") {
                rigid.correspondingJointName = "CC_Base_L_CalfTwist01";
                rigid.correspondingJointName = "CC_Base_L_CalfTwist02";
            }
            if (rigid.name == "rMarker_CC_Base_R_Calf") {
                rigid.correspondingJointName = "CC_Base_R_CalfTwist01";

                
            }
            if (rigid.name == "rMarker_CC_Base_L_Thigh") {
                rigid.correspondingJointName = "CC_Base_L_ThighTwist01";
            }
            if (rigid.name == "rMarker_CC_Base_R_Thigh") {
                rigid.correspondingJointName = "CC_Base_R_ThighTwist01";
            }
            if (rigid.name == "rMarker_CC_Base_Hip") {
                rigid.correspondingJointName = "CC_Base_Pelvis";
            }
            if (rigid.name == "rMarker_CC_Base_L_Upperarm") {
                rigid.correspondingJointName = "CC_Base_L_UpperarmTwist01";
            }
            if (rigid.name == "rMarker_CC_Base_R_Upperarm") {
                rigid.correspondingJointName = "CC_Base_R_UpperarmTwist01";
            }
            if (rigid.name == "rMarker_CC_Base_R_Forearm") {
                rigid.correspondingJointName = "CC_Base_R_ForearmTwist02";
            }
            if (rigid.name == "rMarker_CC_Base_FacialBone") {
             //   rigid.correspondingJointName = "CC_Base_Head";
            }
            */
            for (int i = 0; i < _animatedTransforms.names.size(); i++) {

                std::string transformName = _animatedTransforms.names[i];
                glm::mat4 worldTransform = _animatedTransforms.worldspace[i];
                if (transformName == rigid.correspondingJointName) {
                    glm::mat4 m = GetModelMatrix() * worldTransform;
                    PxMat44 mat = Util::GlmMat4ToPxMat44(m);
                    PxTransform pose(mat);
                    rigid.pxRigidBody->setGlobalPose(pose);
                    rigid.pxRigidBody->putToSleep();
                    found = true;
                    break;
                }
            }

            if (!found) {
            //    std::cout << "No match for rigid: " << rigid.name << " / " << rigid.correspondingJointName << "\n";
            }
        }
    } 
    else {
        //std::cout << "You called SetRagdollToCurrentAniamtionState() on a AnimatedGameObject with name '" << GetName() << "' but it doesn't have a valid ragdoll!!!\n";
    }
}

void AnimatedGameObject::DestroyRagdoll() {
    for (JointComponent& joint: _ragdoll._jointComponents) {
        if (joint.pxD6) {
            joint.pxD6->release();
        }
    }
    for (RigidComponent& rigid : _ragdoll._rigidComponents) {
        if (rigid.pxRigidBody) {
            rigid.pxRigidBody->release();
        }
    }
}

void AnimatedGameObject::WipeAllSkippedMeshIndices() {
    _skippedMeshIndices.clear();
}

void AnimatedGameObject::AddSkippedMeshIndexByName(std::string meshName) {
    for (int i = 0; i < _skinnedModel->m_meshEntries.size(); i++) {
        if (_skinnedModel->m_meshEntries[i].Name == meshName) {
            _skippedMeshIndices.push_back(i);
            //std::cout << "SKPPING MESH " << i << " " << meshName << "!\n";
            return;
        }
    }
    std::cout << "AddSkippedMeshIndexByName() called but mesh " << meshName << " was not found!\n";
}

void AnimatedGameObject::PrintMeshNames() {
    std::cout << _skinnedModel->_filename << "\n";
    for (int i = 0; i < _skinnedModel->m_meshEntries.size(); i++) {
        std::cout << "-" << _skinnedModel->m_meshEntries[i].Name << "\n";
    } 
}


void AnimatedGameObject::UpdateBoneTransformsFromRagdoll() {

    if (false)
    {
       // _debugBones.clear();

        struct JointWorldMatrix {
            std::string name;
            glm::mat4 worldMatrix;
            glm::mat4 localMatrix;
        };
        std::vector<JointWorldMatrix> jointWorldMatrices;
        jointWorldMatrices.resize(_skinnedModel->m_joints.size());

        auto m_animations = _skinnedModel->m_animations;
        auto m_BoneMapping = _skinnedModel->m_BoneMapping;
        auto m_BoneInfo = _skinnedModel->m_BoneInfo;

        // Traverse the tree 
        for (int i = 0; i < _skinnedModel->m_joints.size(); i++) {

            // Get the node and its um bind pose transform?
            const char* NodeName = _skinnedModel->m_joints[i].m_name;
            glm::mat4 NodeTransformation = _skinnedModel->m_joints[i].m_inverseBindTransform;

            // Calculate any animation
           // bool matchingRigidFound = false;


            for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {

                RigidComponent& rigid = _ragdoll._rigidComponents[j];
                if (rigid.correspondingJointName == NodeName) {
                    PxMat44 globalPose = rigid.pxRigidBody->getGlobalPose();
                    NodeTransformation = Util::PxMat44ToGlmMat4(globalPose);


                     glm::vec3 point = NodeTransformation * glm::vec4(0, 0, 0, 1.0);
                     _debugBones.push_back(point);

                    break;
                }
            }

            unsigned int parentIndex = _skinnedModel->m_joints[i].m_parentIndex;

            glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : _skinnedModel->m_joints[parentIndex].m_currentFinalTransform;
            glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

            jointWorldMatrices[i].worldMatrix = GlobalTransformation;
            jointWorldMatrices[i].localMatrix = GlobalTransformation * _skinnedModel->m_joints[i].m_inverseBindTransform;
            jointWorldMatrices[i].name = NodeName;

            glm::vec3 point = GetModelMatrix() * GlobalTransformation * glm::vec4(0, 0, 0, 1.0);
            _debugBones.push_back(point);

            // Store the current transformation, so child nodes can access it
            _skinnedModel->m_joints[i].m_currentFinalTransform = GlobalTransformation;

            if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
                unsigned int BoneIndex = m_BoneMapping[NodeName];
                m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
                m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;
            }
        }


        _animatedTransforms.Resize(_skinnedModel->m_NumBones);


        if (Input::KeyPressed(HELL_KEY_J)) {
            std::cout << "\nANIMATED TRANSFORM NAMES\n";
        }

        for (unsigned int i = 0; i < _skinnedModel->m_NumBones; i++) {

            if (Input::KeyPressed(HELL_KEY_J)) {
                std::cout << i << ": " << _animatedTransforms.names[i] << "\n";
            }

            std::string& transformName = _animatedTransforms.names[i];

            for (int j = 0; j < jointWorldMatrices.size(); j++) {

                std::string& jointName = jointWorldMatrices[j].name;
                glm::mat4& localMatrix = jointWorldMatrices[j].localMatrix;

                if (jointName == transformName) {
                    _animatedTransforms.local[i] = localMatrix;
                    break;
                }      

            }
        }

    }





  

    auto& m_joints = _skinnedModel->m_joints;
    auto& m_BoneMapping = _skinnedModel->m_BoneMapping;
    auto& m_BoneInfo = _skinnedModel->m_BoneInfo;
    auto& m_NumBones = _skinnedModel->m_NumBones;



    if (Input::KeyPressed(HELL_KEY_J)) {
        std::cout << " \n\n\n\n";
    }

        for (int i = 0; i < m_joints.size(); i++)
    {



        // Get the node and its um bind pose transform?
        std::string NodeName = m_joints[i].m_name;



        if (Input::KeyPressed(HELL_KEY_J)) {
        //    std::cout << "" << i << " " << NodeName << "\n";
        }





        glm::mat4 NodeTransformation = m_joints[i].m_inverseBindTransform;

        unsigned int parentIndex = m_joints[i].m_parentIndex;

        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : m_joints[parentIndex].m_currentFinalTransform;
        glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

        m_joints[i].m_currentFinalTransform = GlobalTransformation;


        bool found = false;

        for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {
            RigidComponent& rigid = _ragdoll._rigidComponents[j];

            if (NodeName == rigid.correspondingJointName) {

                //std::cout << j << ": " << NodeName << "\n";

                PxRigidDynamic* rigidBody = rigid.pxRigidBody;
                glm::mat4 matrix = Util::PxMat44ToGlmMat4(rigidBody->getGlobalPose());


                rigidBody->wakeUp();
                glm::mat4 bindPose = m_joints[i].m_inverseBindTransform;

                GlobalTransformation = matrix;// *glm::inverse(bindPose);
                  
           //     glm::mat4 worldMatrix = matrix * bindPose;
                m_joints[i].m_currentFinalTransform = matrix;// *bindPose;

                found = true;


                if (NodeName == "CC_Base_R_Foot") {

                    auto p = rigidBody->getGlobalPose().p;
                    auto q = rigidBody->getGlobalPose().q;

    //                std::cout << "\n";
//                    std::cout << p.x << ", " << p.y << ", " << p.z << "\n";
  //                  std::cout << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "\n";

                }


//                GlobalTransformation = matrix * glm::inverse(bindPose);
            //    m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;


                glm::vec3 point = matrix * glm::vec4(0, 0, 0, 1.0);
                _debugRigids.push_back(point);

                //std::cout << i << " "  << Util::Vec3ToString(point) << "\n";

              //  found = true;
                break;

            }
        }


        glm::vec3 point = m_joints[i].m_currentFinalTransform * glm::vec4(0, 0, 0, 1.0);
        _debugBones.push_back(point);


        if (!found) {

         //   m_joints[i].m_currentFinalTransform = m_joints[i].m_inverseBindTransform;
        }





        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {

            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
            m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;



            if (Input::KeyPressed(HELL_KEY_J)) {
            //          std::cout << "  " << i << " " << NodeName << ": " << m_BoneInfo[BoneIndex].BoneName << "\n";
            }

        }

        /*
        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
            m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;





            PxRigidDynamic* rigidBody = nullptr;

            bool found = false;

            for (int j = 0; j < _ragdoll._rigidComponents.size(); j++) {
                RigidComponent& rigid = _ragdoll._rigidComponents[j];

                if (NodeName == rigid.correspondingJointName) {
                    rigidBody = rigid.pxRigidBody;
                }

                if (rigidBody) {
                    glm::mat4 matrix = Util::PxMat44ToGlmMat4(rigidBody->getGlobalPose());
                    rigidBody->wakeUp();
                    glm::mat4 bindPose = m_BoneInfo[BoneIndex].BoneOffset;
                    m_BoneInfo[BoneIndex].FinalTransformation = matrix * bindPose;
                    m_joints[i].m_currentFinalTransform = matrix;
                    m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
                    found = true;
                    break;
                }

            }
        }*/

    }

    // Update the actual animated transforms, with the ragdoll global poses
    for (unsigned int i = 0; i < m_NumBones; i++) {
        _animatedTransforms.local[i] = m_BoneInfo[i].FinalTransformation;

        if (Input::KeyPressed(HELL_KEY_J)) {
            std::cout << i << ": " << _animatedTransforms.names[i] << " " << m_BoneInfo[i].BoneName << "\n";
        }
    }

}